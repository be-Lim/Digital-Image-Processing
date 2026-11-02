# Digital-Image-Processing (디지털 영상 처리)

## 🎯 About This Repository

이 저장소는 **디지털 영상 처리(Digital Image Processing)** 강의의 핵심 이론을 학습하고, 라이브러리에 의존하지 않는 **로우레벨(Low-level) 알고리즘 구현 능력**을 기르기 위한 기록장입니다.

단순 API 호출을 넘어 메모리 구조와 행렬 연산의 원리를 직접 C/C++로 구현하며, 영상 데이터가 저장·변환·분석되는 과정을 탐구합니다.

영상 처리의 기초부터 컬러 영상 처리, 형태학, 영상 압축, 생체인식 시스템의 성능 평가까지 학습 내용을 정리합니다.

## 🛠️ Tech Stack & Environment

- **Language:** C / C++
- **Environment:** Visual Studio 2022
- **Image Format:** BMP (Grayscale / 24-bit True-color)
- **Key Focus:** Pointer Manipulation, Dynamic Memory Allocation, Matrix Operation, Binary File I/O
- **Documentation:** GitHub Markdown, LaTeX Math, Mermaid

---

## 📚 Curriculum & Progress (학습 진행 상황)

| 주차 | 핵심 주제 (Topic) | 주요 학습·구현 내용 | 링크 (Link) |
| :---: | --- | --- | :---: |
| **Week 03** | **Digital Image Files** | BMP 바이너리 구조 분석, 영상 밝기 조절 및 반전 | [📂 Go](Week03_Digital_Image_Files/) |
| **Week 04** | **Histogram & Binarization** | 히스토그램 평활화·스트레칭, Gonzalez-Woods 자동 임계값 결정 | [📂 Go](Week04_Histogram_And_Binarization/) |
| **Week 05** | **Spatial Filtering** | 2차원 컨볼루션, LPF·HPF 구현 및 자동 이진화 파이프라인 통합 | [📂 Go](Week05_Spatial_Filtering/) |
| **Week 06** | **Image Restoration** | 임펄스·가우시안 노이즈 모델과 영상 복원, Median 순서 통계 필터 구현 | [📂 Go](Week06_Image_Restoration/) |
| **Week 07** | **Image Segmentation** | Grassfire 라벨링 알고리즘, 동공 경계선 추출, 무게중심 및 바운딩 박스 시각화 | [📂 Go](Week07_Image_Segmentation_&_Feature_Extraction/) |
| **Week 09** | **Geometric Transformation** | 역방향 사상(Reverse Mapping) 기반 공간 변환 및 최근접 이웃 보간법 구현 | [📂 Go](Week09_Geometric_Transformation_&_Interpolation/) |
| **Week 10** | **Color Models & True-color Processing** | 트루컬러(24-bit) BMP 구조 분석, RGB·YCbCr·HSI 컬러 모델, 채널 분리 및 그라데이션 구현 | [📂 Go](Week10_Color_Models_&_True-color_Processing/) |
| **Week 11** | **Color Processing & Color-based Segmentation** | 의사 컬러·색 변환 이론, RGB→YCbCr 변환, 적색·피부색 마스킹, 연결요소 라벨링과 얼굴 바운딩 박스 | [📂 Go](Week11_Color_Processing_&_Color_Based_Segmentation/) |
| **Week 12** | **Morphological Image Processing** | 침식·팽창·열림·닫힘, 형태학적 경계와 기울기, Zhang-Suen 세선화·골격화, 끝점·분기점 추출 | [📂 Go](Week12_Morphological_Image_Processing/) |
| **Week 13** | **Fundamentals of Image Compression** | 중복성·정보량·엔트로피, RLE·허프만·산술 부호화, DCT·양자화, JPEG 파이프라인과 압축 품질 평가 | [📂 Go](Week13_Image_Compression/) |
| **Week 14** | **Biometric Systems & Performance Evaluation** | 생체정보의 조건, 등록·인증·식별, 임계값과 FAR·FRR·GAR·EER·ROC, 다중 생체인식 및 개인정보 보호 | [📂 Go](Week14_Biometric_Systems_And_Performance_Evaluation/) |

*강의 진행과 학습 내용에 따라 업데이트됩니다.*

---

## 💡 Repository 구조

각 주차별 폴더는 학습 주제와 실습 유무에 따라 다음과 같이 구성됩니다.

| 구성 요소 | 내용 |
| --- | --- |
| **`README.md`** | 핵심 수학적·공학적 이론, 수식, 알고리즘 처리 흐름 및 학습 내용 정리 |
| **`src.cpp` 또는 `src1.cpp`, `src2.cpp` 등** | 실습 알고리즘을 구현한 C/C++ 소스 코드와 핵심 로직 주석 |
| **입력·결과 이미지** | 실습에 사용하는 입력 영상과 처리 결과를 비교하는 이미지 |

### 실습 중심 주차

- 핵심 이론과 실제 구현을 연결합니다.
- 포인터, 동적 메모리, 화소 접근, 파일 입출력을 직접 다룹니다.
- 입력 영상과 결과 영상을 비교해 알고리즘의 동작을 확인합니다.
- 주제에 따라 하나 이상의 소스 파일을 포함합니다.

### 이론 중심 주차

**Week 13·14는 별도의 소스 코드 없이 `README.md`만으로 구성합니다.**

- **Week 13:** 영상 압축의 정보 이론과 JPEG 처리 과정을 수식과 흐름도로 정리합니다.
- **Week 14:** 생체인식 시스템의 구조와 인증·식별 방식, 성능 지표 및 보안·개인정보 보호를 정리합니다.

---

## 🔍 학습 관점

- **데이터 표현:** BMP 헤더와 화소 배열을 통해 영상의 저장 구조를 이해합니다.
- **직접 구현:** 영상 처리 연산을 C/C++로 구현해 수식과 코드의 관계를 확인합니다.
- **결과 분석:** 처리 전후 영상을 비교하고 파라미터가 결과에 미치는 영향을 살펴봅니다.
- **시스템 이해:** 개별 알고리즘을 분할·특징 추출·압축·인식 파이프라인으로 연결합니다.
- **성능 평가:** 시각적 결과뿐 아니라 압축 품질, 인식 오류율, 운용 임계값을 함께 고려합니다.
