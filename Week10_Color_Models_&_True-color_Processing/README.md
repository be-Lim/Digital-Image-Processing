# 📝 [Week 10] 컬러 모델의 이해 및 트루컬러 영상 처리 (Color Models & True-color Processing)

## 1. 실습 개요
본 실습에서는 그레이스케일(Grayscale) 영상을 넘어 24비트 트루컬러(True-color) BMP 영상의 구조를 분석하고 처리하는 기법을 학습합니다. 빛의 삼원색인 RGB 모델을 기반으로 픽셀 데이터의 메모리 저장 방식(B, G, R 순서)을 이해하고, 이를 직접 제어하여 선, 도형, 색상 띠(Bands)를 생성하는 로우레벨 C++ 코드를 구현하였습니다. 

특히, 선형 보간법(Linear Interpolation)을 응용하여 **빛의 삼원색(RGB)이 각각의 보색(CMY: Cyan, Magenta, Yellow)으로 부드럽게 전이되는 3단 그라데이션(Gradation)**을 수학적으로 설계하였습니다. 더불어 압축과 머신 비전에 최적화된 YCbCr 및 HSI 컬러 모델의 원리를 탐구하였습니다.

---

## 2. 핵심 이론 및 알고리즘 (Key Concepts)
| 컬러 모델 | 역할 및 특징 | 주요 활용 분야 |
| :--- | :--- | :--- |
| **RGB 모델** | 빛의 삼원색인 적(R), 녹(G), 청(B)의 혼합. 밝기와 색상이 혼재됨.  | CRT 모니터, 컴퓨터 그래픽스, 일반 디스플레이 |
| **YCbCr 모델** | 밝기(Y)와 색차 정보(Cb, Cr)를 분리. 성분 간 상관관계가 낮음 | JPEG, MPEG 등 영상 압축 표준 |
| **HSI 모델** | 색상(Hue), 채도(Saturation), 명도(Intensity)로 색을 표현 | 조명 변화에 강건한 머신 비전 객체 식별 |
| **선형 혼합** | 위치에 따른 가중치($wt$)를 부여하여 두 색상 사이를 부드럽게 연결 | 실시간 색상 전이(Gradation) 및 필터 효과 구현 |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 파이프라인 1: BGR 순차 저장 원리를 이용한 화소 제어 (FillColor)
24비트 BMP 파일은 한 픽셀을 위해 3바이트를 사용하며, 메모리에는 **Blue, Green, Red** 순서로 저장됩니다. 이 원리를 1차원 배열 인덱스에 적용하여 특정 2D 좌표의 색상을 정확히 변경합니다.
```cpp
void FillColor(BYTE* Img, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
    // 1D 배열에서 2D 좌표 (X, Y) 접근: (Y * W * 채널수) + (X * 채널수)
    // 24비트 영상이므로 채널수는 3(BGR)
    Img[Y * W * 3 + X * 3]     = B; // Blue (첫 번째 바이트)
    Img[Y * W * 3 + X * 3 + 1] = G; // Green (두 번째 바이트)
    Img[Y * W * 3 + X * 3 + 2] = R; // Red (세 번째 바이트)
}
```

### 파이프라인 2: 가중치($wt$) 기반 선형 혼합 그라데이션 (DrawGradation)
진행 방향(i)에 따라 0.0에서 1.0으로 변하는 가중치 $wt$를 계산하고, 이를 빛의 가산 혼합 원리에 적용하여 원색이 보색으로 부드럽게 전환되는 로직을 구현했습니다.
- Blue $\rightarrow$ Yellow (R+G): Blue 성분은 감소(1.0 - wt), Red와 Green 성분은 증가(wt)
- Green $\rightarrow$ Magenta (R+B): Green 성분은 감소, Red와 Blue 성분은 증가
- Red $\rightarrow$ Cyan (G+B): Red 성분은 감소, Green과 Blue 성분은 증가
```cpp
// 예시: Blue에서 Yellow(Red+Green)로의 전이 로직
double wt = i / (double)(W - 1); // 0.0 ~ 1.0 사이의 가중치
Img[a * W * 3 + i * 3]     = (BYTE)(255 * (1.0 - wt)); // Blue: 서서히 감소
Img[a * W * 3 + i * 3 + 1] = (BYTE)(255 * wt);         // Green: 서서히 증가
Img[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt);         // Red: 서서히 증가
```

---

## 4. 트러블슈팅 및 공학적 고찰 (Technical Insights)

### RGB 모델의 한계와 YCbCr/HSI의 필요성:
RGB 모델은 색상과 밝기 정보가 서로 얽혀 있어(Correlated), 밝기만 조절하려 해도 R, G, B 세 요소를 모두 일정한 비율로 제어해야 하는 제약이 있습니다. 또한 데이터 중복이 많아 압축에 불리합니다. 반면 YCbCr은 밝기(Y)를 독립적으로 제어할 수 있어 효율적인 데이터 압축이 가능하며, HSI는 인간의 시각 인지 방식과 유사하게 조명과 상관없이 고유 색상(Hue)을 추출할 수 있어 컴퓨터/머신 비전 시스템 설계에 훨씬 유리함을 확인했습니다.

### 비트 깊이(Bit Depth)에 따른 메모리 관리:
8비트 인덱스 영상과 24비트 트루컬러 영상은 픽셀당 차지하는 바이트 수가 다릅니다. 따라서 malloc을 통한 동적 할당 시 ImgSize * 3을 적용하고, fread/fwrite 시 채널 수 처리를 조건문(biBitCount == 24)으로 엄격히 분기해야 함을 체감했습니다. 특히 컬러 영상 처리 시 데이터량이 3배로 증가하므로 포인터 연산 오차가 이미지 왜곡으로 직결될 수 있음을 확인하였습니다.

---

## 5. 실행 결과 (Visual Results)

| 특정 색상 선 그리기 | 사각형 박스 채우기 | RGB 채널 분리 띠 | RGB → CMY 3단 그라데이션 |
| :--- | :--- | :--- | :--- |
| ![output_line.bmp](https://github.com/user-attachments/files/27462412/output_line.bmp) | ![output_box.bmp](https://github.com/user-attachments/files/27462416/output_box.bmp) | ![output_horizontal_band.bmp](https://github.com/user-attachments/files/27462423/output_horizontal_band.bmp) | ![output.bmp](https://github.com/user-attachments/files/27624601/output.bmp) |

---

## 💡 학습 소감 및 AI/딥러닝 인사이트
이번 실습은 단순한 흑백 영상을 넘어 실제 세상과 같은 다채로운 컬러 데이터를 다루는 법을 익힌 중요한 과정이었습니다. 특히 빛의 삼원색을 2차색으로 수학적으로 전이시키는 과정을 통해, 채널 간의 연산이 어떻게 시각적 색상으로 발현되는지 깊이 이해할 수 있었습니다.
이러한 로우레벨 컬러 모델에 대한 이해는 딥러닝 기반 컴퓨터 비전 모델의 전처리(Preprocessing) 단계에서 매우 중요한 시사점을 제공할 것으로 예상합니다.

1. **CNN 채널(Channel) 기반 연산의 이해:** CNN 모델이 컬러 이미지를 처리할 때 3개의 입력 채널을 독립적으로 연산(Convolution)한 뒤 통합하는 과정이, 본 실습에서 R, G, B 배열을 각각 나누어 가중치를 곱하고 더하는 방식과 정확히 맞닿아 있음을 깨달았습니다.
2. **데이터 증강(Data Augmentation)과 조명 강건성:** 자율주행차 등 실무 환경의 AI 모델이 빛의 변화에 견디기 위해, 밝기와 색상이 혼재된 RGB 대신 HSI나 YCbCr 모델을 사용하여 색상은 유지한 채 밝기 채널($Y$ 또는 $I$)에만 변화를 주어 학습 데이터를 늘리는 기법의 근본 원리를 깨달았습니다.
