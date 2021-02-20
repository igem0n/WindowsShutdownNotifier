#include <Windows.h>
#include "devicemanager.h"

HWND hWnd;
DeviceManager manager;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch(message)
    {
    case WM_QUERYENDSESSION:
        return !manager.hasNewDevices();
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_CREATE:
        ShutdownBlockReasonCreate(hWnd, L"U has unplugged devices!");
        manager.updateCurrentDevices();
        if(!manager.loadDefaultDevices())
        {
            manager.setCurrentDevicesAsDefault();
        }
        break;
    case WM_DESTROY:
        ShutdownBlockReasonDestroy(hWnd);
        PostQuitMessage(0);
        break;
    case WM_DEVICECHANGE:
        manager.updateCurrentDevices();
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TestClass";
    wcex.hIconSm = NULL;
    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    RECT sz = {0, 0, 512, 512};
    AdjustWindowRect(&sz, WS_OVERLAPPEDWINDOW, TRUE);
    hWnd = CreateWindow(L"TestClass", L"Test Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, sz.right - sz.left, sz.bottom - sz.top,
        NULL, NULL, hInstance, NULL);
    if(!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
    if(dwCtrlType == CTRL_C_EVENT ||
        dwCtrlType == CTRL_BREAK_EVENT ||
        dwCtrlType == CTRL_CLOSE_EVENT)
    {
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        return TRUE;
    }
    return FALSE;
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    CoInitialize(0);
    MyRegisterClass(hInstance);
    if(!InitInstance(hInstance, SW_HIDE))
    {
        return FALSE;
    }

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    CoUninitialize();
    return 0;
}
