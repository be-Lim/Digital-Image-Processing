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

void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Out[i] = 255 - Img[i];
}

void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] + val > 255)
			Out[i] = 255;
		else if (Img[i] + val < 0)
			Out[i] = 0;
		else
			Out[i] = Img[i] + val;
	}
}

void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] * val > 255.0)
			Out[i] = 255;
		else
			Out[i] = (BYTE)(Img[i] * val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Histo[Img[i]]++;
}

void ObtainAHistogram(int* Histo, int* AHisto)
{
	for (int i = 0; i < 256; i++)
		for (int j = 0; j <= i; j++)
			AHisto[i] += Histo[j];
}

void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}
}

void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
	int ImgSize = W * H;
	int Nt = W * H;
	int Gmax = 255;
	double Ratio = (double)Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++)
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	for (int i = 0; i < ImgSize; i++)
		Out[i] = NormSum[Img[i]];
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

int main()
{
	/*영상 입력*/
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)

	FILE* fp;
	//fp = fopen("LENNA.bmp", "rb");
	fp = fopen("coin.bmp", "rb");
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
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);


	// LENNA 영상 반전
	InverseImage(Image, Output, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_inverse.bmp");
	// LENNA 영상 밝기 조절 + 클리핑 처리
	BrightnessAdj(Image, Output, W, H, 50);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_brightness.bmp");
	// LENNA 영상 대비 조절 + 클리핑 처리
	ContrastAdj(Image, Output, W, H, 1.5);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_contrast.bmp");
	
	
	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };
	
	// coin 영상 스트레칭
	ObtainHistogram(Image, Histo, W, H);
	HistogramStretching(Image, Output, Histo, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_stretch.bmp");

	// coin 영상 평활화
	ObtainHistogram(Image, Histo, W, H);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, W, H);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_equalization.bmp");
	
	// coin 영상 경계값 자동 결정 후 이진화
	BYTE Th;
	ObtainHistogram(Image, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Image, Output, W, H, Th);
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output_binarization.bmp");
	
	free(Image);
	free(Output);

	return 0;
}
