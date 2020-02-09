#include <map>
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

void sys::mouse::cursor(str image)
{
    ::SetCursor(::LoadCursor(NULL,
        image == "hand" ? IDC_HAND :
        image == "noway" ? IDC_NO :
        image == "no way" ? IDC_NO :
        image == "editor" ? IDC_IBEAM :
        image == "vertical splitter" ? IDC_SIZEWE :
        image == "horizontal splitter" ? IDC_SIZENS : 
        IDC_ARROW
    ));
}

struct Clipboard
{
    Clipboard (const Clipboard&) = delete;
    Clipboard& operator = (const Clipboard&) = delete;
    Clipboard () { if (!::OpenClipboard(nullptr)) throw std::runtime_error("Can't open clipboard."); }
   ~Clipboard () { ::CloseClipboard(); }
};
struct ClipboardText
{
    ClipboardText (const ClipboardText&) = delete;
    ClipboardText& operator = (const ClipboardText&) = delete;
    explicit ClipboardText(HANDLE handle) : handle(handle) {
    text = static_cast<const WCHAR*>(::GlobalLock(handle));
    if (!text) throw std::runtime_error("Can't acquire lock on clipboard text."); }
   ~ClipboardText () { ::GlobalUnlock(handle); }
    HANDLE handle; const WCHAR * text;
};

str sys::clipboard::get::string ()
{
    try
    {
        Clipboard clipboard;
        HANDLE h = GetClipboardData(CF_UNICODETEXT);
        if (!h) throw std::runtime_error("Can't get clipboard text.");
        return utf8(std::wstring(ClipboardText(h).text));
    }
    catch(...){}
    return "";
}
void sys::clipboard::set (str s)
{
    if (s == "") return;
    try
    {
        Clipboard clipboard;
        std::wstring text = winstr(s);
        HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, (text.size()+1) * sizeof(WCHAR)); if (!handle) return;
	    LPWSTR buffer = (LPWSTR)::GlobalLock(handle); if (!buffer) return;
        memcpy(buffer, text.data(), text.size() * sizeof(WCHAR));
        buffer[text.size()] = 0;
        ::GlobalUnlock(handle);
        ::EmptyClipboard();
        ::SetClipboardData(CF_UNICODETEXT, buffer);
    }
    catch(...){}
}
pix::image<RGBA> sys::clipboard::get::image ()
{
    return pix::image<RGBA>();
}
void sys::clipboard::set (pix::frame<RGBA> image)
{
}


static std::map<str, int> int_settings;
static std::map<str, str> str_settings;

static void load_settings ()
{
}
static void save_settings ()
{
}

str sys::settings::load (str name, str default_value) {
    load_settings();
    auto [it, ins] = str_settings.try_emplace(name, default_value);
    return it->second;
}
int sys::settings::load (str name, int default_value) {
    load_settings();
    auto [it, ins] = int_settings.try_emplace(name, default_value);
    return it->second;
}
void sys::settings::save (str name, str value) {
    str_settings[name] = value;
    save_settings();
}
void sys::settings::save (str name, int value) {
    int_settings[name] = value;
    save_settings();
}

// https://blog.keyman.com/2008/06/robust-key-mess/

static bool alt = false;
static bool ctrl = false;
static bool shift = false;

static str wm_key (WPARAM wparam, LPARAM lparam, bool down)
{
    str s;
    if (0x30 <= wparam && wparam <= 0x39) s = '0' + (char)wparam-0x30; else
    if (0x41 <= wparam && wparam <= 0x5A) s = 'A' + (char)wparam-0x41; else
    if (0x70 <= wparam && wparam <= 0x7B) s = "F" + std::to_string(wparam-0x70+1); else
    switch(wparam){
    case VK_BACK    : s = "backspace"; break;
    case VK_TAB     : s = "tab"; break;
    case VK_RETURN  : s = "enter"; break;
    case VK_ESCAPE  : s = "escape"; break;
    case VK_INSERT  : s = "insert"; break;
    case VK_DELETE  : s = "delete"; break;
    case VK_SPACE   : s = "space"; break;
    case VK_SNAPSHOT: s = "print screen"; break;

    case VK_PRIOR   : s = "page up"; break;
    case VK_NEXT    : s = "page down"; break;
    case VK_END     : s = "end"; break;
    case VK_HOME    : s = "home"; break;
    case VK_LEFT    : s = "left"; break;
    case VK_RIGHT   : s = "right"; break;
    case VK_UP      : s = "up"; break;
    case VK_DOWN    : s = "down"; break;

    case VK_OEM_PLUS    : s = "+"; break;
    case VK_OEM_MINUS   : s = "-"; break;
    case VK_OEM_COMMA   : s = ","; break;
    case VK_OEM_PERIOD  : s = "."; break;

    case VK_MENU    : alt   = down; break;
    case VK_SHIFT   : shift = down; break;
    case VK_CONTROL : ctrl  = down; break;
    }
    if (s == "") return "";
    if (shift) s = "shift+" + s;
    if (alt  ) s =   "alt+" + s;
    if (ctrl ) s =  "ctrl+" + s;
    return s;
}
static str wm_char (WPARAM wparam, LPARAM lparam)
{
    if (alt) return "";
    if (ctrl) return "";
    if (wparam < 32) return "";
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
        case WM_CHAR            : sys::keyboard::on::input (wm_char(wparam, lparam)); break;

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

