# 📝 [Week 10] 컬러 모델의 이해 및 트루컬러 영상 처리 (Color Models & True-color Processing)

## 1. 실습 개요
본 실습에서는 그레이스케일(Grayscale) 영상을 넘어 24비트 트루컬러(True-color) BMP 영상의 구조를 분석하고 처리하는 기법을 학습합니다. 빛의 삼원색인 RGB 모델을 기반으로 픽셀 데이터의 메모리 저장 방식(B, G, R 순서)을 이해하고, 이를 직접 제어하여 선, 도형, 색상 띠(Bands), 그리고 선형 보간을 활용한 그라데이션(Gradation)을 생성하는 로우레벨 C++ 코드를 구현하였습니다. 또한, 압축과 머신 비전에 최적화된 YCbCr 및 HSI 컬러 모델의 수학적 원리를 탐구하였습니다.

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
24비트 BMP 파일은 한 픽셀을 위해 3바이트를 사용하며, 메모리에는 Blue, Green, Red 순서로 저장됩니다. 이 원리를 배열 인덱스에 적용하여 특정 좌표의 색상을 정확히 변경합니다.
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
진행 방향(i)에 따라 0.0에서 1.0으로 변하는 가중치 $wt$를 계산하고, 이를 보색 관계에 적용하여 두 색상을 부드럽게 섞습니다.
```cpp
double wt = i / (double)(W - 1); // 0.0 ~ 1.0 사이의 가중치
Img[a * W * 3 + i * 3]     = (BYTE)(255 * (1.0 - wt)); // Blue: 감소
Img[a * W * 3 + i * 3 + 1] = (BYTE)(255 * (1.0 - wt)); // Green: 감소
Img[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt);         // Red: 증가
// 결과: 파란색/청록색에서 빨간색으로 부드럽게 변화
```

---

## 4. 트러블슈팅 및 공학적 고찰 (Technical Insights)

### RGB 모델의 한계와 YCbCr/HSI의 필요성:
RGB 모델은 색상과 밝기 정보가 서로 얽혀 있어(Correlated), 밝기만 조절하려 해도 R, G, B 세 요소를 모두 변경해야 하는 번거로움이 있습니다. 또한 데이터 중복이 많아 압축에 불리합니다. 반면 YCbCr은 밝기(Y)를 독립적으로 제어할 수 있어 효율적인 압축이 가능하며, HSI는 인간의 시각 인지 방식과 유사하게 조명과 상관없이 색상(Hue)을 구분할 수 있어 머신 비전 시스템 설계에 훨씬 유리함을 확인했습니다.

### 비트 깊이(Bit Depth)에 따른 메모리 관리:
실습 중 8비트(인덱스 컬러)와 24비트(트루 컬러) BMP 파일을 동시에 처리할 때, malloc 시 메모리 할당 크기와 fread/fwrite 시 채널 수 처리에 주의해야 함을 체감했습니다. 특히 컬러 영상 처리 시 데이터량이 3배로 증가하므로 포인터 연산 오차가 이미지 왜곡으로 직결될 수 있음을 확인하였습니다.

---

## 5. 실행 결과 (Visual Results)

| 특정 색상 선 그리기 | 사각형 박스 채우기 | RGB 채널 분리 띠 | Blue-Red 그라데이션 |
| :--- | :--- | :--- | :--- |
| ![output_line.bmp](https://github.com/user-attachments/files/27462412/output_line.bmp) | ![output_box.bmp](https://github.com/user-attachments/files/27462416/output_box.bmp) | ![output_horizontal_band.bmp](https://github.com/user-attachments/files/27462423/output_horizontal_band.bmp) | ![output_gradation.bmp](https://github.com/user-attachments/files/27462429/output_gradation.bmp) |

---

## 💡 학습 소감 및 AI/딥러닝 인사이트
이번 실습은 단순한 흑백 영상을 넘어 실제 세상과 같은 다채로운 컬러 데이터를 다루는 법을 익힌 중요한 과정이었습니다. 특히 HSI 모델에서 명도(Intensity)만을 분리하여 처리하는 방식은 딥러닝 기반 컴퓨터 비전 모델의 전처리에서 매우 중요한 시사점을 준다고 생각합니다.
1. **CNN 채널(Channel) 기반 연산의 이해:** CNN 모델이 컬러 이미지를 처리할 때 3개의 입력 채널을 독립적으로 연산한 뒤 통합하는 과정이, RGB 모델에서 각 성분별로 회선(Convolution)을 수행하는 고전적 방식과 맞닿아 있음을 이해했습니다.
2. **데이터 증강(Data Augmentation)과 조명 강건성:** 자율주행차 등 실무 환경의 AI 모델이 빛의 변화에 견디기 위해, 밝기와 색상이 혼재된 RGB 대신 HSI나 YCbCr 모델을 사용하여 색상은 유지한 채 밝기 채널($Y$ 또는 $I$)에만 변화를 주어 학습 데이터를 늘리는 기법의 근본 원리를 깨달았습니다.
