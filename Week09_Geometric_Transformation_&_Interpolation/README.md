# 📝 [Week 09] 영상의 기하학적 변환 및 보간법 (Geometric Transformation & Interpolation)

## 1. 실습 개요
본 실습은 영상의 픽셀 밝기를 조작하는 것을 넘어, 픽셀의 물리적 위치(Coordinate)를 2차원 공간상에서 재배치하는 기하학적 변환(Geometric Transformation)을 다룹니다. 평행 이동, 대칭 반전, 확대/축소, 회전 등의 변환을 C++ 로우레벨 코드로 구현하였습니다. 특히 좌표 변환 과정에서 발생하는 픽셀의 빈 공간(Hole)과 중첩(Overlap) 문제를 해결하기 위해 역방향 사상(Reverse Mapping)을 필수적으로 적용하였으며, 소수점 좌표를 처리하는 보간법(Interpolation)의 원리를 코드로 체득하였습니다.

---

## 2. 핵심 이론 및 알고리즘 (Key Concepts)
| 알고리즘 종류 | 역할 및 특징 | 본 실습에서의 활용 |
| :--- | :--- | :--- |
| **기하학적 변환 (Geometric Trans.)** | 픽셀의 공간적 위치를 이동, 대칭, 확대/축소, 회전시켜 영상을 재배치 | 삼각함수와 변환 행렬을 이용해 영상의 형태와 크기를 조작 |
| **역방향 사상 (Reverse Mapping)** | 출력(목적) 영상의 좌표에서 역연산을 통해 원본 영상의 입력 화소를 추적 | 순방향 사상에서 발생하는 홀(Hole)과 중첩(Overlap) 문제 원천 차단 |
| **최근접 이웃 보간법 (Nearest Neighbor)** | 계산된 소수점 주소를 반올림하여 가장 가까운 정수 주소의 픽셀값을 할당 | 스케일링 시 int 형변환을 이용해 처리 속도를 최적화 (Aliasing 관찰) |
| **양선형 보간법 (Bilinear Interpolation)** | 주변 4개 화소의 거리에 반비례하는 가중합(Weighted Sum)을 이용해 픽셀 생성 | 최근접 이웃 보간법의 계단 현상을 극복하는 스무딩 기법으로 원리 학습 |

---

## 3. 핵심 구현 로직 (Implementation Details)

### 파이프라인 1: 역방향 사상 기반의 크기 변환 (Scaling)
단순히 원본 화소를 밀어내는 것이 아니라, 목적 영상의 크기만큼 반복문을 돌며 원본의 좌표를 역으로 찾아오는 역방향 사상 파이프라인입니다.

1. **역연산 및 최근접 이웃 보간법:**
   출력 좌표 (j, i)를 스케일 팩터(SF_X, SF_Y)로 나누어 원본 좌표를 역연산합니다. 이때 int로 캐스팅하여 소수점을 버림으로써 가장 가까운 정수 좌표를 참조하는 '최근접 이웃 보간법'을 수행합니다.
2. **범위 초과 방지 로직:**
   역산된 좌표가 원본 이미지의 크기(W, H)를 벗어나지 않을 때만 픽셀값을 복사하여 쓰레기값 참조를 방지합니다.
```cpp
for (int i = 0; i < H; i++) {
  for (int j = 0; j < W; j++) {
    // 역방향 사상이므로 역연산 시행 후 int 캐스팅 (최근접 이웃 보간법)
    int tmpX = (int)(j / SF_X); 
    int tmpY = (int)(i / SF_Y);
        
    if ((tmpY < H) && (tmpX < W))
      Tmp[i * W + j] = Img[tmpY * W + tmpX];
  }
}
```

### 파이프라인 2: 영상 중심 기준 회전 변환 (Rotation_center)
영상의 원점(0, 0)이 아닌, 영상의 정중앙을 축으로 삼아 영상을 회전시키는 파이프라인입니다.

1. **중심축 이동 및 역회전 연산:**
   좌표를 영상의 중심(X_center, Y_center)으로 이동시킨 후, 삼각함수(sin, cos)를 이용해 시계방향(역방향)으로 회전 연산을 수행합니다.
```cpp
// 2D 이미지의 영상 중심 기준 역회전 산출 공식
tmpX = (int)(cos(Radian) * (j - X_center) + sin(Radian) * (i - Y_center) + X_center);
tmpY = (int)(-sin(Radian) * (j - X_center) + cos(Radian) * (i - Y_center) + Y_center);
```

---

## 4. 트러블슈팅 및 공학적 고찰 (Technical Insights)

### 순방향 사상(Forward Mapping)의 치명적 한계:
원본 영상의 화소들을 확대하거나 회전시켜 목적 영상에 차례대로 뿌릴 경우, 계산된 소수점 좌표들이 흩어지며 목적 영상 곳곳에 픽셀이 비어버리는 검은 구멍(Hole)이나 한 픽셀에 두 값이 덮어씌워지는 중첩(Overlap) 현상이 발생합니다. 이를 해결하기 위해 출력 영상의 모든 빈 픽셀에서부터 출발하여 원본의 픽셀을 거꾸로 당겨오는 역방향 사상(Reverse Mapping)을 설계하여 완벽한 이미지를 생성해냈습니다.

### 최근접 이웃 보간법의 Aliasing 현상 증명:
Scaling Up 영상의 대각선 모서리 부분을 관찰한 결과, 계단 모양으로 픽셀이 깨지는 알리아싱(Aliasing) 현상이 발생했습니다. 이는 C++ 코드 구현 시 int 캐스팅을 사용하여 소수점 이하 정보를 무시하고 억지로 가장 가까운 화소로 매핑했기 때문이며, 이론으로 배운 보간법의 한계를 시각적으로 명확히 확인하는 계기가 되었습니다.

---

## 5. 실행 결과 (Visual Results)

| Original, Translation (50, 30) | Horizontal Flip, Vertical Flip | Scaling Up (1.5, 1.2), Scaling Down (0.5, 0.7) | Rotation Zero, Rotation Center |
| :--- | :--- | :--- | :--- |
| ![LENNA.bmp](https://github.com/user-attachments/files/27328608/LENNA.bmp) | ![horizontal_flip.bmp](https://github.com/user-attachments/files/27328612/horizontal_flip.bmp) | ![scaling_up.bmp](https://github.com/user-attachments/files/27328616/scaling_up.bmp) | ![rotation_zero.bmp](https://github.com/user-attachments/files/27328621/rotation_zero.bmp) |
| ![translation.bmp](https://github.com/user-attachments/files/27328625/translation.bmp) | ![vertical_flip.bmp](https://github.com/user-attachments/files/27328629/vertical_flip.bmp) | ![scaling_down.bmp](https://github.com/user-attachments/files/27328632/scaling_down.bmp) | ![rotation_center.bmp](https://github.com/user-attachments/files/27328635/rotation_center.bmp) |

---

## 💡 학습 소감
이번 주차는 단순히 픽셀의 밝기를 바꾸는 것을 넘어, 공간 상의 좌표를 제어하는 기하학적 변환의 원리를 로우레벨부터 구현해 본 뜻깊은 경험이었습니다. 특히 좌표의 역연산을 통해 빈 공간(Hole)을 채우는 역방향 사상(Reverse Mapping)의 개념은 컴퓨터 비전 파이프라인의 필수적인 요소임을 깨달았습니다.
나아가 이번 실습은 현대 딥러닝 아키텍처의 근본을 이해하는 통찰력을 제공했습니다. 딥러닝 모델 학습 시 데이터 부족을 해결하는 Data Augmentation(데이터 증강)의 코어 로직이 이 코드와 동일하게 동작함을 알 수 있었습니다. 또한, CNN의 차원 복원을 위한 UpSampling 레이어나, 최신 객체 탐지 알고리즘(Mask R-CNN)에서 오차를 바로잡는 RoI Align 기법 역시 이번에 학습한 역방향 사상과 보간법(Bilinear/Nearest)에 수학적 뿌리를 두고 있다는 점을 체감할 수 있었습니다.
