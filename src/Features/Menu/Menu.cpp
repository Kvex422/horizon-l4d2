#include "Menu.h"

#include "../Vars.h"

static constexpr int MENU_W = 520;
static constexpr int MENU_H = 480;
static constexpr int TAB_H = 32;
static constexpr int ITEM_H = 18;
static constexpr int PADDING = 12;
static constexpr int CHECKBOX_SZ = 12;
static constexpr int SLIDER_W = 140;
static constexpr int SLIDER_H = 10;
static constexpr int COL_W = 240;

static const Color clrBg       = { 22, 22, 28, 240 };
static const Color clrHeader   = { 30, 30, 40, 255 };
static const Color clrTab      = { 40, 40, 55, 255 };
static const Color clrTabAct   = { 80, 60, 180, 255 };
static const Color clrAccent   = { 100, 80, 200, 255 };
static const Color clrCheck    = { 100, 200, 100, 255 };
static const Color clrUncheck  = { 55, 55, 65, 255 };
static const Color clrText     = { 210, 210, 220, 255 };
static const Color clrDim      = { 140, 140, 150, 255 };
static const Color clrSliderBg = { 40, 40, 50, 255 };
static const Color clrSliderFg = { 100, 80, 200, 255 };
static const Color clrSection  = { 130, 110, 220, 255 };

static const wchar_t* s_szTabs[] = { L"Aimbot", L"ESP", L"Misc", L"Visuals" };
static constexpr int NUM_TABS = 4;

static const wchar_t* s_szAimbotPriority[] = { L"FOV", L"Distance" };
static const wchar_t* s_szAimbotHitbox[] = { L"Head", L"Body", L"Auto" };

bool CFeatures_Menu::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		m_bOpen = !m_bOpen;
		return true;
	}

	if (!m_bOpen)
		return false;

	return (m_nMouseX >= m_nPosX && m_nMouseX <= m_nPosX + MENU_W &&
		m_nMouseY >= m_nPosY && m_nMouseY <= m_nPosY + MENU_H);
}

void CFeatures_Menu::PollInput()
{
	POINT pt;
	GetCursorPos(&pt);

	HWND hwnd = FindWindowW(L"Valve001", nullptr);
	if (hwnd)
		ScreenToClient(hwnd, &pt);

	m_nMouseX = pt.x;
	m_nMouseY = pt.y;

	bool bNowDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	m_bMouseClicked = (bNowDown && !m_bLastMouseDown);
	m_bMouseDown = bNowDown;
	m_bLastMouseDown = bNowDown;

	if (!m_bMouseDown)
	{
		m_bDragging = false;
		m_nActiveSlider = -1;
	}
}

void CFeatures_Menu::Render()
{
	if (!m_bOpen)
		return;

	PollInput();
	m_nSliderID = 0;

	const int x = m_nPosX;
	const int y = m_nPosY;

	G::Draw.Rect(x, y, MENU_W, MENU_H, clrBg);
	G::Draw.OutlinedRect(x, y, MENU_W, MENU_H, clrAccent);

	G::Draw.Rect(x, y, MENU_W, TAB_H, clrHeader);
	G::Draw.String(EFonts::MENU_ARIAL, x + 8, y + 8, clrAccent, TXT_DEFAULT, L"Horizon");

	if (m_bMouseClicked &&
		m_nMouseX >= x && m_nMouseX <= x + MENU_W &&
		m_nMouseY >= y && m_nMouseY <= y + TAB_H)
	{
		m_bDragging = true;
		m_nDragX = m_nMouseX - m_nPosX;
		m_nDragY = m_nMouseY - m_nPosY;
	}

	if (m_bDragging && m_bMouseDown)
	{
		m_nPosX = m_nMouseX - m_nDragX;
		m_nPosY = m_nMouseY - m_nDragY;
	}

	const int nTabW = (MENU_W - 100) / NUM_TABS;

	for (int n = 0; n < NUM_TABS; n++)
	{
		const int nTabX = x + 100 + (n * nTabW);
		const Color clr = (m_nTab == n) ? clrTabAct : clrTab;

		G::Draw.Rect(nTabX, y + 2, nTabW - 2, TAB_H - 4, clr);
		G::Draw.String(EFonts::MENU_TAHOMA, nTabX + nTabW / 2, y + TAB_H / 2, clrText, TXT_CENTERXY, s_szTabs[n]);

		if (m_bMouseClicked &&
			m_nMouseX >= nTabX && m_nMouseX <= nTabX + nTabW &&
			m_nMouseY >= y + 2 && m_nMouseY <= y + TAB_H)
		{
			m_nTab = n;
		}
	}

	int nDrawY = y + TAB_H + PADDING;
	const int nLeftCol = x + PADDING;
	const int nRightCol = x + PADDING + COL_W + 10;

	switch (m_nTab)
	{
		case 0:
		{
			DrawSectionHeader(L"Aimbot", nLeftCol, nDrawY);
			DrawCheckbox(L"Enabled", Vars::Aimbot::bEnabled, nLeftCol, nDrawY);
			DrawCheckbox(L"Silent Aim", Vars::Aimbot::bSilent, nLeftCol, nDrawY);
			DrawCheckbox(L"Visibility Check", Vars::Aimbot::bVisCheck, nLeftCol, nDrawY);
			DrawCheckbox(L"Auto Fire", Vars::Aimbot::bAutoFire, nLeftCol, nDrawY);
			DrawSliderFloat(L"FOV", Vars::Aimbot::flFOV, 1.0f, 180.0f, nLeftCol, nDrawY);
			DrawCombo(L"Priority", Vars::Aimbot::nPriority, s_szAimbotPriority, 2, nLeftCol, nDrawY);
			DrawCombo(L"Hitbox", Vars::Aimbot::nHitbox, s_szAimbotHitbox, 3, nLeftCol, nDrawY);
			DrawCheckbox(L"FOV Circle", Vars::Aimbot::bFOVCircle, nLeftCol, nDrawY);

			nDrawY = y + TAB_H + PADDING;
			DrawSectionHeader(L"Targets", nRightCol, nDrawY);
			DrawCheckbox(L"Common Infected", Vars::Aimbot::bTargetInfected, nRightCol, nDrawY);
			DrawCheckbox(L"Special Infected", Vars::Aimbot::bTargetSpecial, nRightCol, nDrawY);
			DrawCheckbox(L"Witch", Vars::Aimbot::bTargetWitch, nRightCol, nDrawY);
			DrawCheckbox(L"Survivors (PvP)", Vars::Aimbot::bTargetSurvivors, nRightCol, nDrawY);

			nDrawY += 10;
			DrawSectionHeader(L"Triggerbot", nRightCol, nDrawY);
			DrawCheckbox(L"Enabled", Vars::Triggerbot::bEnabled, nRightCol, nDrawY);
			DrawCheckbox(L"Infected Only", Vars::Triggerbot::bInfectedOnly, nRightCol, nDrawY);
			break;
		}
		case 1:
		{
			DrawSectionHeader(L"ESP", nLeftCol, nDrawY);
			DrawCheckbox(L"Enabled", Vars::ESP::bEnabled, nLeftCol, nDrawY);
			DrawCheckbox(L"Bounding Box", Vars::ESP::bBox, nLeftCol, nDrawY);
			DrawCheckbox(L"Health Bar", Vars::ESP::bHealthBar, nLeftCol, nDrawY);
			DrawCheckbox(L"Name", Vars::ESP::bName, nLeftCol, nDrawY);
			DrawCheckbox(L"Weapon", Vars::ESP::bWeapon, nLeftCol, nDrawY);
			DrawCheckbox(L"Distance", Vars::ESP::bDistance, nLeftCol, nDrawY);
			DrawCheckbox(L"Snaplines", Vars::ESP::bSnaplines, nLeftCol, nDrawY);

			nDrawY = y + TAB_H + PADDING;
			DrawSectionHeader(L"Filters", nRightCol, nDrawY);
			DrawCheckbox(L"Players", Vars::ESP::bPlayers, nRightCol, nDrawY);
			DrawCheckbox(L"Common Infected", Vars::ESP::bInfected, nRightCol, nDrawY);
			DrawCheckbox(L"Special Infected", Vars::ESP::bSpecial, nRightCol, nDrawY);
			DrawCheckbox(L"Witch", Vars::ESP::bWitch, nRightCol, nDrawY);
			DrawCheckbox(L"Items / Weapons", Vars::ESP::bItems, nRightCol, nDrawY);

			nDrawY += 10;
			DrawSectionHeader(L"Glow", nRightCol, nDrawY);
			DrawCheckbox(L"Glow ESP", Vars::ESP::bGlow, nRightCol, nDrawY);
			break;
		}
		case 2:
		{
			DrawSectionHeader(L"Movement", nLeftCol, nDrawY);
			DrawCheckbox(L"Bunny Hop", Vars::Misc::bBhop, nLeftCol, nDrawY);
			DrawCheckbox(L"Auto Strafe", Vars::Misc::bAutoStrafe, nLeftCol, nDrawY);
			DrawCheckbox(L"Auto Deadstop", Vars::Misc::bAutoDeadstop, nLeftCol, nDrawY);
			DrawCheckbox(L"Speedhack (XBUTTON1)", Vars::Misc::bSpeedhack, nLeftCol, nDrawY);
			DrawSliderInt(L"Speed Factor", Vars::Misc::nSpeedhackFactor, 1, 20, nLeftCol, nDrawY);

			nDrawY += 10;
			DrawSectionHeader(L"Weapons", nLeftCol, nDrawY);
			DrawCheckbox(L"No Spread", Vars::Misc::bNoSpread, nLeftCol, nDrawY);
			DrawCheckbox(L"No Recoil", Vars::Misc::bNoRecoil, nLeftCol, nDrawY);

			nDrawY = y + TAB_H + PADDING;
			DrawSectionHeader(L"Fakelag", nRightCol, nDrawY);
			DrawCheckbox(L"Enabled", Vars::Misc::bFakelag, nRightCol, nDrawY);
			DrawSliderInt(L"Ticks", Vars::Misc::nFakelagTicks, 1, 14, nRightCol, nDrawY);

			nDrawY += 10;
			DrawSectionHeader(L"Feedback", nRightCol, nDrawY);
			DrawCheckbox(L"Hitmarker", Vars::Misc::bHitmarker, nRightCol, nDrawY);
			DrawCheckbox(L"Hit Sound", Vars::Misc::bHitSound, nRightCol, nDrawY);
			break;
		}
		case 3:
		{
			DrawSectionHeader(L"Chams", nLeftCol, nDrawY);
			DrawCheckbox(L"Enabled", Vars::Chams::bEnabled, nLeftCol, nDrawY);
			DrawCheckbox(L"Players", Vars::Chams::bPlayers, nLeftCol, nDrawY);
			DrawCheckbox(L"Common Infected", Vars::Chams::bInfected, nLeftCol, nDrawY);
			DrawCheckbox(L"Ignore Z (Wallhack)", Vars::Chams::bIgnoreZ, nLeftCol, nDrawY);

			nDrawY = y + TAB_H + PADDING;
			DrawSectionHeader(L"World", nRightCol, nDrawY);
			DrawCheckbox(L"No Fog", Vars::Misc::bNoFog, nRightCol, nDrawY);
			DrawSliderFloat(L"FOV Override", Vars::Misc::flFOVOverride, 0.0f, 150.0f, nRightCol, nDrawY);
			DrawSliderFloat(L"Viewmodel FOV", Vars::Misc::flViewmodelFOV, 0.0f, 130.0f, nRightCol, nDrawY);
			break;
		}
	}

	G::Draw.String(EFonts::MENU_TAHOMA, x + MENU_W / 2, y + MENU_H - 14, clrDim, TXT_CENTERXY, L"INSERT to toggle | Polonium");

	m_bMouseClicked = false;
}

void CFeatures_Menu::DrawCheckbox(const wchar_t* szLabel, bool& bVar, int x, int& y)
{
	const Color clr = bVar ? clrCheck : clrUncheck;

	G::Draw.Rect(x, y + 2, CHECKBOX_SZ, CHECKBOX_SZ, clr);
	G::Draw.OutlinedRect(x, y + 2, CHECKBOX_SZ, CHECKBOX_SZ, { 0, 0, 0, 180 });
	G::Draw.String(EFonts::MENU_TAHOMA, x + CHECKBOX_SZ + 6, y + 1, clrText, TXT_DEFAULT, szLabel);

	if (m_bMouseClicked &&
		m_nMouseX >= x && m_nMouseX <= x + COL_W &&
		m_nMouseY >= y && m_nMouseY <= y + ITEM_H)
	{
		bVar = !bVar;
	}

	y += ITEM_H;
}

void CFeatures_Menu::DrawSliderInt(const wchar_t* szLabel, int& nVar, int nMin, int nMax, int x, int& y)
{
	const int nSliderID = m_nSliderID++;
	const int nSliderX = x + COL_W - SLIDER_W;
	const float flRatio = static_cast<float>(nVar - nMin) / static_cast<float>(nMax - nMin);
	const int nFillW = static_cast<int>(SLIDER_W * flRatio);

	G::Draw.String(EFonts::MENU_TAHOMA, x, y + 1, clrText, TXT_DEFAULT, szLabel);
	G::Draw.Rect(nSliderX, y + 3, SLIDER_W, SLIDER_H, clrSliderBg);
	G::Draw.Rect(nSliderX, y + 3, nFillW, SLIDER_H, clrSliderFg);
	G::Draw.OutlinedRect(nSliderX, y + 3, SLIDER_W, SLIDER_H, { 0, 0, 0, 180 });

	wchar_t szBuf[32];
	swprintf_s(szBuf, L"%d", nVar);
	G::Draw.String(EFonts::MENU_TAHOMA, nSliderX + SLIDER_W + 4, y + 1, clrDim, TXT_DEFAULT, szBuf);

	if (m_bMouseDown &&
		m_nMouseX >= nSliderX && m_nMouseX <= nSliderX + SLIDER_W &&
		m_nMouseY >= y && m_nMouseY <= y + ITEM_H)
	{
		m_nActiveSlider = nSliderID;
	}

	if (m_bMouseDown && m_nActiveSlider == nSliderID)
	{
		const float flNew = static_cast<float>(m_nMouseX - nSliderX) / static_cast<float>(SLIDER_W);
		nVar = nMin + static_cast<int>(flNew * (nMax - nMin));
		if (nVar < nMin) nVar = nMin;
		if (nVar > nMax) nVar = nMax;
	}

	y += ITEM_H + 4;
}

void CFeatures_Menu::DrawSliderFloat(const wchar_t* szLabel, float& fVar, float fMin, float fMax, int x, int& y)
{
	const int nSliderID = m_nSliderID++;
	const int nSliderX = x + COL_W - SLIDER_W;
	const float flRatio = (fVar - fMin) / (fMax - fMin);
	const int nFillW = static_cast<int>(SLIDER_W * flRatio);

	G::Draw.String(EFonts::MENU_TAHOMA, x, y + 1, clrText, TXT_DEFAULT, szLabel);
	G::Draw.Rect(nSliderX, y + 3, SLIDER_W, SLIDER_H, clrSliderBg);
	G::Draw.Rect(nSliderX, y + 3, nFillW, SLIDER_H, clrSliderFg);
	G::Draw.OutlinedRect(nSliderX, y + 3, SLIDER_W, SLIDER_H, { 0, 0, 0, 180 });

	wchar_t szBuf[32];
	swprintf_s(szBuf, L"%.1f", fVar);
	G::Draw.String(EFonts::MENU_TAHOMA, nSliderX + SLIDER_W + 4, y + 1, clrDim, TXT_DEFAULT, szBuf);

	if (m_bMouseDown &&
		m_nMouseX >= nSliderX && m_nMouseX <= nSliderX + SLIDER_W &&
		m_nMouseY >= y && m_nMouseY <= y + ITEM_H)
	{
		m_nActiveSlider = nSliderID;
	}

	if (m_bMouseDown && m_nActiveSlider == nSliderID)
	{
		float flNew = static_cast<float>(m_nMouseX - nSliderX) / static_cast<float>(SLIDER_W);
		if (flNew < 0.0f) flNew = 0.0f;
		if (flNew > 1.0f) flNew = 1.0f;
		fVar = fMin + flNew * (fMax - fMin);
	}

	y += ITEM_H + 4;
}

void CFeatures_Menu::DrawCombo(const wchar_t* szLabel, int& nVar, const wchar_t** pItems, int nCount, int x, int& y)
{
	G::Draw.String(EFonts::MENU_TAHOMA, x, y + 1, clrText, TXT_DEFAULT, szLabel);

	const int nComboX = x + COL_W - SLIDER_W;
	G::Draw.Rect(nComboX, y + 2, SLIDER_W, CHECKBOX_SZ + 2, clrSliderBg);
	G::Draw.OutlinedRect(nComboX, y + 2, SLIDER_W, CHECKBOX_SZ + 2, { 0, 0, 0, 180 });

	if (nVar >= 0 && nVar < nCount)
		G::Draw.String(EFonts::MENU_TAHOMA, nComboX + 4, y + 1, clrText, TXT_DEFAULT, pItems[nVar]);

	if (m_bMouseClicked &&
		m_nMouseX >= nComboX && m_nMouseX <= nComboX + SLIDER_W &&
		m_nMouseY >= y && m_nMouseY <= y + ITEM_H)
	{
		nVar = (nVar + 1) % nCount;
	}

	y += ITEM_H + 2;
}

void CFeatures_Menu::DrawSectionHeader(const wchar_t* szLabel, int x, int& y)
{
	G::Draw.String(EFonts::MENU_TAHOMA, x, y, clrSection, TXT_DEFAULT, szLabel);
	y += 4;
	G::Draw.Line(x, y + 12, x + COL_W - 10, y + 12, { 50, 50, 65, 255 });
	y += 16;
}
