#pragma once

#include <Windows.h>

using namespace std;
HDC desktop = GetDC(NULL);
CURSORINFO curInf;
int sx = GetSystemMetrics(SM_CXSCREEN);
int sy = GetSystemMetrics(SM_CYSCREEN);
HWND upWind = GetForegroundWindow();
HDC upHdc = GetDC(upWind);

vector<HICON> cur;

//CURSOR CLASS//
void DublicateCursor(int CursorPosX, int CursorPosY, int Duration = 0) {
	GetCursorInfo(&curInf);
	curInf.cbSize = sizeof(curInf);
	DrawIcon(desktop, CursorPosX, CursorPosY, curInf.hCursor);
	Sleep(Duration);
}

void DrawIconOnCursor(LPCWSTR IDI__, int Duration = 0) {
	GetCursorInfo(&curInf);
	curInf.cbSize = sizeof(curInf);
	DrawIcon(desktop, curInf.ptScreenPos.x, curInf.ptScreenPos.y, LoadIconW(NULL, IDI__));
	Sleep(Duration);
}

void DrawIconRandPos(LPCWSTR IDI__, int Duration = 0) {
	GetCursorInfo(&curInf);
	curInf.cbSize = sizeof(curInf);
	DrawIcon(desktop, rand() % sx, rand() % sy, LoadIconW(NULL, IDI__));
	Sleep(Duration);
}

void DrawIconOnCursor(HICON icon, int Duration = 0) {
	GetCursorInfo(&curInf);
	curInf.cbSize = sizeof(curInf);
	DrawIcon(desktop, curInf.ptScreenPos.x, curInf.ptScreenPos.y, icon);
	Sleep(Duration);
}

void DrawIconRandPos(HICON icon, int Duration = 0) {
	GetCursorInfo(&curInf);
	curInf.cbSize = sizeof(curInf);
	DrawIcon(desktop, rand() % sx, rand() % sy, icon);
	Sleep(Duration);
}
//CURSOR CLASS END//

//GDI CLASS//
void CreateGDI(int x, int y, int x2, int y2, DWORD EffectCode, int Duration = 0) {
	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(SM_CXSCREEN);
	sy = GetSystemMetrics(SM_CYSCREEN);
	BitBlt(desktop, x, y, sx, sy, upHdc, x2, y2, EffectCode);
	Sleep(Duration);
}

void JustGDI(int Duration = 0) {
	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(SM_CXSCREEN);
	sy = GetSystemMetrics(SM_CYSCREEN);
	BitBlt(desktop, 0, 0, sx, sy, upHdc, 0, 0, 0x555555);
	Sleep(Duration);
}

void MadGDI(int Duration = 0) {
	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(SM_CXSCREEN);
	sy = GetSystemMetrics(SM_CYSCREEN);

	Sleep(Duration);
	BitBlt(desktop, rand() % sx, rand() % sy, rand() % sx, rand() % sy, upHdc, rand() % sx, rand() % sy, 0xc909090);
	Sleep(Duration);
	BitBlt(desktop, rand() % sx, rand() % sy, rand() % sx, rand() % sy, upHdc, rand() % sx, rand() % sy, 0xc555555);
}

void TunnelGDI(int Duration = 0) {
	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(0);
	sy = GetSystemMetrics(1);
	StretchBlt(desktop, 50, 50, sx - 100, sy - 100, desktop, 0, 0, sx, sy, SRCCOPY);
	Sleep(Duration);
}

void TunnelRandGDI(int Duration = 0) {
	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(0);
	sy = GetSystemMetrics(1);
	StretchBlt(desktop, rand() % sx, rand() % sy, rand() % sx, rand() % sy, desktop, 0, 0, sx, sy, SRCCOPY);
	Sleep(Duration);
}

void ScratchGDI(int Duration = 0) {
	static int x = 0;
	int step_x = 10;

	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(0);
	sy = GetSystemMetrics(1);

	if (x >= sx) { x = 0; }

	StretchBlt(desktop, x, rand() % 120 - 60, step_x, sy, desktop, x, 0, step_x, sy, SRCCOPY);

	x += step_x;

	Sleep(Duration);
}

void GlitchGDI(int Duration = 0) {
	int meltX = 1;
	int meltX2 = 1;
	int meltY = 1;
	int meltY2 = 1;
	meltX = +1 + rand() % 500;
	meltX2 = -1 + rand() % 500;
	meltY = +1 + rand() % 500;
	meltY2 = -1 + rand() % 500;

	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(SM_CXSCREEN);
	sy = GetSystemMetrics(SM_CYSCREEN);
	CreateGDI(rand() % meltX, rand() % meltY, rand() % meltX2, rand() % meltY2, 0xc777777, Duration);
	CreateGDI(rand() % meltX, rand() % meltY, rand() % meltX2, rand() % meltY2, 0x555555, Duration);

	meltX = 1;
	meltX2 = 1;
	meltY = 1;
	meltY2 = 1;
}

void InsertColorsGDI(int Duration = 0) {
	desktop = GetDC(NULL);
	upWind = GetForegroundWindow();
	upHdc = GetDC(upWind);
	sx = GetSystemMetrics(SM_CXSCREEN);
	sy = GetSystemMetrics(SM_CYSCREEN);
	BitBlt(desktop, 0, 0, sx, sy, upHdc, 0, 0, 0x555555);
	Sleep(Duration);
}
//GDI CLASS END//

HICON RandIcon() {
	return cur[rand() % cur.size()];
}