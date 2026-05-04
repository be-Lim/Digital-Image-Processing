#pragma warning(disable:4996) //보안상 기존에 사용하던 입력함수의 문제 해결하는 전처리문
#include <stdio.h>
#include <stdlib.h> //동적할당에 필요한 헤더파일
#include <Windows.h> //BMP 헤더파일의 구조체 파일
#include <math.h>

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, int W, int H, BYTE* Output, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W * H, fp);
	fclose(fp);
}

void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

void VerticalFlip(BYTE* Img, BYTE* Out, BYTE* Tmp, int W, int H)
{
	for (int i = 0; i < W * H; i++)
		Tmp[i] = Img[i];
	for (int i = 0; i < H / 2; i++) {	// y좌표
		for (int j = 0; j < W; j++) {	// x좌표
			swap(&Tmp[i * W + j], &Tmp[(H - 1 - i) * W + j]);
		}
	}
	for (int i = 0; i < W * H; i++)
		Out[i] = Tmp[i];
}

void HorizontalFlip(BYTE* Img, BYTE* Out, BYTE* Tmp, int W, int H)
{
	for (int i = 0; i < W * H; i++)
		Tmp[i] = Img[i];
	for (int i = 0; i < W / 2; i++) {	// x좌표
		for (int j = 0; j < H; j++) {	// y좌표
			swap(&Tmp[j * W + i], &Tmp[j * W + (W - 1 - i)]);
		}
	}
	for (int i = 0; i < W * H; i++)
		Out[i] = Tmp[i];
}

void Translation(BYTE* Img, BYTE* Out, BYTE* Tmp, int W, int H, int Tx, int Ty)
{
	for (int i = 0; i < W * H; i++)
		Tmp[i] = 0;
	Ty *= -1;	// BMP 영상은 위아래가 거꾸로 뒤집혀있다
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H) && (i + Ty >= 0) && (j + Tx < W) && (j + Tx >= 0))
				Tmp[(i + Ty) * W + (j + Tx)] = Img[i * W + j];
		}
	}
	for (int i = 0; i < W * H; i++)
		Out[i] = Tmp[i];
}

void Scaling(BYTE* Img, BYTE* Out, BYTE* Tmp, int W, int H, double SF_X, double SF_Y)
{
	for (int i = 0; i < W * H; i++)
		Tmp[i] = 0;
	int tmpX, tmpY;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(j / SF_X);		// 역방향사상이므로 역연산 시행
			tmpY = (int)(i / SF_Y);
			if ((tmpY < H) && (tmpX < W))
				Tmp[i * W + j] = Img[tmpY * W + tmpX];
		}
	}
	for (int i = 0; i < W * H; i++)
		Out[i] = Tmp[i];
}

// 영상의 원점을 중심으로 반시계방향 회전
void Rotation_zero(BYTE* Img, BYTE* Out, BYTE* Tmp, int W, int H, int Angle)
{
	for (int i = 0; i < W * H; i++)
		Tmp[i] = 0;
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * j + sin(Radian) * i); 	// 역방향사상이므로 역연산 시행
			tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
			if ((tmpY < H) && (tmpY >= 0) && (tmpX < W) && (tmpX >= 0))
				Tmp[i * W + j] = Img[tmpY * W + tmpX];
		}
	}
	for (int i = 0; i < W * H; i++)
		Out[i] = Tmp[i];
}

// 영상의 중심에 대한 반시계 방향 회전
void Rotation_center(BYTE* Img, BYTE* Out, BYTE* Tmp, int W, int H, int Angle)
{
	for (int i = 0; i < W * H; i++)
		Tmp[i] = 0;
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	int X_center = (int)(W / 2);
	int Y_center = (int)(H / 2);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * (j - X_center) + sin(Radian) * (i - Y_center) + X_center); 	// 역방향사상이므로 역연산 시행
			tmpY = (int)(-sin(Radian) * (j - X_center) + cos(Radian) * (i - Y_center) + Y_center);
			if ((tmpY < H) && (tmpY >= 0) && (tmpX < W) && (tmpX >= 0))
				Tmp[i * W + j] = Img[tmpY * W + tmpX];
		}
	}
	for (int i = 0; i < W * H; i++)
		Out[i] = Tmp[i];
}

int main()
{
	/* 영상 입력 */
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)

	FILE* fp;
	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}

	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp); //배열의 이름 자체가 주소

	int W = hInfo.biWidth;
	int H = hInfo.biHeight;
	int ImgSize = W * H;

	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Temp = (BYTE*)malloc(ImgSize);
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	VerticalFlip(Image, Output, Temp, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "vertical_flip.bmp");

	HorizontalFlip(Image, Output, Temp, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "horizontal_flip.bmp");

	Translation(Image, Output, Temp, W, H, 50, 30);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "translation.bmp");

	Scaling(Image, Output, Temp, W, H, 1.5, 1.2);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "scaling_up.bmp");

	Scaling(Image, Output, Temp, W, H, 0.5, 0.7);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "scaling_down.bmp");

	Rotation_zero(Image, Output, Temp, W, H, 30);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "rotation_zero.bmp");

	Rotation_center(Image, Output, Temp, W, H, 30);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "rotation_center.bmp");


	free(Image);
	free(Temp);
	free(Output);

	return 0;
}
