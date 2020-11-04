#pragma once
#include "sys_ui.h"
#include "windows_aux.h"
#include <gl\gl.h>
#include <gl\glu.h>
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32'  \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")

aux::str sys::dialog (str title, str text, sys::choice choice, void* handle)
{
    UINT type = MB_OK;
    switch (choice) {
    case sys::choice::ok:                  type = MB_OK                ; break;
    case sys::choice::ok_cancel:           type = MB_OKCANCEL          ; break;
    case sys::choice::yes_no:              type = MB_YESNO             ; break;
    case sys::choice::yes_no_cancel:       type = MB_YESNOCANCEL       ; break;
    case sys::choice::abort_retry_ignore:  type = MB_ABORTRETRYIGNORE  ; break;
    case sys::choice::cancel_try_continue: type = MB_CANCELTRYCONTINUE ; break;
    case sys::choice::retry_cancel:        type = MB_RETRYCANCEL       ; break;
    }

    type |= MB_APPLMODAL | MB_ICONWARNING;

    auto rc = ::MessageBox((HWND)(handle), text.c_str(), title.c_str(), type);

    switch (rc) {
    case IDOK       : return "ok";
    case IDCANCEL   : return "cancel";
    case IDABORT    : return "abort";
    case IDRETRY    : return "retry";
    case IDIGNORE   : return "ignore";
    case IDYES      : return "yes";
    case IDNO       : return "no";
    case IDTRYAGAIN : return "try again";
    case IDCONTINUE : return "continue";
    default: return "";
    }
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

// https://blog.keyman.com/2008/06/robust-key-mess/

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

    case VK_MENU    : sys::keyboard::alt   = down; break;
    case VK_SHIFT   : sys::keyboard::shift = down; break;
    case VK_CONTROL : sys::keyboard::ctrl  = down; break;
    }
    if (s == "") return "";
    if (sys::keyboard::shift) s = "shift+" + s;
    if (sys::keyboard::alt  ) s =   "alt+" + s;
    if (sys::keyboard::ctrl ) s =  "ctrl+" + s;
    return s;
}
static str wm_char (WPARAM wparam, LPARAM lparam)
{
    if (wparam < 32) return "";
    if (sys::keyboard::alt) return "";
    if (sys::keyboard::ctrl) return "";
    std::wstring s; s += (wchar_t)wparam; return unwinstr(s);
}

LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    sys::window* win = (sys::window*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    int LW=(short)LOWORD(wparam); int LL=(short)LOWORD(lparam); int LX=GET_X_LPARAM(lparam);
    int HW=(short)HIWORD(wparam); int HL=(short)HIWORD(lparam); int LY=GET_Y_LPARAM(lparam);

    switch (msg) {
    case WM_COMMAND: if (wparam == 11111) win->on_timing(); break;
    
    case WM_MOUSEMOVE     : win->mouse_on_move  (XY(LX,LY)); break;
    case WM_MOUSEWHEEL    : win->mouse_on_wheel (XY(LX,LY), HW); break;
    case WM_LBUTTONDOWN   : win->mouse_on_press (XY(LX,LY), 'L', true ); SetCapture(hwnd); break;
    case WM_LBUTTONUP     : win->mouse_on_press (XY(LX,LY), 'L', false); ReleaseCapture(); break;
    case WM_MBUTTONDOWN   : win->mouse_on_press (XY(LX,LY), 'M', true ); break;
    case WM_MBUTTONUP     : win->mouse_on_press (XY(LX,LY), 'M', false); break;
    case WM_RBUTTONDOWN   : win->mouse_on_press (XY(LX,LY), 'R', true ); break;
    case WM_RBUTTONUP     : win->mouse_on_press (XY(LX,LY), 'R', false); break;
    case WM_CAPTURECHANGED: win->mouse_on_leave (); break;
    case WM_MOUSELEAVE    : win->mouse_on_leave (); break;
    
    case WM_SETFOCUS      : win->keyboard_on_focus (true); break;
    case WM_KILLFOCUS     : win->keyboard_on_focus (false); break;
    case WM_SYSKEYDOWN    : win->keyboard_on_press (wm_key(wparam, lparam, true ), true ); break;
    case WM_SYSKEYUP      : win->keyboard_on_press (wm_key(wparam, lparam, false), false); break;
    case WM_KEYDOWN       : win->keyboard_on_press (wm_key(wparam, lparam, true ), true ); break;
    case WM_KEYUP         : win->keyboard_on_press (wm_key(wparam, lparam, false), false); break;
    case WM_CHAR          : win->keyboard_on_input (wm_char(wparam, lparam)); break;
    
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        int x = ps.rcPaint.left;
        int y = ps.rcPaint.top;
        int w = ps.rcPaint.right - x;
        int h = ps.rcPaint.bottom - y;
    
        int W = win->image.size.x;
        int H = win->image.size.y;
    
        BITMAPINFO bi;
        ZeroMemory(&bi,              sizeof(bi));
        bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth       = W;
        bi.bmiHeader.biHeight      = -h;
        bi.bmiHeader.biPlanes      = 1;
        bi.bmiHeader.biBitCount    = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        bi.bmiHeader.biSizeImage   = W*h*4;
    
        auto p = &win->image(0,y);
        SetDIBitsToDevice(hdc, x,y,w,h, x,0,0,h, p, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE:
    {
        static bool minimized = false;
        if(wparam == SIZE_MINIMIZED) { if (!minimized) win->on_pause (); minimized = true; break; }
        if(wparam != SIZE_MINIMIZED) { if (!minimized) win->on_resume(); minimized = false; }
        win->on_resize(XY(LOWORD(lparam), HIWORD(lparam)));
        break;
    }
    case WM_CREATE : 
        win = (sys::window*)((CREATESTRUCT*)lparam)->lpCreateParams;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(win));
        win->native_handle1 = hwnd;
        win->on_start();
        return 0;

    case WM_DESTROY:
        win->on_finish();
        PostQuitMessage(0);
        return 0;

    default: return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    if (win->image.size.x > 0 // not yet on first WM_SETFOCUS
    &&  win->image.size.y > 0)
        win->update();
    
    if (win->image.size.x > 0 // not yet on first WM_SETFOCUS
    &&  win->image.size.y > 0)
        win->update();

    return 0;
}

LRESULT CALLBACK WindowProcGL (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto rc = WindowProc(hwnd, msg, wparam, lparam);

    sys::window* w = (sys::window*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    HGLRC handle = w ? (HGLRC)(w->native_handle2) : NULL;

    switch (msg) {
    case WM_CREATE : 
    {
        PIXELFORMATDESCRIPTOR pfd = 
        { 
            sizeof(PIXELFORMATDESCRIPTOR),
            1,                      // version number 
            PFD_DRAW_TO_WINDOW |    // support window 
            PFD_SUPPORT_OPENGL |    // support OpenGL 
            PFD_DOUBLEBUFFER,       // double buffered 
            PFD_TYPE_RGBA,          // RGBA type 
            24,                     // 24-bit color depth 
            0, 0, 0, 0, 0, 0,       // color bits ignored 
            0,                      // no alpha buffer 
            0,                      // shift bit ignored 
            0,                      // no accumulation buffer 
            0, 0, 0, 0,             // accum bits ignored 
            32,                     // 32-bit z-buffer     
            0,                      // no stencil buffer 
            0,                      // no auxiliary buffer 
            PFD_MAIN_PLANE,         // main layer 
            0,                      // reserved 
            0, 0, 0                 // layer masks ignored 
        };

        HDC hdc = ::GetDC(hwnd);

        auto pixel_format  = 
        ::ChoosePixelFormat   (hdc, &pfd);
        ::DescribePixelFormat (hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd );
        ::SetPixelFormat      (hdc, pixel_format, &pfd);

        handle = 
        ::wglCreateContext    (hdc);
        ::wglMakeCurrent      (hdc, handle);
        ::ReleaseDC     (hwnd, hdc);

        w->native_handle2 = handle;
        break;
    }
    case WM_DESTROY:
    {
        ::wglMakeCurrent   (NULL, NULL);
        ::wglDeleteContext (handle);
        break;
    }
    case WM_SIZE:
    {
        HDC hdc = ::GetDC(hwnd);
        ::wglMakeCurrent (hdc, handle);
        ::ReleaseDC(hwnd, hdc);
        ::glViewport (0, 0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        w->on_resize(XY(LOWORD(lparam), HIWORD(lparam)));
        break;
    }
    case WM_PAINT:
    {
        HDC hdc = ::GetDC(hwnd);
        ::wglMakeCurrent (hdc, handle);

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ::glFinish();

        ::SwapBuffers(hdc);
        ::ReleaseDC(hwnd, hdc);
    }
    }

    return rc;
}

void sys::window::create (str title)
{
    WNDCLASS wc = {};
    wc.hInstance = ::GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("AE window class name");
    wc.lpfnWndProc = opengl ? WindowProcGL : WindowProc;
    ::RegisterClass(&wc);

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int w = CW_USEDEFAULT;
    int h = CW_USEDEFAULT;

    HWND handle = ::CreateWindowEx(
        WS_EX_APPWINDOW, // optional styles
        wc.lpszClassName, // window class
        title.c_str(),
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
void sys::window::update()
{
    auto hwnd = (HWND)(native_handle1);
    for (XYXY r : image.updates)
    {
        RECT rect;
        rect.left   = r.l;
        rect.top    = r.t;
        rect.right  = r.r;
        rect.bottom = r.b;
        InvalidateRect(hwnd, &rect, FALSE);
        UpdateWindow  (hwnd);
    }
    image.updates.clear();
}
void sys::window::timing()
{
    auto hwnd = (HWND)(native_handle1);
    ::PostMessage(hwnd, WM_COMMAND, 11111, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow)
{
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    RECT r; ::SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    sys::screen::size.x = r.right - r.left;
    sys::screen::size.y = r.bottom - r.top;

    sys::app_instance app;

    MSG msg = {}; //...W for UNICODE
    while (::GetMessageW(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}





