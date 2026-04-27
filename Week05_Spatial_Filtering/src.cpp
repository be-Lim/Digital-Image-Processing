#pragma warning(disable:4996) //보안상 기존에 사용하던 입력함수의 문제 해결하는 전처리문
#include <stdio.h>
#include <stdlib.h> //동적할당에 필요한 헤더파일
#include <Windows.h> //BMP 헤더파일의 구조체 파일

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, int W, int H, BYTE* Output, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W * H, fp);
	fclose(fp);
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Histo[Img[i]]++;
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, int Threshold)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold)
			Out[i] = 0;
		else
			Out[i] = 255;
	}
}

int DetermThGonzalez(int* H)
{
	BYTE ep = 3;
	BYTE Low, High;
	BYTE Th, NewTh;
	int G1 = 0, G2 = 0, cnt1 = 0, cnt2 = 0, mu1, mu2;

	for (int i = 0; i < 256; i++) {
		if (H[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (H[i] != 0) {
			High = i;
			break;
		}
	}

	Th = (BYTE)((Low + High) / 2);

	while (1)
	{
		for (int i = Th + 1; i <= High; i++) {
			G1 += (H[i] * i);
			cnt1 += H[i];
		}
		for (int i = Low; i <= Th; i++) {
			G2 += (H[i] * i);
			cnt2 += H[i];
		}

		if (cnt1 == 0)
			cnt1 = 1;
		if (cnt2 == 0)
			cnt2 = 1;

		mu1 = (int)(G1 / cnt1);
		mu2 = (int)(G2 / cnt2);

		NewTh = (BYTE)((mu1 + mu2) / 2);

		if (abs(NewTh - Th) < ep) {
			Th = NewTh;
			break;
		}
		else {
			Th = NewTh;
		}
		G1 = G2 = cnt1 = cnt2 = 0;
	}
	return Th;
}

//박스 평균화
void AverageConv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;

	for (int i = 1; i < H-1; i++) {		//Y좌표(행)
		for (int j = 1; j < W-1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i+m) * W + (j+n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

//가우시안 평균화
void GaussianAvrConv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							0.125, 0.25, 0.125,
							0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

//Prewitt 마스크 X
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//0 ~ 765 ==> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

//Prewitt 마스크 Y
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							 0.0, 0.0, 0.0,
							 1.0, 1.0, 1.0 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//0 ~ 765 ==> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

//Sobel 마스크 X
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-2.0, 0.0, 2.0,
							-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//0 ~ 1020 ==> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

//Sobel 마스크 Y
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
							 0.0, 0.0, 0.0,
							 1.0, 2.0, 1.0 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//0 ~ 1020 ==> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

//Laplace 마스크
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 8.0, -1.0,
							-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//0 ~ 2040 ==> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

//Laplace_DC 마스크
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 9.0, -1.0,
							-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) {		//Y좌표(행)
		for (int j = 1; j < W - 1; j++) {	//X좌표(열)
			for (int m = -1; m <= 1; m++) {		//Kernel 행
				for (int n = -1; n <= 1; n++) {	//Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//0 ~ 2295 ==> 0 ~ 255
			//원래 영상의 밝기를 유지하기 위해 나누기가 아닌 클리핑처리를 시행
			if (SumProduct > 255.0)
				Out[i * W + j] = 255;
			else if (SumProduct < 0.0)
				Out[i * W + j] = 0;
			else
				Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

int main()
{
	/*영상 입력*/
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

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };
	
	// 평균화 필터 적용
	AverageConv(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_average.bmp");
	GaussianAvrConv(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_gaussian.bmp");
	
	// X 방향 경계 검출
	Prewitt_X_Conv(Image, Temp, W, H);
	ObtainHistogram(Temp, Histo, W, H);
	int Th = DetermThGonzalez(Histo);
	Binarization(Temp, Output, W, H, Th);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_prewitt_x.bmp");
	Sobel_X_Conv(Image, Temp, W, H);
	ObtainHistogram(Temp, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Temp, Output, W, H, Th);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_sobel_x.bmp");

	// Y 방향 경계 검출
	Prewitt_Y_Conv(Image, Temp, W, H);
	ObtainHistogram(Temp, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Temp, Output, W, H, Th);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_prewitt_y.bmp");
	Sobel_Y_Conv(Image, Temp, W, H);
	ObtainHistogram(Temp, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Temp, Output, W, H, Th);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_sobel_y.bmp");

	// X, Y 방향 경계 검출
	Sobel_X_Conv(Image, Temp, W, H);
	Sobel_Y_Conv(Image, Output, W, H);
	for (int i = 0; i < ImgSize; i++) {	// X 방향, Y 방향 경계 비교 후 합치기
		if (Temp[i] > Output[i])
			Output[i] = Temp[i];
	}
	ObtainHistogram(Output, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Output, Output, W, H, Th);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_sobel_xy.bmp");

	// 고역통과필터(HPF) 필터 적용
	Laplace_Conv(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_laplace.bmp");

	// High Boosting Filter(HBF) 적용
	Laplace_Conv_DC(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_laplace_dc.bmp");

	// 평균화 필터(LPF) 적용 후 High Boosting Filter(HBF) 적용 --> 노이즈 감소 효과
	GaussianAvrConv(Image, Temp, W, H);
	Laplace_Conv_DC(Temp, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_lpf_hbf.bmp");
	
	free(Image);
	free(Temp);
	free(Output);

	return 0;
}
