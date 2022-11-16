#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <ctime>
#include<commctrl.h>
#include <windowsX.h>
#include "resource1.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HWND hTime, hProgress, hRadio1;
TCHAR str_time[100];
SYSTEMTIME st;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

DWORD WINAPI ThreadControlBar(LPVOID lp)
{
	DWORD pr = GetPriorityClass(GetCurrentProcess());
	SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

	HWND hProg = (HWND)lp;
	while (TRUE) {
		SendMessage(hProg, PBM_SETPOS, rand() % 200 + 1, 0);
		Sleep(250);
	}
	SetPriorityClass(GetCurrentProcess(), pr);
	return 0;
}

DWORD WINAPI ThreadTime(LPVOID lp) {
	DWORD pr = GetPriorityClass(GetCurrentProcess());
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	HWND hProgress = (HWND)lp;
	struct Time {
		int Minutes = 2;
		int Seconds = 30;
		int Hours = 1;
	} Time;

	while (TRUE) {
		wsprintf(str_time, TEXT("%d:%d"), Time.Hours, Time.Minutes, Time.Seconds);
		SetWindowText(hProgress, str_time);
		if (Time.Hours <= 0) {
			Time.Hours = 1;
			if (Time.Seconds <= 0) {
				Time.Seconds --;
				if (Time.Minutes <= 0) {
					Time.Seconds--;
					ExitThread;
					MessageBox(hTime, TEXT("Time is out ->("), TEXT("Finish"), MB_OK | MB_ICONINFORMATION);
					return 0;
				}
				else {
					Time.Hours--;
					Time.Seconds--;
					Time.Minutes--;
				}
			}
			else {
				Time.Seconds--;
			}
			Sleep(1000);
		}
	}
	SetPriorityClass(GetCurrentProcess(), pr);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG: {
		hTime = GetDlgItem(hWnd, IDC_TIME);
		hRadio1 = GetDlgItem(hWnd, IDC_RADIO1);

		hProgress = GetDlgItem(hWnd, IDC_PROGRESS1);
		SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 200));
		SendMessage(hProgress, PBM_SETSTEP, 1, 0);
		SendMessage(hProgress, PBM_SETPOS, 0, 0);
		SendMessage(hProgress, PBM_SETBARCOLOR, 0, LPARAM(RGB(0, 214, 120)));

		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
					  break;

	case WM_COMMAND: {
		if (IDC_BUTTON1 == LOWORD(wParam)) {
			HANDLE hThread = CreateThread(NULL, 0, ThreadControlBar, hProgress, 0, NULL);
			CloseHandle(hThread);
		}
		else if (IDC_BUTTON2 == LOWORD(wParam)) {
			HANDLE hThread = CreateThread(NULL, 0, ThreadTime, hTime, 0, NULL);
			CloseHandle(hThread);
		}
	}
				   break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}