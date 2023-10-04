#pragma once
#include "Snake.h"
#include "framework.h"

class Food
{
public:
	Food();
	~Food();

	void Initialize(ID2D1HwndRenderTarget* m_pRenderTarget);
	void Reset(Snake* snake);
	void Draw(ID2D1HwndRenderTarget* m_pRenderTarget);

	POINT position;

private:

	ID2D1SolidColorBrush* m_pBlueBrush;
};