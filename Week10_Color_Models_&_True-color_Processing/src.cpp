#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, int W, int H, BYTE* Output, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	if (hInfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W * H * 3, fp);
	}
	else if (hInfo.biBitCount == 8) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W * H, fp);
	}
	fclose(fp);
}

void CopyBMPFile(BYTE* Img, BYTE* Out, int W, int H)
{
	for (int i = 0; i < W * H * 3; i++)
		Out[i] = Img[i];
}

void FillColor(BYTE* Img, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B)
{
	Img[Y * W * 3 + X * 3] = B; // Blue 성분
	Img[Y * W * 3 + X * 3 + 1] = G; // Green 성분
	Img[Y * W * 3 + X * 3 + 2] = R; // Red 성분
}

void DrawLine(BYTE* Img, int W, int H, int x, int y, BYTE R, BYTE G, BYTE B, char VH)
{
	if (VH == 'v')
		for (int i = 0; i < W; i++)
			FillColor(Img, i, y, W, H, R, G, B);
	else if (VH == 'h')
		for (int i = 0; i < H; i++)
			FillColor(Img, x, i, W, H, R, G, B);
	else
		return;
}

void DrawBox(BYTE* Img, int W, int H, int x1, int y1, int x2, int y2, BYTE R, BYTE G, BYTE B)
{
	for (int i = y1; i <= y2; i++) {
		for (int j = x1; j <= x2; j++) {
			FillColor(Img, j, i, W, H, R, G, B);
		}
	}
}

void DrawHorizontalBands(BYTE* Img, int W, int H)
{
	// 초기화
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Img[i * W * 3 + j * 3] = 0;
			Img[i * W * 3 + j * 3 + 1] = 0;
			Img[i * W * 3 + j * 3 + 2] = 0;
		}
	}
	// y좌표 기준 0~239 (Red)
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < W; j++) {
			Img[i * W * 3 + j * 3 + 2] = 255;
		}
	}
	// y좌표 기준 120 ~ 359 (Green)
	for (int i = 120; i < 360; i++) {
		for (int j = 0; j < W; j++) {
			Img[i * W * 3 + j * 3 + 1] = 255;
		}
	}
	// y좌표 기준 240 ~ 479 (Blue)
	for (int i = 240; i < 480; i++) {
		for (int j = 0; j < W; j++) {
			Img[i * W * 3 + j * 3] = 255;
		}
	}
}

void DrawGradation_B_R(BYTE* Img, int W, int H, int a1, int a2, char VH)
{
	double wt;
	if (VH == 'v') {
		for (int a = a1; a < a2; a++) {
			for (int i = 0; i < H; i++) {
				wt = i / (double)(H - 1);
				Img[i * W * 3 + a * 3] = (BYTE)(255 * (1.0 - wt));	// Blue
				Img[i * W * 3 + a * 3 + 1] = (BYTE)(255 * (1.0 - wt));	// Green
				Img[i * W * 3 + a * 3 + 2] = (BYTE)(255 * wt);	// Red
			}
		}
	}
	else if (VH == 'h') {
		for (int a = a1; a < a2; a++) {
			for (int i = 0; i < W; i++) {
				wt = i / (double)(W - 1);
				Img[a * W * 3 + i * 3] = (BYTE)(255 * (1.0 - wt));	// Blue
				Img[a * W * 3 + i * 3 + 1] = (BYTE)(255 * (1.0 - wt));	// Green
				Img[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt);	// Red
			}
		}
	}
	else
		return;
}

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트

	FILE* fp;
	fp = fopen("tcsample.bmp", "rb");
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
	

	// (50, 40)위치를 특정 색상으로 
	CopyBMPFile(Image, Output, W, H);
	DrawLine(Output, W, H, 50, 40, 255, 0, 255, 'v');
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_line.bmp");

	// (50, 100) ~ (300, 400) 박스 채우기
	CopyBMPFile(Image, Output, W, H);
	DrawBox(Output, W, H, 50, 100, 300, 400, 0, 255, 255);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_box.bmp");

	// 가로 띠 만들기
	DrawHorizontalBands(Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_horizontal_band.bmp");

	// 그라데이션 만들기 (B ~ R)
	CopyBMPFile(Image, Output, W, H);
	DrawGradation_B_R(Output, W, H, 120, 360, 'h');
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_gradation.bmp");

	free(Image);
	free(Output);

	return 0;
}
