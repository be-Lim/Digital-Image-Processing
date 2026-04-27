# 📝 [Week 03] BMP 구조 분석 및 픽셀 기반 영상 처리 (Point Processing)

## 1. 실습 개요
본 실습에서는 **C**를 사용하여 대표적인 비압축 그래픽 포맷인 **BMP(Bitmap)** 파일의 구조를 바이너리 수준에서 분석하고, 픽셀 데이터에 직접 접근하여 영상의 밝기 조절 및 반전 알고리즘을 구현하였습니다. 라이브러리를 사용하지 않고 로우레벨에서 파일을 읽고 쓰는 과정을 통해 디지털 영상의 메모리 적재 방식과 포인터 연산을 체득하였습니다.

---

## 2. BMP 파일 구조 (File Structure)
BMP 파일은 장치 독립적 비트맵(DIB)으로, 다음과 같은 계층적인 구조를 가집니다. 본 실습에서는 **8-bit Grayscale 영상**을 기준으로 분석하였습니다.

| 구성 요소 | 설명 | 크기 (Bytes) |
| :--- | :--- | :--- |
| **파일 헤더 (BITMAPFILEHEADER)** | 파일 타입(BM), 파일 크기, 데이터 오프셋 정보 등 | 14 Bytes |
| **정보 헤더 (BITMAPINFOHEADER) ** | 영상의 가로/세로 크기, 해상도, 색상 수 등 | 40 Bytes |
| **팔레트 정보 (RGBQUAD)** | 색상 인덱스 정보를 담은 테이블 (256개 단계) | 1,024 Bytes |
| **영상의 픽셀 데이터** | 실제 픽셀값 (1픽셀당 1Byte, Grayscale) | Width * Height |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 📂 파일 입출력 및 메모리 관리 
- **데이터 읽기**: `fopen`, `fread`를 사용하여 BMP 파일의 바이너리 데이터를 순차적으로 읽어들입니다.
- **동적 할당**: `malloc`을 이용해 이미지 크기($Width \times Height$)만큼의 메모리를 할당하여 픽셀 데이터를 처리합니다.
- **자료형 활용**: `Windows.h` 헤더의 `BYTE` 타입을 사용하여 0~255 사이의 픽셀값을 제어합니다.

### ⚙️ 영상 처리 알고리즘 
- **원본 복사**: 입력 버퍼의 데이터를 출력 버퍼로 전송하여 무손실 복사를 수행합니다.
- **밝기 증가**: 모든 픽셀에 상수 50을 더하여 전체적인 조도를 높입니다. ($s = r + 50$)
- **영상 반전**: 픽셀의 최대값(255)에서 현재값을 빼서 색상을 반전(Invert)시킵니다. ($s = 255 - r$)

---

## 4. 트러블슈팅 및 개선점 (Technical Insights)
- **오버플로우(Overflow) 문제**: `Output[i] = Image[i] + 50;` 연산 시, 결과값이 255를 초과하면 데이터 타입 특성상 0에 가까운 값으로 되돌아가는(Wrap-around) 현상이 발생할 수 있습니다.
- **해결 방안**: 향후 구현 시 조건문을 활용하여 연산 결과가 255보다 크면 255로 고정하고, 0보다 작으면 0으로 고정하는 **Clipping 기법**을 적용하여 영상의 깨짐 현상을 방지할 계획입니다.

---

## 5. 실행 결과 (Visual Results)
| 원본 영상 (Original) | 밝기 +50 (Brightness) | 영상 반전 (Inversion) |
| :--- | :--- | :--- |
| ![output1.bmp](https://github.com/user-attachments/files/27094856/output1.bmp) | ![output2.bmp](https://github.com/user-attachments/files/27094859/output2.bmp) | ![output3.bmp](https://github.com/user-attachments/files/27094861/output3.bmp) |

---

## 💡 학습 소감
디지털 영상이 메모리상에서 어떻게 배열(Array) 형태로 관리되는지 구조적으로 파악하게 된 계기가 되었고, 8-bit 영상에서 1024Bytes의 팔레트 정보가 필요한 이유를 이해할 수 있었습니다.
