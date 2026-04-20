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

```
SideProject_Account/
├── models/
│   ├── account.h           # Account 구조체, AccountStatus, OverdraftPolicy
│   └── transaction.h       # Transaction 구조체, TransactionType, TransactionStatus
├── modules/
│   ├── accountmanager.h    # 계좌 CRUD / 잔고 계산 / 공유 데이터
│   ├── depositmodule.h     # 입금 등록 / 수정
│   ├── withdrawmodule.h    # 출금 등록 / 수정 / 정책 확인
│   ├── transfermodule.h    # 송금 원자성 처리
│   ├── transactionfilter.h # 거래 필터 / 정렬 / 합계
│   ├── balancequery.h      # 잔고 조회 / 시계열 스냅샷
│   └── correctionmodule.h  # 거래 취소 / 수정 / 롤백
├── main.cpp
├── mainwindow.h / .cpp / .ui
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
