# DeltieGame

\# 🌿 DeltieGame: MintEngine



\*\*MintEngine\*\*을 기반으로 개발 중인 콘솔 시뮬레이션 게임 \*\*DeltieGame\*\* 프로젝트입니다.



\## 🚀 프로젝트 개요

이 프로젝트는 상용 엔진(Unreal Engine)의 구조를 학습하며, C++을 이용해 자신만의 게임 엔진을 구축하는 것을 목표로 합니다. 현재 본인이 LLM 모델에 페르소나를 주입중인 AI 캐릭터 \*\*Lichier(Siel)\*\*을 중심으로 한 시뮬레이션 기능을 최종 목표로 두고 있는 중입니다. 현재 Windows Console API에 의존해 렌더링 시스템을 구축하고 있지만 엔진의 유연한 확장을 위해 추상화를 목표로 하고 있습니다. 추후 그래픽 라이브러리로의 전환 및 멀티 플랫폼 지원을 고려하고 있습니다.



\## 🛠 기술 스택

\* \*\*Language:\*\* C++ (Google C++ Style Guide 준수)

\* \*\*Platform:\*\* Windows Console (Win32 API)

\* \*\*Development Environment:\*\* Visual Studio 2022, x64

\* \*\*Key Features:\*\* \* 유니코드 기반 전각 문자 렌더링 시스템

&nbsp; \* 계층형 레이어 아키텍처 (Core, Engine, Render, Level, Actor)

&nbsp; \* Gemini API 연동을 통한 AI 캐릭터 상호작용 (예정)



\## 📂 프로젝트 구조



\* \*\*MintEngine:\*\* 핵심 게임 엔진 모듈 (DLL 방식)

\* \*\*GuildManager:\*\* 엔진을 사용하는 클라이언트 게임 프로젝트

\* \*\*Config:\*\* 엔진 및 게임 설정 파일 (Setting.txt)



\## ⚙️ 실행 방법

1\. `DeltieGame.sln` 파일을 비주얼 스튜디오에서 엽니다.

2\. `GuildManager` 프로젝트를 \*\*시작 프로젝트\*\*로 설정합니다.

3\. `x64 Debug` 또는 `Release` 모드로 빌드 후 실행(`F5`)합니다.



---

© 2026 Deltea. All rights reserved.

