#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, int W, int H, BYTE* Output, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	if (hInfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W * H * 3, fp);
	}
	else {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W * H, fp);
	}
	fclose(fp);
}

void FillColor(BYTE* Img, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B)
{
	Img[Y * W * 3 + X * 3] = B; // Blue 성분
	Img[Y * W * 3 + X * 3 + 1] = G; // Green 성분
	Img[Y * W * 3 + X * 3 + 2] = R; // Red 성분
}

// Red 값이 큰 화소만 masking (RGB 모델 기준)
void Red_Masking_RGB(BYTE* Img, BYTE* Out, int W, int H, BYTE R, BYTE G, BYTE B)
{
	for (int i = 0; i < H; i++) {	// Y좌표
		for (int j = 0; j < W; j++) {	// X좌표
			if (Img[i * W * 3 + j * 3 + 2] > R &&	// 경험적으로 얻은 수치 (130, 50, 100)
				Img[i * W * 3 + j * 3 + 1] < G &&
				Img[i * W * 3 + j * 3 + 0] < B) {
				Out[i * W * 3 + j * 3] = Img[i * W * 3 + j * 3];
				Out[i * W * 3 + j * 3 + 1] = Img[i * W * 3 + j * 3 + 1];
				Out[i * W * 3 + j * 3 + 2] = Img[i * W * 3 + j * 3 + 2];
			}
			else {
				Out[i * W * 3 + j * 3] = 0;
				Out[i * W * 3 + j * 3 + 1] = 0;
				Out[i * W * 3 + j * 3 + 2] = 0;
			}
		}
	}
}

void RGB2YCbCr(BYTE* Img, BYTE* Y, BYTE* Cb, BYTE* Cr, int W, int H)
{
	for (int i = 0; i < H; i++) {	// Y좌표
		for (int j = 0; j < W; j++) {	// X좌표
			Y[i * W + j] = (BYTE)(0.299 * Img[i * W * 3 + j * 3 + 2]
				+ 0.587 * Img[i * W * 3 + j * 3 + 1]
				+ 0.114 * Img[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Img[i * W * 3 + j * 3 + 2]
				- 0.3313 * Img[i * W * 3 + j * 3 + 1]
				+ 0.5 * Img[i * W * 3 + j * 3] + 128.0);
			Cr[i * W + j] = (BYTE)(0.5 * Img[i * W * 3 + j * 3 + 2]
				- 0.4187 * Img[i * W * 3 + j * 3 + 1]
				- 0.0813 * Img[i * W * 3 + j * 3] + 128.0);
		}
	}
}

// 빨간색 영역만 masking (YCbCr 모델 기준)
void Red_Masking_YCbCr(BYTE* Img, BYTE* Out, int W, int H, BYTE* Y, BYTE* Cb, BYTE* Cr)
{
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Cb[i * W + j] < 140 && Cr[i * W + j] > 190) {
				Out[i * W * 3 + j * 3] = Img[i * W * 3 + j * 3];
				Out[i * W * 3 + j * 3 + 1] = Img[i * W * 3 + j * 3 + 1];
				Out[i * W * 3 + j * 3 + 2] = Img[i * W * 3 + j * 3 + 2];
			}
			else {
				Out[i * W * 3 + j * 3] = 0;
				Out[i * W * 3 + j * 3 + 1] = 0;
				Out[i * W * 3 + j * 3 + 2] = 0;
			}
		}
	}
}

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트

	FILE* fp;
	fp = fopen("fruit.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);

	int H = hInfo.biHeight;
	int W = hInfo.biWidth;
	int ImgSize = H * W;

	BYTE* Image;
	BYTE* Output;

	if (hInfo.biBitCount == 24) { // 트루컬러
		Image = (BYTE*)malloc(ImgSize * 3);
		Output = (BYTE*)malloc(ImgSize * 3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);
	}
	else { // 인덱스(그레이)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE*)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
	fclose(fp);

	//Red_Masking_RGB(Image, Output, W, H, 130, 50, 100);

	BYTE* Y = (BYTE*)malloc(ImgSize);
	BYTE* Cb = (BYTE*)malloc(ImgSize);
	BYTE* Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	Red_Masking_YCbCr(Image, Output, W, H, Y, Cb, Cr);

	/*
	fp = fopen("Y.raw", "wb");
	fwrite(Y, sizeof(BYTE), W * H, fp);
	fclose(fp);
	fp = fopen("Cb.raw", "wb");
	fwrite(Cb, sizeof(BYTE), W * H, fp);
	fclose(fp);
	fp = fopen("Cr.raw", "wb");
	fwrite(Cr, sizeof(BYTE), W * H, fp);
	fclose(fp);
	*/

	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output.bmp");

	free(Image);
	free(Output);
	free(Y);
	free(Cb);
	free(Cr);

	return 0;
}
