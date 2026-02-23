#pragma once

#include"Util/Util.h"
#include<iostream>


namespace Mint
{
	// DLL 내부에서만 사용하도록 한다
	class Render
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
		static void Draw(const Vector2& position,
			Color color,
			const char image)
		{
			Util::SetConsolePosition(position);
			Util::SetConsoleTextColor(color);
			Draw(image);
		}
	};
}