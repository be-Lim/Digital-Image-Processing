# 📝 [Week 07] 영상 분할 및 객체 특징 추출 (Image Segmentation & Feature Extraction)

## 1. 실습 개요
본 실습은 단순한 화질 개선(Image Processing)을 넘어, 영상 내에서 의미 있는 특정 객체를 찾아내는 **컴퓨터 비전(Computer Vision)**의 기초 단계를 구현합니다. 눈(Eye) 영상에서 관심 영역(ROI)인 **'동공(Pupil)'**만을 분할(Segmentation)해내고, 라벨링(Labeling)을 통해 객체를 식별한 뒤, 해당 객체의 외곽선(Boundary), 무게 중심(Center of Gravity), 그리고 바운딩 박스(Bounding Box)를 추출하는 일련의 파이프라인을 C++ 로우레벨 코드로 구축하였습니다.

---

## 2. 핵심 이론 및 알고리즘 (Key Concepts)

| 알고리즘 종류 | 역할 및 특징 | 본 실습에서의 활용 |
| :--- | :--- | :--- |
| **이진화 (Binarization)** | 영상을 전경(Foreground)과 배경(Background)으로 분리 | 임계값을 설정하여 검은색 동공 영역만 추출 (Threshold: 30, 50) |
| **라벨링 (Labeling)** | 연결된 픽셀 그룹에 동일한 ID를 부여하여 독립된 객체로 인식 | **Grassfire 알고리즘**을 사용하여 영상 내 여러 노이즈 중 가장 면적이 큰 동공만 필터링 (Size Filtering) |
| **경계 추적 (Boundary Tracking)** | 객체의 윤곽선 픽셀만 추출 | 4방향(상하좌우) 인접 화소 검사를 통해 내부 픽셀과 경계선 픽셀을 구분 |
| **특징 추출 (Feature Extraction)** | 객체의 정량적 데이터(위치, 크기 등) 확보 | 동공의 무게 중심(Cx, Cy)을 계산하고, 객체를 감싸는 Bounding Box 좌표 획득 |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 파이프라인 1: 동공 경계선 추출 (`pupil1_edge`)
이진화를 거친 영상에는 동공 외에도 눈썹이나 그림자 같은 노이즈가 존재합니다. 이를 제거하고 순수한 동공의 윤곽선만 따내는 파이프라인입니다.

1. **Size Filtering (가장 큰 객체 추출):**
   `m_BlobColoring` 함수 내에서 Grassfire 알고리즘으로 모든 객체에 라벨을 붙인 뒤, 가장 면적이 넓은(`Out_Area`) 객체 하나만 전경으로 남기고 나머지는 배경으로 날려버립니다.
2. **4방향 경계 검출 로직:**
   전경 픽셀(0)을 탐색할 때, 상하좌우 4방향 픽셀이 **모두 전경이 아니라면** 해당 픽셀은 객체의 가장자리(경계)로 간주하고 255(흰색)로 출력합니다.
```cpp
if (Bin[i * W + j] == 0) { // 현재 픽셀이 전경(동공)일 때
    // 상, 하, 좌, 우 픽셀 중 하나라도 배경(255)이 섞여 있다면 윤곽선으로 판단
    if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 &&
          Bin[i * W + (j - 1)] == 0 && Bin[i * W + (j + 1)] == 0))
        Out[i * W + j] = 255;
}
```

### 파이프라인 2: 무게 중심 및 바운딩 박스 추적 (pupil1_cross_bound)
정제된 동공 영역의 기하학적 특징(Feature)을 수치화하여 모니터에 시각화하는 과정입니다.

1. **무게 중심(Center of Gravity) 계산:**
동공 영역으로 판별된 모든 픽셀의 X, Y 좌표를 각각 누적 합산한 뒤, 총 픽셀 개수(면적)로 나누어 평균 좌표(Cx, Cy)를 구합니다.
```cpp
// 2D 이미지의 무게 중심 산출 공식
*Cx = SumX / cnt;
*Cy = SumY / cnt;
```
2. **바운딩 박스(Bounding Box) 탐색:**
영상의 상, 하, 좌, 우 4방향에서 각각 스캔을 시작하여, 가장 처음으로 전경(동공) 픽셀이 발견되는 시점의 좌표를 LUY(위), RDY(아래), LUX(왼쪽), RDX(오른쪽)로 저장합니다.

3. **시각화 (Drawing):**
계산된 좌표를 바탕으로 원본 이미지 위에 십자가(Cross Line)와 사각형 테두리(Rect Outline)를 그려 넣습니다.

---

## 4. 트러블슈팅 및 공학적 고찰 (Technical Insights)

### 재귀호출(Recursive)의 한계와 스택(Stack) 활용:
Grassfire 알고리즘 구현 시, 단순 재귀호출을 사용하면 동공처럼 큰 객체를 라벨링할 때 시스템 스택 오버플로우(Stack Overflow)가 발생할 위험이 있습니다. 이를 방지하기 위해 push와 pop 함수를 구현하여 메모리에 동적 할당된 배열(stackx, stacky)을 스택처럼 활용하여 안정성을 확보했습니다.

---

## 5. 실행 결과 (Visual Results)

| 원본 영상 (pupil1.bmp) | 동공 경계 추출 (Edge) | 무게중심 및 바운딩 박스 |
| :--- | :--- | :--- |
| ![pupil1.bmp](https://github.com/user-attachments/files/27232975/pupil1.bmp) | ![output_pupil1_edge.bmp](https://github.com/user-attachments/files/27232986/output_pupil1_edge.bmp) | ![output_pupil1_cross_bound.bmp](https://github.com/user-attachments/files/27232992/output_pupil1_cross_bound.bmp) |

---

## 💡 학습 소감
이번 주차는 영상 처리(Image Processing) 기법들이 어떻게 컴퓨터 비전(Computer Vision)으로 진화하는지 직접 코드로 확인한 실습이었습니다.
이전 주차까지는 이미지를 단순히 '보기 좋게' 만드는 것에 집중했다면, 이번에는 영상을 픽셀의 집합이 아닌 '위치와 크기를 가진 객체(Object)'로 컴퓨터가 이해하게 만드는 과정을 경험했습니다.
특히 라벨링을 통해 노이즈를 걸러내고 동공의 무게 중심과 바운딩 박스를 추출하는 이 로직은, 향후 딥러닝 기반의 객체 탐지(Object Detection, YOLO 등) 모델을 다룰 때 가장 밑바탕이 되는 핵심 파이프라인임을 깨달았습니다.
