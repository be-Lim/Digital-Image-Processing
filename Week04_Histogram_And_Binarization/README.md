# 📝 [Week 04] 히스토그램 분석 및 영상 이진화 (Histogram & Binarization)

## 1. 실습 개요
본 실습에서는 영상의 통계적 특성을 나타내는 **히스토그램(Histogram)**을 분석하고, 이를 기반으로 명암 대비를 개선하는 **스트레칭(Stretching)** 및 **평활화(Equalization)** 기법을 구현하였습니다. 또한, 배경과 물체를 분리하기 위한 **이진화(Binarization)** 과정에서 최적의 임계값을 자동으로 찾아주는 **Gonzalez-Woods 알고리즘**을 학습하고 코드로 구현하였습니다.

---

## 2. 히스토그램 및 이진화 개념 (Key Concepts)
영상의 밝기 분포에 따라 적절한 처리 기법을 선택하는 것이 중요합니다.

| 분석 항목 | 설명 | 비고 |
| :--- | :--- | :--- |
| **Bimodal 히스토그램** | 밝기 분포가 두 개의 봉우리(Peak)로 나뉜 형태 | `coin.bmp` 등 이진화에 매우 적합 |
| **Uniform 히스토그램** | 밝기 값이 고르게 분포된 형태 | `LENNA.bmp` 등 이진화보다는 평활화 권장 |
| **이진화 (Binarization)** | 특정 임계값(Threshold)을 기준으로 전경과 배경을 분리 | 자동 결정 알고리즘의 필요성 대두 |
| **평활화 vs 스트레칭** | 밝기 값의 분포를 이용해 명암비를 극대화하는 기법 | 평활화가 스트레칭보다 효과가 강력함 |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 📂 산술 연산 및 클리핑(Clipping)
- **밝기/대비 조절**: 단순히 값을 더하거나 곱하는 것에 그치지 않고, 결과가 0~255 범위를 벗어나지 않도록 조건문을 사용하여 강제로 고정(Clipping) 처리하였습니다.

### ⚙️ 히스토그램 평활화 (Histogram Equalization)
1. **히스토그램 획득**: 영상 전체를 순회하며 각 밝기 빈도수 계산.
2. **누적 히스토그램(CDF) 생성**: 0부터 255까지 빈도수를 누적 합산.
3. **정규화 매핑**: `(255.0 / 전체 화소 수) * 누적합` 공식을 통해 새로운 픽셀값으로 변환.

### ⚙️ 경계값 자동 결정 (Gonzalez-Woods)
- 초기 임계값 $T$를 설정한 후, $T$를 기준으로 나뉜 두 영역의 평균 밝기값($\mu_1, \mu_2$)의 중간값으로 임계값을 지속적으로 갱신합니다. 두 임계값의 차이가 매우 작아질 때까지 반복하여 객관적인 기준을 도출합니다.

---

## 4. 트러블슈팅 및 개선점 (Technical Insights)
- **주관적 임계값의 한계**: 사용자가 직접 숫자를 입력하는 이진화는 영상마다 결과가 달라지는 문제가 있었습니다.
- **해결 방안**: Gonzalez-Woods 알고리즘을 통해 영상의 데이터(히스토그램)에 기반한 **자동 임계값 결정 함수**를 구현함으로써 일관성 있는 이진화 결과를 얻을 수 있었습니다.

---

## 5. 실행 결과 (Visual Results)
| 원본영상 | 영상반전 | 밝기조절+클리핑 | 대비조절+클리핑 |
| :---: | :---: | :---: | :---: |
| [LENNA.bmp](https://github.com/user-attachments/files/27096382/LENNA.bmp) | [output_inverse.bmp](https://github.com/user-attachments/files/27096355/output_inverse.bmp) | [output_brightness.bmp](https://github.com/user-attachments/files/27096357/output_brightness.bmp) | [output_contrast.bmp](https://github.com/user-attachments/files/27096359/output_contrast.bmp) |


| 원본영상 | 스트레칭 | 평활화 | 자동 이진화 |
| :---: | :---: | :---: | :---: |
| [coin.bmp](https://github.com/user-attachments/files/27096387/coin.bmp) | [output_stretch.bmp](https://github.com/user-attachments/files/27096334/output_stretch.bmp) | [output_equalization.bmp](https://github.com/user-attachments/files/27096337/output_equalization.bmp) | [output_binarization.bmp](https://github.com/user-attachments/files/27096340/output_binarization.bmp) |

---

## 💡 학습 소감
단순한 픽셀 연산을 넘어 영상의 통계적 분포를 제어하는 법을 배웠습니다. 특히 히스토그램 평활화의 수학적 원리가 실제 이미지의 시인성을 어떻게 극적으로 개선하는지 확인했으며, 이는 향후 **AI 모델의 전처리(Preprocessing)** 과정에서 필수적인 역량이 될 것이라 확신합니다.
