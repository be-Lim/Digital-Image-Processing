# 📝 [Week 05] 공간 필터링 및 경계선 이진화 통합 (Spatial Filtering & Edge Binarization)

## 1. 실습 개요
본 실습에서는 영상의 공간 주파수(Spatial Frequency)를 제어하는 **컨볼루션(Convolution)** 연산을 구현하고, 이를 지난 주차에 학습한 **Gonzalez-Woods 자동 이진화 알고리즘**과 결합하는 통합 파이프라인을 구축하였습니다. 단순한 에지(Edge) 검출을 넘어, 검출된 경계선의 히스토그램을 분석하고 최적의 임계값으로 이진화함으로써 컴퓨터 비전(CV) 객체 인식을 위한 명확한 특징 추출(Feature Extraction) 과정을 로우레벨 C++ 코드로 체득하였습니다.

---

## 2. 핵심 이론 및 알고리즘 (Key Concepts)

| 필터/알고리즘 종류 | 역할 및 특징 | 본 실습에서의 활용 |
| :--- | :--- | :--- |
| **저역 통과 필터 (LPF)** | Box Average, Gaussian 마스크 | 고주파(노이즈)를 제거하여 부드러운 영상 생성 |
| **에지 검출 (Prewitt/Sobel)** | 수직, 수평 방향의 밝기 변화량 추출 | 객체의 경계선 탐색 (계수의 합 = 0) |
| **에지 이진화 (Binarization)** | 연속적인 그라데이션 값을 0과 255로 명확히 분리 | Gonzalez 알고리즘을 적용하여 최적 경계선만 추출 |
| **고역 강조 필터 (HBF)** | 라플라시안 마스크 + 원본 밝기 유지 | LPF적용 후 HBF를 결합해 노이즈 없는 선명한 영상 확보 |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 컨볼루션 연산 (4중 for문 구조)
- 영상의 각 화소(y, x)를 순회하며 $3 \times 3$ 마스크 커널과 곱의 합(Sum of Products)을 계산합니다.
- 계산된 결과값이 0~255 범위를 유지하도록 마스크 특성에 따라 절대값을 취하고 정규화(나누기) 연산을 수행합니다.
- 마스크 연산 시 영상의 가장자리(Border) 영역을 처리하기 위해 여백(Margin)을 고려하여 인덱스를 제어하였습니다.

### 에지 검출 및 자동 이진화 파이프라인 결합
에지 마스크(Prewitt, Sobel) 연산을 거친 영상(`Temp`)은 윤곽선의 강도 정보를 담고 있습니다. 이 정보를 이진화하여 명확한 선으로 만들기 위해 4주차 모듈을 재사용하여 파이프라인을 설계했습니다.

```cpp
// 1. Sobel X방향 마스크 연산으로 경계 추출
Sobel_X_Conv(Image, Temp, W, H);

// 2. 추출된 경계 영상의 히스토그램 획득
ObtainHistogram(Temp, Histo, W, H);

// 3. Gonzalez-Woods 알고리즘으로 최적의 에지 임계값 자동 도출
int Th = DetermThGonzalez(Histo);

// 4. 해당 임계값으로 에지 영상을 완벽하게 흑백(이진화) 분리
Binarization(Temp, Output, W, H, Th);
```

## 4. 트러블슈팅 및 공학적 고찰 (Technical Insights)

### 문제점 (에지 영상의 불명확성)
- 소벨(Sobel) 필터만 적용했을 때는 경계선의 밝기가 연속적이라 배경과 윤곽선을 컴퓨터가 완벽히 분리해 인식하기 어려웠습니다.

### 해결 방안 (알고리즘 융합)
- 4주차에 구현했던 자동 임계값 결정(DetermThGonzalez) 알고리즘을 컨볼루션 결과 영상에 적용했습니다. 이를 통해 사람이 직접 수치를 조정하지 않아도 가장 뚜렷한 외곽선만 남기는 객관적인 에지 맵(Edge Map)을 성공적으로 생성해냈습니다.

---

## 5. 실행 결과 (Visual Results)

| 박스 (LPF) | 가우시안 (LPF) |
| :--- | :--- |
| ![output_average.bmp](https://github.com/user-attachments/files/27112302/output_average.bmp) | ![output_gaussian.bmp](https://github.com/user-attachments/files/27112305/output_gaussian.bmp) |

| Prewitt X 에지 이진화 | 소벨 X 에지 이진화 | Prewitt Y 에지 이진화 | 소벨 Y 에지 이진화 | 통합 소벨 (X+Y) 이진화 |
| :--- | :--- | :--- | :--- | :--- |
| ![output_prewitt_x.bmp](https://github.com/user-attachments/files/27112401/output_prewitt_x.bmp) | ![output_sobel_x.bmp](https://github.com/user-attachments/files/27112407/output_sobel_x.bmp) | ![output_prewitt_y.bmp](https://github.com/user-attachments/files/27112412/output_prewitt_y.bmp) | ![output_sobel_y.bmp](https://github.com/user-attachments/files/27112430/output_sobel_y.bmp) | ![output_sobel_xy.bmp](https://github.com/user-attachments/files/27112445/output_sobel_xy.bmp) |

| 라플라시안 (HPF) | 라플라시안 DC (HBF) | 하이브리드 (LPF + HBF) |
| :--- | :--- | :--- |
| ![output_laplace.bmp](https://github.com/user-attachments/files/27112469/output_laplace.bmp) | ![output_laplace_dc.bmp](https://github.com/user-attachments/files/27112473/output_laplace_dc.bmp) | ![output_lpf_hbf.bmp](https://github.com/user-attachments/files/27112476/output_lpf_hbf.bmp) |

---

## 💡 학습 소감
Prewitt과 Sobel 마스크의 컨볼루션 결과는 시각적으로 큰 차이가 없음을 확인했고, 라플라시안 필터(HPF)는 HBF와 달리 컨볼루션 결과 영상의 밝기가 감소했음을 확인했습니다. 이는 아마도 마스크의 계수의 합이 0이기 때문이라고 추측합니다.
또한 이번 주차는 단순한 진도 나가기를 넘어, **"배웠던 알고리즘들을 어떻게 결합하여 더 고차원적인 결과를 만들어내는가"**를 깊이 체감한 실습이었습니다. 영상 노이즈를 억제한 뒤(LPF) 경계를 살리는(HBF) 기법이나, 에지를 추출한 뒤 자동으로 이진화(Gonzalez-Woods)하는 일련의 과정을 구현해보면서 실제 자율주행이나 인공지능(AI) 비전 시스템의 전처리 파이프라인과 정확히 일치함을 깨달았습니다. 이러한 로우레벨부터의 모듈화 경험은 향후 복잡한 딥러닝 모델의 데이터 로더나 증강(Augmentation) 파이프라인을 설계할 때 강력한 통찰력을 제공할 것입니다.
