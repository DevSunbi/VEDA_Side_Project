# VEDA_Side_Project

VEDA 5주차 20-22일 Qt 사이드 프로젝트

---

# SideProject — 계좌 관리 앱

Qt6 / C++17 기반의 개인 계좌 관리 데스크톱 애플리케이션입니다.

---

## 주요 기능

| 기능 | 설명 |
|------|------|
| 계좌 관리 | 계좌 생성 / 수정 / 비활성(보관) |
| 입금 | 입금 거래 등록 및 수정 |
| 출금 | 잔고 부족 정책 적용(차단 / 마이너스 허용) |
| 송금(이체) | 출금·입금 원자성 보장, 동일 계좌 간 송금 차단 |
| 잔고 조회 | 계좌별 / 전체 활성 계좌 합산 잔고 |
| 거래 내역 | 기간(이번달/지난달/사용자 지정) · 유형 · 계좌 필터 |
| 거래 취소/수정 | 상태 변경(`canceled`) + 잔고 자동 롤백 |

---

## 프로젝트 구조

> 파일들이 역할별로 모듈화되어 분리되었습니다.

```
SideProject_Account/
├── main.cpp
├── mainwindow.h / cpp      # 메인 UI 화면 클래스 (사용자 이벤트 및 UI 업데이트 처리)
├── bankmanager.h / cpp     # 핵심 업무 매니저 (계좌 및 거래 내역 로직 담당)
├── models.h                # 계좌(AccountModel), 거래 내역(TransactionModel) Qt 모델 클래스
├── account.h / (cpp)       # 계좌 구조체 및 관련 선언
├── transaction.h / (cpp)   # 거래 구조체 및 관련 선언
├── mainwindow.ui           # Qt Designer UI 레이아웃
└── CMakeLists.txt
```

---

## 데이터 모델

### Account
| 필드 | 타입 | 설명 |
|------|------|------|
| `id` | `int` | 내부 고유 ID |
| `name` | `QString` | 계좌명 |
| `accountNumber` | `QString` | 계좌 번호 (옵션) |
| `bankName` | `QString` | 은행명 (옵션) |
| `initialBalance` | `qint64` | 초기 잔고 |
| `createdAt` | `QDateTime` | 생성일 |
| `status` | `AccountStatus` | Active / Inactive |
| `overdraftPolicy` | `OverdraftPolicy` | Deny / Allow |
| `currentBalance` | `qint64` | 계산된 현재 잔고 |

### Transaction
| 필드 | 타입 | 설명 |
|------|------|------|
| `id` | `int` | 내부 고유 ID |
| `accountId` | `int` | 연결 계좌 ID |
| `type` | `TransactionType` | Deposit / Withdraw / TransferOut / TransferIn |
| `status` | `TransactionStatus` | Posted / Canceled |
| `amount` | `qint64` | 금액 (항상 양수) |
| `occurredAt` | `QDateTime` | 거래 일시 |
| `memo` | `QString` | 메모 (옵션) |
| `category` | `QString` | 입금 사유 (옵션) |
| `transferGroupId` | `int` | 송금 쌍 묶음 ID |

---

## 잔고 계산 원칙

```
현재 잔고 = initialBalance + Σ(입금/TransferIn) - Σ(출금/TransferOut)
          단, status == Canceled 인 거래는 제외
```

---

## 최근 진행 및 문제 해결 사항 (업데이트 내역)

- **UI 불일치 동기화**: `mainwindow.h` 및 `mainwindow.cpp`의 슬롯 함수 이름(`on_Deposit_Btn_clicked` 등)을 `mainwindow.ui`의 실제 버튼 이름들과 완벽히 일치하도록 수정하여 Qt Auto-Connect 기능 정상화.
- **계좌 생성 로직 디버깅**: `QInputDialog` 사용 시 미리 선언되지 않았던 식별자(`isSuccess`) 사용 문제 해결, 입력 취소 시 빈 계좌가 강제 생성되는 버그 방어.
- **계좌 삭제 누락 기능 구현**: `mainwindow.cpp` 호출부 대소문자 오타(`targetid` -> `targetId`) 수정 및 `AccountModel` 내부에 실질적으로 데이터를 지우는 `removeAccount(int row)` 로직 새로 추가.
- **탭 동기화 스크립트 연결**: 탭 전환 시 잔액 및 뷰를 새로고침 해주는 기능인 `refreshSummary()`를 `Sel_Acc_Tab`에 맞춰 자동 호출되도록 스크립트 이름 연결.

---

## 빌드 환경

- **Qt** : 6.x
- **C++ Standard** : C++17
- **Build System** : CMake 3.16+
- **IDE** : Qt Creator

```bash
cmake -S . -B build
cmake --build build
```

---

## 라이선스

Personal side project — All rights reserved.
