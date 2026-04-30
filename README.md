# Digital-Image-Processing (디지털 영상 처리)

## 🎯 About This Repository
이 저장소는 **디지털 영상 처리(Digital Image Processing)** 강의의 핵심 이론을 학습하고, 라이브러리에 의존하지 않는 **로우레벨(Low-level) 알고리즘 구현 능력**을 기르기 위한 기록장입니다. 

향후 컴퓨터 비전(Computer Vision) 및 인공지능(AI) 연구의 밑거름이 될 수 있도록, 단순 API 호출을 넘어 메모리 구조와 행렬 연산의 원리를 직접 C/C++로 구현하며 깊이 있게 탐구합니다.

## 🛠️ Tech Stack & Environment
- **Language:** C / C++ 
- **Environment:** Visual Studio 2022
- **Key Focus:** Pointer Manipulation, Dynamic Memory Allocation, Matrix Operation, Binary File I/O

---

## 📚 Curriculum & Progress (학습 진행 상황)

| 주차 | 핵심 주제 (Topic) | 주요 구현 내용 | 링크 (Link) |
| :---: | :--- | :--- | :---: |
| **Week 03** | **Digital_Image_Files** | BMP 바이너리 구조 분석, 영상 밝기 조절 및 반전 | [📂 Go](Week03_Digital_Image_Files/) |
| **Week 04** | **Histogram & Binarization** | 히스토그램 평활화/스트레칭, Gonzalez-Woods 자동 임계값 결정 | [📂 Go](Week04_Histogram_And_Binarization/) |
| **Week 05** | **Spatial Filtering** | 2차원 컨볼루션, LPF/HPF 구현 및 자동 이진화 파이프라인 통합 | [📂 Go](Week05_Spatial_Filtering/) |
| **Week 06** | **Image Restoration** | 임펄스/가우시안 노이즈 모델 복원, Median 순서 통계 필터 구현 | [📂 Go](Week06_Image_Restoration/) |
| **Week 07** | **Image Segmentation** | Grassfire 라벨링 알고리즘, 동공 경계선(Edge) 추출, 무게중심 및 바운딩 박스 시각화 | [📂 Go](Week07_Image_Segmentation_&_Feature_Extraction/) |

*(강의 진행에 따라 매주 업데이트됩니다.)*

---

## 💡 Repository 구조
본 저장소의 각 주차별 폴더는 다음 원칙에 따라 구성됩니다.
1. `README.md` : 해당 주차의 **핵심 수학적/공학적 이론 요약** 및 **실제 실행 결과(Before/After 이미지)** 증명
2. `src.cpp` : 알고리즘이 구현된 메인 소스 코드 (핵심 로직 주석 포함)
