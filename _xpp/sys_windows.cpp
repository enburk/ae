#include "sys.h"
#include "sys_windows.h"

#pragma comment(linker,"\"/manifestdependency:type='win32'  \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")

static HWND Hwnd;

void sys::window::update(XYXY r)
{
    RECT rect;
    rect.left   = r.l;
    rect.top    = r.t;
    rect.right  = r.r;
    rect.bottom = r.b;
    RedrawWindow (Hwnd, &rect, NULL, RDW_INVALIDATE);
    UpdateWindow (Hwnd);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_DESTROY: PostQuitMessage(0); return 0;
        case WM_SIZE:
        {
            if(wparam == SIZE_MINIMIZED) break;
            sys::window::image.resize(XY(LOWORD(lparam), HIWORD(lparam)));
            sys::window::on::resize();
            RedrawWindow (hwnd, NULL, NULL, RDW_INVALIDATE);
            UpdateWindow (hwnd);
            return 0;
        }
        case WM_PAINT:
        {
            int x = 0;
            int y = 0;
            int w = sys::window::image.size.x;
            int h = sys::window::image.size.y;

            BITMAPINFO bi;
            ZeroMemory(&bi,              sizeof(bi));
            bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth       = w;
            bi.bmiHeader.biHeight      = -h;
            bi.bmiHeader.biPlanes      = 1;
            bi.bmiHeader.biBitCount    = 32; 
            bi.bmiHeader.biCompression = BI_RGB;
            bi.bmiHeader.biSizeImage   = w*h*4;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            x = ps.rcPaint.left;
            y = ps.rcPaint.top;
            w = ps.rcPaint.right - x;
            h = ps.rcPaint.bottom - y;
            sys::window::on::redraw(XYWH(x, y, w, h));
            auto p = sys::window::image.data.data();
            SetDIBitsToDevice(hdc, x,y,w,h, 0,0,0,h, p, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow)
{
    SetProcessDpiAwarenessContext (DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = TEXT("DPP IDE class name");
    RegisterClass(&wc);

    RECT r; ::SystemParametersInfo ( SPI_GETWORKAREA, 0, &r, 0 );

    HWND hwnd = Hwnd = CreateWindowEx(
        WS_EX_APPWINDOW ,   // Optional styles
        wc.lpszClassName,   // Window class
        TEXT("AE IDE"),    // Window text
        WS_OVERLAPPEDWINDOW,
        r.left, r.top, r.right, r.bottom,
        NULL,               // Parent window    
        NULL,               // Menu
        hInstance,          // Instance handle
        NULL                // Additional application data
        );
    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, SW_MAXIMIZE);//nCmdShow);

    // Run the message loop.

    MSG msg = { }; while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

