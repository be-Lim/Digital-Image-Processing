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

void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Out[i] = 255 - Img[i];
}

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}

// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;

	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;

	// 가장 면적이 넓은 영역을 찾아내기 위함
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area])
			Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++)
		CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장 (size filtering)
		//if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // 특정 면적 이상인 영역만 출력
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);	//그대로 출력
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함


void Obtain2DBoundingBox(BYTE* Img, int W, int H, int* LUX, int* LUY, int* RDX, int* RDY)
{
	int flag = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) {
				*LUY = i;
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;
	}
	flag = 0;

	for (int i = H - 1; i >= 0; i--) {
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) {
				*RDY = i;
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;
	}
	flag = 0;

	for (int j = 0; j < W; j++) {
		for (int i = 0; i < H; i++) {
			if (Img[i * W + j] == 0) {
				*LUX = j;
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;
	}
	flag = 0;

	for (int j = W - 1; j >= 0; j--) {
		for (int i = 0; i < H; i++) {
			if (Img[i * W + j] == 0) {
				*RDX = j;
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;
	}
	//printf("%d, %d, %d, %d", LUX, LUY, RDX, RDY);
}

void DrawColorRectOutline(BYTE* Img, int W, int H, int LUX, int LUY, int RDX, int RDY, BYTE R, BYTE G, BYTE B)
{
	for (int i = LUX; i < RDX; i++) {
		Img[LUY * W * 3 + i * 3 + 0] = B;
		Img[LUY * W * 3 + i * 3 + 1] = G;
		Img[LUY * W * 3 + i * 3 + 2] = R;
	}
	for (int i = LUX; i < RDX; i++) {
		Img[RDY * W * 3 + i * 3 + 0] = B;
		Img[RDY * W * 3 + i * 3 + 1] = G;
		Img[RDY * W * 3 + i * 3 + 2] = R;
	}
	for (int i = LUY; i < RDY; i++) {
		Img[i * W * 3 + LUX * 3 + 0] = B;
		Img[i * W * 3 + LUX * 3 + 1] = G;
		Img[i * W * 3 + LUX * 3 + 2] = R;
	}
	for (int i = LUY; i < RDY; i++) {
		Img[i * W * 3 + RDX * 3 + 0] = B;
		Img[i * W * 3 + RDX * 3 + 1] = G;
		Img[i * W * 3 + RDX * 3 + 2] = R;
	}
}

void FillColor(BYTE* Img, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B)
{
	Img[Y * W * 3 + X * 3] = B; // Blue 성분
	Img[Y * W * 3 + X * 3 + 1] = G; // Green 성분
	Img[Y * W * 3 + X * 3 + 2] = R; // Red 성분
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

// 얼굴 영역만 masking (YCbCr 모델 기준)
void Face_Masking_YCbCr(BYTE* Img, BYTE* Out, int W, int H, BYTE* Y, BYTE* Cb, BYTE* Cr)
{
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((Cb[i * W + j] > 95 && Cb[i * W + j] < 125)
				&& (Cr[i * W + j] > 135 && Cr[i * W + j] < 205)) {
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
	fp = fopen("face.bmp", "rb");
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
	BYTE* Temp;
	BYTE* Output;

	if (hInfo.biBitCount == 24) { // 트루컬러
		Image = (BYTE*)malloc(ImgSize * 3);
		Temp = (BYTE*)malloc(ImgSize * 3);
		Output = (BYTE*)malloc(ImgSize * 3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);
	}
	else { // 인덱스(그레이)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE*)malloc(ImgSize);
		Temp = (BYTE*)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
	fclose(fp);

	BYTE* Y = (BYTE*)malloc(ImgSize);
	BYTE* Cb = (BYTE*)malloc(ImgSize);
	BYTE* Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	Face_Masking_YCbCr(Image, Output, W, H, Y, Cb, Cr);

	int LUX, LUY, RDX, RDY;
	BYTE* temp3 = (BYTE*)malloc(ImgSize);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Output[i * W * 3 + j * 3] == 0)
				temp3[i * W + j] = 0;
			else
				temp3[i * W + j] = 255;
		}
	}
	m_BlobColoring(temp3, H, W);
	Obtain2DBoundingBox(temp3, W, H, &LUX, &LUY, &RDX, &RDY);
	DrawColorRectOutline(Image, W, H, LUX, LUY, RDX, RDY, 255, 0, 0);

	SaveBMPFile(hf, hInfo, hRGB, W, H, Image, "output.bmp");

	free(Image);
	free(Temp);
	free(temp3);
	free(Output);
	free(Y);
	free(Cb);
	free(Cr);

	return 0;
}
