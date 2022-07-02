#pragma once
#include "windows_aux.h"
#pragma comment(linker,"\"/manifestdependency:type='win32'  \\
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int LW = (short)LOWORD(wparam); int LL = (short)LOWORD(lparam); int LX = GET_X_LPARAM(lparam);
    int HW = (short)HIWORD(wparam); int HL = (short)HIWORD(lparam); int LY = GET_Y_LPARAM(lparam);

    switch (msg) {
    // case WM_COMMAND         : if (wparam == 11111) sys::window::on::timing(); break;
    //
    // case WM_MOUSEMOVE       : sys::mouse::on::move  (XY(LX,LY)); break;
    // case WM_MOUSEWHEEL      : sys::mouse::on::wheel (XY(LX,LY), HW); break;
    // case WM_LBUTTONDOWN     : sys::mouse::on::press (XY(LX,LY), 'L', true ); SetCapture(Hwnd); break;
    // case WM_LBUTTONUP       : sys::mouse::on::press (XY(LX,LY), 'L', false); ReleaseCapture(); break;
    // case WM_MBUTTONDOWN     : sys::mouse::on::press (XY(LX,LY), 'M', true ); break;
    // case WM_MBUTTONUP       : sys::mouse::on::press (XY(LX,LY), 'M', false); break;
    // case WM_RBUTTONDOWN     : sys::mouse::on::press (XY(LX,LY), 'R', true ); break;
    // case WM_RBUTTONUP       : sys::mouse::on::press (XY(LX,LY), 'R', false); break;
    // case WM_CAPTURECHANGED  : sys::mouse::on::leave (); break;
    // case WM_MOUSELEAVE      : sys::mouse::on::leave (); break;
    //
    // case WM_SETFOCUS        : sys::keyboard::on::focus (true); break;
    // case WM_KILLFOCUS       : sys::keyboard::on::focus (false); break;
    // case WM_SYSKEYDOWN      : sys::keyboard::on::press (wm_key(wparam, lparam, true ), true ); break;
    // case WM_SYSKEYUP        : sys::keyboard::on::press (wm_key(wparam, lparam, false), false); break;
    // case WM_KEYDOWN         : sys::keyboard::on::press (wm_key(wparam, lparam, true ), true ); break;
    // case WM_KEYUP           : sys::keyboard::on::press (wm_key(wparam, lparam, false), false); break;
    // case WM_CHAR            : sys::keyboard::on::input (wm_char(wparam, lparam)); break;
    //
    // case WM_PAINT:
    // {
    //     PAINTSTRUCT ps;
    //     HDC hdc = BeginPaint(hwnd, &ps);
    //     int x = ps.rcPaint.left;
    //     int y = ps.rcPaint.top;
    //     int w = ps.rcPaint.right - x;
    //     int h = ps.rcPaint.bottom - y;
    //
    //     int W = sys::window::image.size.x;
    //     int H = sys::window::image.size.y;
    //
    //     BITMAPINFO bi;
    //     ZeroMemory(&bi,              sizeof(bi));
    //     bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    //     bi.bmiHeader.biWidth       = W;
    //     bi.bmiHeader.biHeight      = -h;
    //     bi.bmiHeader.biPlanes      = 1;
    //     bi.bmiHeader.biBitCount    = 32;
    //     bi.bmiHeader.biCompression = BI_RGB;
    //     bi.bmiHeader.biSizeImage   = W*h*4;
    //
    //     auto p = &sys::window::image(0,y);
    //     SetDIBitsToDevice(hdc, x,y,w,h, x,0,0,h, p, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    //     EndPaint(hwnd, &ps);
    //     return 0;
    // }
    // case WM_SIZE:
    // {
    //     static bool minimized = false;
    //     if(wparam == SIZE_MINIMIZED) { if (!minimized) sys::window::on::pause (); minimized = true; break; }
    //     if(wparam != SIZE_MINIMIZED) { if (!minimized) sys::window::on::resume(); minimized = false; }
    //     sys::window::image.resize(XY(LOWORD(lparam), HIWORD(lparam)));
    //     sys::window::on::resize();
    //     break;
    // }
    // case WM_CREATE : sys::window::on::start (); return 0;
     case WM_DESTROY: //sys::window::on::finish();
      PostQuitMessage(0); return 0;

    default: return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    // if (sys::window::image.size.x > 0 // not yet on first WM_SETFOCUS
    // &&  sys::window::image.size.y > 0)
    //     sys::window::update();
    //
    // if (sys::window::image.size.x > 0 // not yet on first WM_SETFOCUS
    // &&  sys::window::image.size.y > 0)
    //     sys::window::update();

    return 0;
}

void ae::gui::window::create ()
{
    WNDCLASS wc = {};
    wc.lpszClassName = L"AE window class name";
    wc.hInstance = ::GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProc;
    ::RegisterClass(&wc);

    HWND handle;
    auto title = L"AE window";
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int w = CW_USEDEFAULT;
    int h = CW_USEDEFAULT;

    handle = ::CreateWindowEx(
        WS_EX_APPWINDOW, // optional styles
        wc.lpszClassName, // window class
        title,
        WS_OVERLAPPEDWINDOW,
        x, y, w, h,
        NULL, // parent window
        NULL, // menu
        ::GetModuleHandle(NULL),
        this // user data
        );

    if (!handle) throw std::system_error(
        ::GetLastError(), std::system_category(),
        "Failed to create window.");

    ::ShowWindow(handle, SW_MAXIMIZE);
}

int main (int argc, char *argv[])
{
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    RECT r; ::SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    ae::system::screen.size.x = r.right - r.left;
    ae::system::screen.size.y = r.bottom - r.top;

    ae::main_();

    ae::system::window.create();

    MSG msg = {};
    while (::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}





