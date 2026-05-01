# [Week 06] 영상 노이즈 모델 분석 및 복원 (Image Noise Models & Restoration)

## 1. 실습 개요
본 실습에서는 영상의 획득 및 전송 과정에서 발생하는 **노이즈(Noise)**의 특성을 분석하고, 이를 제거하여 원본 영상을 추정하는 **영상 복원(Image Restoration)** 기법을 구현하였습니다. 가우시안(Gaussian) 노이즈와 임펄스(Salt & Pepper) 노이즈가 추가된 각각의 영상에 대해 선형 필터(평균화)와 비선형 순서 통계 필터(중간값, 최대/최소)를 교차 적용해보며, **노이즈 모델의 특성에 따른 최적의 공간 필터링 파이프라인**을 검증하였습니다.

---

## 2. 핵심 이론 및 알고리즘 (Key Concepts)

디지털 영상의 주요 노이즈 원인에 따라 효과적인 제거 필터가 다릅니다.

| 필터 종류 | 연산 특성 | 효과 및 장단점 | 최적 노이즈 모델 |
| :--- | :--- | :--- | :--- |
| **산술 평균 / 가우시안** | 선형 (마스크 가중치 합산) | 전반적인 노이즈를 부드럽게 완화하지만 경계(Edge)가 뭉개짐 | **가우시안 노이즈** |
| **중간값 (Median)** | 비선형 (오름차순 정렬 후 중앙값) | 아웃라이어(튀는 값)를 무시하여 경계를 보존하면서 노이즈 완벽 제거 | **임펄스 (Salt & Pepper)** |
| **최댓값 (Max)** | 비선형 (가장 밝은 값 선택) | 어두운 노이즈(Pepper) 제거 및 밝은 영역 확장 | Pepper 노이즈 |
| **최솟값 (Min)** | 비선형 (가장 어두운 값 선택) | 밝은 노이즈(Salt) 제거 및 어두운 영역 확장 | Salt 노이즈 |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 비선형 순서 통계 필터 구현 (Median / Max / Min)
마스크 내의 픽셀들을 산술적으로 곱하고 더하는 것이 아니라, 1차원 배열로 추출하여 **통계적 위치(순서)**를 기반으로 중심 픽셀값을 결정합니다.

```cpp
// 1. 윈도우 내부 픽셀들을 1차원 동적 배열(temp)로 추출
temp[(m + Margin) * Size + (n + Margin)] = Img[(i + m) * W + j + n];

// 2. 오름차순 정렬 (Sorting) 적용
// Bubble Sort 로직 등을 활용하여 temp 배열을 크기순으로 정렬

// 3. 통계값 추출 후 출력 영상에 대입
Out[i * W + j] = Median(temp, WSize);       // 중앙값 (S/2 인덱스)
// Out[i * W + j] = MaxPooling(temp, WSize); // 최댓값 (S-1 인덱스)
// Out[i * W + j] = MinPooling(temp, WSize); // 최솟값 (0 인덱스)
```

### 동적 윈도우 크기(Dynamic Window Size) 제어
필터의 크기(Size)를 함수의 인자로 받아, 3x3 뿐만 아니라 5x5, 7x7, 9x9 등 다양한 크기의 마스크를 동적으로 생성하고 패딩 마진(Margin = Size / 2)을 자동 계산하도록 모듈화하였습니다.

---

## 4. 트러블슈팅 및 공학적 고찰 (Technical Insights)

### 노이즈 모델과 필터의 궁합 (Mismatch Issue)
- **현상**: 임펄스(Salt & Pepper) 노이즈 영상에 평균화 필터를 적용할 경우, 극단적인 노이즈 값(0 또는 255)이 주변 픽셀 연산에 포함되어 오히려 노이즈가 옅게 번지는 현상이 발생했습니다.
- **해결**: 평균 대신 중간값(Median) 필터를 적용한 결과, 극단적인 값은 정렬 시 양 끝단으로 밀려나 선택되지 않으므로 노이즈가 깔끔하게 소거됨을 확인했습니다.

### 필터 크기(Window Size)와 연산량의 트레이드오프
- **현상 관찰**: Median_Filtering 함수에서 필터 사이즈를 3x3에서 9x9로 키울수록, 코드가 실행되는 시간이 기하급수적으로 길어지고 결과 이미지의 윤곽선도 심하게 뭉개졌습니다.
- **원인 분석**: 윈도우 사이즈가 커지면 컨볼루션 연산 횟수도 증가하지만, 무엇보다 윈도우 내 픽셀을 정렬하는 알고리즘(O(N<sup>2</sup>))의 연산량이 영상의 전체 화소 수만큼 반복되기 때문입니다. 이를 통해 단순한 기능 구현을 넘어 **알고리즘의 시간 복잡도(Time Complexity)**가 실제 영상 처리 속도에 미치는 영향을 체감했습니다.

---

## 5. 실행 결과 (Visual Results)

### [Test 1] 가우시안 노이즈 제거 비교

- 가우시안 노이즈에는 중간값/최대/최소 필터보다 평균화 필터가 더 우수함을 알 수 있습니다.
- 이는 가우시안 노이즈가 특정 픽셀이 튀는 것이 아니라 전체적으로 분포된 연속적인 값이므로, 중간값보다는 픽셀들의 값을 평균 내어 분산을 줄이는 선형 필터가 더 적합하기 때문으로 예상합니다.

| 원본 (Gaussian Noise) | 평균화 (Gaussian / LPF) | 중간값 (Median 3x3) | 최댓값 (Max 3x3) | 최솟값 (Min 3x3) |
| :--- | :--- | :--- | :--- | :--- |
| ![LENNA_gauss.bmp](https://github.com/user-attachments/files/27119525/LENNA_gauss.bmp) | ![gaussian.bmp](https://github.com/user-attachments/files/27119535/gaussian.bmp) | ![median_3.bmp](https://github.com/user-attachments/files/27119538/median_3.bmp) | ![max_3.bmp](https://github.com/user-attachments/files/27119540/max_3.bmp) | ![min_3.bmp](https://github.com/user-attachments/files/27119551/min_3.bmp) |


### [Test 2] 임펄스 (Salt & Pepper) 노이즈 제거 비교

- 임펄스 노이즈에는 평균화보다 중간값(Median) 필터가 압도적으로 우수함을 증명합니다.
- 또한, 임펄스 노이즈에 **최댓값(Max) 필터**를 적용하면 어두운 Pepper 노이즈가 제거되고 밝은 Salt 노이즈만 확장되어 남으며, 반대로 **최솟값(Min) 필터**를 적용하면 Salt 노이즈가 제거되고 Pepper 노이즈만 남는 필터별 수학적 특성을 명확히 확인할 수 있습니다.

| 원본 (Impulse Noise) | 평균화 (Gaussian / LPF) | 중간값 (Median 3x3) | 최댓값 (Max 3x3) | 최솟값 (Min 3x3) |
| :--- | :--- | :--- | :--- | :--- |
| ![LENNA_impulse.bmp](https://github.com/user-attachments/files/27119325/LENNA_impulse.bmp) | ![gaussian.bmp](https://github.com/user-attachments/files/27119339/gaussian.bmp) | ![median_3.bmp](https://github.com/user-attachments/files/27119352/median_3.bmp) | ![max_3.bmp](https://github.com/user-attachments/files/27119363/max_3.bmp) | ![min_3.bmp](https://github.com/user-attachments/files/27119371/min_3.bmp) |

### [Test 3] Median 필터 사이즈 증가에 따른 결과 비교

- 사이즈가 커질수록 굵은 노이즈도 제거 가능하지만, 영상의 디테일(Edge) 손실이 발생합니다.

| 3x3 Median | 5x5 Median | 7x7 Median | 9x9 Median |
| :--- | :--- | :--- | :--- |
| ![median_3.bmp](https://github.com/user-attachments/files/27119434/median_3.bmp) | ![median_5.bmp](https://github.com/user-attachments/files/27119450/median_5.bmp) | ![median_7.bmp](https://github.com/user-attachments/files/27119455/median_7.bmp) | ![median_9.bmp](https://github.com/user-attachments/files/27119462/median_9.bmp) |

---

## 💡 학습 소감
이번 주차는 노이즈라는 '오염된 데이터'를 어떻게 수학적/통계적으로 복원할 것인지 고민하는 시간이었습니다. 특히 중간값(Median) 필터처럼 아웃라이어(Outlier)에 강건한 알고리즘을 C언어 배열과 정렬 로직을 이용해 로우레벨에서 구현해 본 것은 뜻깊은 경험이었습니다. 이는 향후 머신러닝이나 딥러닝 모델 학습 시, 센서 데이터의 결측치나 이상치를 제거하는 데이터 전처리(Data Preprocessing) 로직을 설계할 때 중요한 직관을 제공할 것으로 예상됩니다.
