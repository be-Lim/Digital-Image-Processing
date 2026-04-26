#pragma warning(disable:4996) // 보안상 기존에 사용하던 입력함수의 문제 해결하는 전처리문
#include <stdio.h>
#include <stdlib.h> // 동적할당에 필요한 헤더파일
#include <Windows.h> // BMP 헤더파일의 구조체 파일

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, int W, int H, BYTE* Output, const char* FileName) {
	FILE* fp = fopen(FileName, "wb");  // 출력 영상을 저장하기 위해 파일 포인터(쓰기 모드) 정의
  // 영상의 헤더정보를 파일 포인터에 저장
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
  // 영상의 픽셀정보를 파일 포인터에 저
	fwrite(Output, sizeof(BYTE), W * H, fp);
	fclose(fp);
}

int main()
{
	/*영상 입력*/
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 정보 --> 256개(표현 가능 단계) * 4Bytes(RGBQUAD 1개 크기)

	FILE* fp;  // 파일 포인터 정의(메모리와 연결되는 '통로')
	fp = fopen("LENNA.bmp", "rb");  // 파일 포인터에 LENNA.bmp 파일 정보 저장
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
  // 파일 포인터에 저장된 LENNA.bmp 파일의 헤더정보를 앞에서 정의한 헤더변수에 저장
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // 배열의 이름 자체가 주소
	// 이미지 크기 정의
	int W = hInfo.biWidth;
	int H = hInfo.biHeight;
	int ImgSize = W * H;
	BYTE* Image = (BYTE*)malloc(ImgSize);  // 입력영상 버퍼 동적할당
	BYTE* Output = (BYTE*)malloc(ImgSize);  // 출력영상 버퍼 동적할당
	fread(Image, sizeof(BYTE), ImgSize, fp);  // 파일 포인터에 저장된 픽셀 데이터를 입력영상 버퍼에 저장
	fclose(fp);


	/*영상 처리*/

	//원본 영상
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output1.bmp");

	//원본 영상 밝기값 50 증가
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i] + 50;
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output2.bmp");

	//원본 영상 반전
	for (int i = 0; i < ImgSize; i++)
		Output[i] = 255 - Image[i];
	SaveBMPFile(hf, hInfo, hRGB, W, H, Output, "output3.bmp");

	free(Image);
	free(Output);

	return 0;
}
