#pragma once

#include "../../SDK/SDK.h"

class CFeatures_Menu
{
public:
	void Render();
	bool HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

	inline bool IsOpen() const { return m_bOpen; }

private:
	void DrawCheckbox(const wchar_t* szLabel, bool& bVar, int x, int& y);
	void DrawSliderInt(const wchar_t* szLabel, int& nVar, int nMin, int nMax, int x, int& y);
	void DrawSliderFloat(const wchar_t* szLabel, float& fVar, float fMin, float fMax, int x, int& y);
	void DrawCombo(const wchar_t* szLabel, int& nVar, const wchar_t** pItems, int nCount, int x, int& y);
	void DrawSectionHeader(const wchar_t* szLabel, int x, int& y);
	void PollInput();

	bool m_bOpen = false;
	int m_nTab = 0;
	int m_nDragX = 0, m_nDragY = 0;
	bool m_bDragging = false;
	int m_nPosX = 50, m_nPosY = 50;

	int m_nMouseX = 0, m_nMouseY = 0;
	bool m_bMouseDown = false;
	bool m_bMouseClicked = false;

	int m_nActiveSlider = -1;
	int m_nSliderID = 0;
	bool m_bLastMouseDown = false;
};

namespace F { inline CFeatures_Menu Menu; }
