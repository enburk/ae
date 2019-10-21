#include "sys.h"
#include "sys_windows.h"
using namespace pix;

#pragma comment(linker,"\"/manifestdependency:type='win32'  \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")

static HWND Hwnd;

void sys::window::update()
{
    if (false)
    if (sys::window::image.updates.size() > 0) {
        sys::window::image.updates.clear();
        RedrawWindow (Hwnd, NULL, NULL, RDW_INVALIDATE);
        UpdateWindow (Hwnd);
        return;
    }
    for (XYXY r : sys::window::image.updates)
    {
        RECT rect;
        rect.left   = r.l;
        rect.top    = r.t;
        rect.right  = r.r;
        rect.bottom = r.b;
        InvalidateRect(Hwnd, &rect, FALSE);
        UpdateWindow  (Hwnd);
    }
    sys::window::image.updates.clear();
}
void sys::window::timing()
{
    ::PostMessage(Hwnd, WM_COMMAND, 11111, 0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int LW = (short) LOWORD (wparam); int LL = (short) LOWORD (lparam); int LX = GET_X_LPARAM (lparam);
    int HW = (short) HIWORD (wparam); int HL = (short) HIWORD (lparam); int LY = GET_Y_LPARAM (lparam);

    switch (msg)
    {
        case WM_COMMAND         : if (wparam == 11111) sys::window::on::timing(); break;

        case WM_MOUSEMOVE       : sys::mouse::on::move  (XY(LX,LY)); break; // if( ! MouseSub ) ::SetCursor ( MouseImage ); break;
        case WM_MOUSEWHEEL      : sys::mouse::on::wheel (XY(LX,LY), HW); break;
        case WM_LBUTTONDOWN     : sys::mouse::on::press (XY(LX,LY), 'L', true ); SetCapture(Hwnd); break;
        case WM_LBUTTONUP       : sys::mouse::on::press (XY(LX,LY), 'L', false); ReleaseCapture(); break;
        case WM_MBUTTONDOWN     : sys::mouse::on::press (XY(LX,LY), 'M', true ); break;
        case WM_MBUTTONUP       : sys::mouse::on::press (XY(LX,LY), 'M', false); break;
        case WM_RBUTTONDOWN     : sys::mouse::on::press (XY(LX,LY), 'R', true ); break;
        case WM_RBUTTONUP       : sys::mouse::on::press (XY(LX,LY), 'R', false); break;
        case WM_CAPTURECHANGED  : sys::mouse::on::leave (); break;
        case WM_MOUSELEAVE      : sys::mouse::on::leave (); break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            int x = ps.rcPaint.left;
            int y = ps.rcPaint.top;
            int w = ps.rcPaint.right - x;
            int h = ps.rcPaint.bottom - y;

            int W = sys::window::image.size.x;
            int H = sys::window::image.size.y;

            BITMAPINFO bi;
            ZeroMemory(&bi,              sizeof(bi));
            bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth       = W;
            bi.bmiHeader.biHeight      = -h;
            bi.bmiHeader.biPlanes      = 1;
            bi.bmiHeader.biBitCount    = 32; 
            bi.bmiHeader.biCompression = BI_RGB;
            bi.bmiHeader.biSizeImage   = W*h*4;

            auto p = &sys::window::image(0,y);
            SetDIBitsToDevice(hdc, x,y,w,h, x,0,0,h, p, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_SIZE:
        {
            static bool minimized = false;
            if(wparam == SIZE_MINIMIZED) { if (!minimized) sys::window::on::pause (); minimized = true; break; }
            if(wparam != SIZE_MINIMIZED) { if (!minimized) sys::window::on::resume(); minimized = false; }
            sys::window::image.resize(XY(LOWORD(lparam), HIWORD(lparam)));
            sys::window::on::resize();
            break;
        }
        case WM_CREATE : sys::window::on::start (); return 0;
        case WM_DESTROY: sys::window::on::finish(); PostQuitMessage(0); return 0;

        default: return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    sys::window::update();

    return 0;
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

    sys::screen::size.x = r.right - r.left;
    sys::screen::size.y = r.bottom - r.top;

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

