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

//박스 평균화
void AverageConv(BYTE* Img, BYTE* Out, int W, int H)
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111 };
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

void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	//오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) {	//pivot index
		for (int j = i + 1; j < size; j++) {	//비교대상 index
			if (arr[i] > arr[j])
				swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
	//오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) {	//pivot index
		for (int j = i + 1; j < size; j++) {	//비교대상 index
			if (arr[i] > arr[j])
				swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	//오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) {	//pivot index
		for (int j = i + 1; j < size; j++) {	//비교대상 index
			if (arr[i] > arr[j])
				swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}

void Median_Filtering(BYTE* Img, BYTE* Out, int W, int H, int Size)
{
	int Margin = Size / 2;	//컨볼루션 윈도우 마진 크기
	int WSize = Size * Size;	//윈도우 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);	//윈도우 내부값을 담을 동적 배열
	int i, j, m, n;
	for (i = Margin; i < H - Margin; i++) {		//윈도우를 행 방향으로 이동
		for (j = Margin; j < W - Margin; j++) {	//윈도우를 열 방향으로 이동
			for (m = -Margin; m <= Margin; m++) {	//윈도우 내부에서 컨볼루션 연산 시 행 방향 이동 
				for (n = -Margin; n <= Margin; n++) {	//윈도우 내부에서 컨볼루션 연산 시 열 방향 이동
					//윈도우 내부로 들어온 2차원 배열 Image의 픽셀 주소를 변환하여 1차원 배열 temp에 대입
					temp[(m + Margin) * Size + (n + Margin)] = Img[(i + m) * W + j + n];
				}
			}
			//1차원 배열 temp의 값들을 오름차순 정렬 후 중간값을 Output의 중심화소(i, j)에 대입
			Out[i * W + j] = Median(temp, WSize);
		}
	}
	free(temp);
}

void Max_Filtering(BYTE* Img, BYTE* Out, int W, int H, int Size)
{
	int Margin = Size / 2;	//컨볼루션 윈도우 마진 크기
	int WSize = Size * Size;	//윈도우 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);	//윈도우 내부값을 담을 동적 배열
	int i, j, m, n;
	for (i = Margin; i < H - Margin; i++) {		//윈도우를 행 방향으로 이동
		for (j = Margin; j < W - Margin; j++) {	//윈도우를 열 방향으로 이동
			for (m = -Margin; m <= Margin; m++) {	//윈도우 내부에서 컨볼루션 연산 시 행 방향 이동 
				for (n = -Margin; n <= Margin; n++) {	//윈도우 내부에서 컨볼루션 연산 시 열 방향 이동
					//윈도우 내부로 들어온 2차원 배열 Image의 픽셀 주소를 변환하여 1차원 배열 temp에 대입
					temp[(m + Margin) * Size + (n + Margin)] = Img[(i + m) * W + j + n];
				}
			}
			//1차원 배열 temp의 값들을 오름차순 정렬 후 최댓값을 Output의 중심화소(i, j)에 대입
			Out[i * W + j] = MaxPooling(temp, WSize);
		}
	}
	free(temp);
}

void Min_Filtering(BYTE* Img, BYTE* Out, int W, int H, int Size)
{
	int Margin = Size / 2;	//컨볼루션 윈도우 마진 크기
	int WSize = Size * Size;	//윈도우 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);	//윈도우 내부값을 담을 동적 배열
	int i, j, m, n;
	for (i = Margin; i < H - Margin; i++) {		//윈도우를 행 방향으로 이동
		for (j = Margin; j < W - Margin; j++) {	//윈도우를 열 방향으로 이동
			for (m = -Margin; m <= Margin; m++) {	//윈도우 내부에서 컨볼루션 연산 시 행 방향 이동 
				for (n = -Margin; n <= Margin; n++) {	//윈도우 내부에서 컨볼루션 연산 시 열 방향 이동
					//윈도우 내부로 들어온 2차원 배열 Image의 픽셀 주소를 변환하여 1차원 배열 temp에 대입
					temp[(m + Margin) * Size + (n + Margin)] = Img[(i + m) * W + j + n];
				}
			}
			//1차원 배열 temp의 값들을 오름차순 정렬 후 최솟값을 Output의 중심화소(i, j)에 대입
			Out[i * W + j] = MinPooling(temp, WSize);
		}
	}
	free(temp);
}

int main()
{
	/*영상 입력*/
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)

	FILE* fp;
	fp = fopen("LENNA_impulse.bmp", "rb");
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

	// Low Pass Filter(Box)
	AverageConv(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "average.bmp");

	// Low Pass Filter(Gaussian)
	GaussianAvrConv(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "gaussian.bmp");

	// 3*3 size Median filter
	Median_Filtering(Image, Output, W, H, 3);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "median_3.bmp");

	// 5*5 size Median filter
	Median_Filtering(Image, Output, W, H, 5);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "median_5.bmp");

	// 7*7 size Median filter
	Median_Filtering(Image, Output, W, H, 7);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "median_7.bmp");

	// 9*9 size Median filter
	Median_Filtering(Image, Output, W, H, 9);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "median_9.bmp");
	// --> filter 사이즈가 커질수록 코드 실행시간이 오래걸리고 이미지도 더 뭉개짐

	// 3*3 size Maximum filter
	Max_Filtering(Image, Output, W, H, 3);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "max_3.bmp");

	// 3*3 size Minimum filter
	Min_Filtering(Image, Output, W, H, 3);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "min_3.bmp");

	free(Image);
	free(Temp);
	free(Output);

	return 0;
}
