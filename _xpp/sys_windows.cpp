#include "sys.h"
#include "sys_windows.h"
using namespace pix;

#pragma comment(linker,"\"/manifestdependency:type='win32'  \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")

HWND Hwnd;

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

static str wm_key (WPARAM wparam, LPARAM lparam, bool down)
{
    int a = lparam & 0x40000000 ? 1 : 0;
    bool alt = false;
    bool ctrl = false;
    bool shift = false;
    str c;

    if (0x30 <= wparam && wparam <= 0x39) c = '9' + (char)wparam; else
    if (0x41 <= wparam && wparam <= 0x5A) c = 'A' + (char)wparam; else
    if (0x70 <= wparam && wparam <= 0x7B) c = "F" + std::to_string(wparam-0x70+1); else
    switch(wparam){
    case VK_BACK: c = "backspace"; break;
    case VK_TAB: c = "tab"; break;
    case VK_RETURN: c = "enter"; break;
    case VK_ESCAPE: c = "escape"; break;
    case VK_INSERT: c = "insert"; break;
    case VK_DELETE: c = "delete"; break;
    case VK_SPACE: c = "space"; break;
    case VK_SNAPSHOT: c = "print screen"; break;

    case VK_PRIOR: c = "page up"; break;
    case VK_NEXT: c = "page down"; break;
    case VK_END: c = "end"; break;
    case VK_HOME: c = "home"; break;
    case VK_LEFT: c = "left"; break;
    case VK_RIGHT: c = "right"; break;
    case VK_UP: c = "up"; break;
    case VK_DOWN: c = "down"; break;

    case VK_MENU: alt = down; break;
    case VK_SHIFT: shift = down; break;
    case VK_CONTROL: ctrl = down; break;
    }
    if (c == "") return "";
    if (shift) c = "shift+" + c;
    if (alt  ) c =   "alt+" + c;
    if (ctrl ) c =  "ctrl+" + c;
    return c;
}
static str wm_syskey (WPARAM wparam, LPARAM lparam)
{
    return "";
}
static str wm_char (WPARAM wparam)
{
    if (wparam < 32) return "";
    if (wparam == 128) return ""; // ctrl+backspace
    std::wstring s; s += (wchar_t)wparam; return utf8(s);
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

        case WM_SETFOCUS        : sys::keyboard::on::focus (true); break;
        case WM_KILLFOCUS       : sys::keyboard::on::focus (false); break;
        case WM_KEYDOWN         : sys::keyboard::on::press (wm_key(wparam, lparam, true ), true ); break;
        case WM_KEYUP           : sys::keyboard::on::press (wm_key(wparam, lparam, false), false); break;
        case WM_CHAR            : sys::keyboard::on::input (wm_char(wparam)); break;

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

    if (sys::window::image.size.x > 0 // not yet on first WM_SETFOCUS
    &&  sys::window::image.size.y > 0)
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
        TEXT("AE IDE"),     // Window text
        WS_OVERLAPPEDWINDOW,
        r.left, r.top, r.right, r.bottom,
        NULL,               // Parent window    
        NULL,               // Menu
        hInstance,          // Instance handle
        NULL                // Additional application data
        );
    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, SW_MAXIMIZE);//nCmdShow);

    // the message loop   ..........W crucial for Unicode
    MSG msg = { }; while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

