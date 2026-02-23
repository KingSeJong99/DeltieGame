#pragma once

#include"Util/Util.h"
#include<iostream>
#include<vector>

namespace Mint
{
	// DLL 내부에서만 사용하도록 한다
	class Renderer
	{
	public:
		// 간단한 그리기 함수
		static void Draw(const char image)
		{
			std::cout << image;
		}

		// 위치 설정 및 그리기 한 번에 처리하는 함수
		static void Draw(const Vector2& position, const char image)
		{
			Util::SetConsolePosition(position);
			Draw(image);
		}

		// 위치, 색상 설정 및 그리기
		static void Draw(
			const Vector2& position,
			Color color,
			const char image)
		{
			// 커서 위치 설정
			Util::SetConsolePosition(position);

			// 텍스트 색상 설정
			Util::SetConsoleTextColor(color);

			// 글자를 출력한다
			Draw(image);

			// 원래 색상으로 되돌린다
			Util::SetConsoleTextColor(Color::White);
		}
	};
}