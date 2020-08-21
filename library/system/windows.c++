#pragma comment(linker,"\"/manifestdependency:type='win32'  \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")

int main (int argc, char *argv[])
{
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    RECT r; ::SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    ae::system::screen::size::x = r.right - r.left;
    ae::system::screen::size::y = r.bottom - r.top;

    ae::main_();

    MSG msg = {}; //...W crucial for Unicode
    while (::GetMessageW(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow)
{
    SetProcessDpiAwarenessContext (DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    RECT r; ::SystemParametersInfo ( SPI_GETWORKAREA, 0, &r, 0 );

    sys::screen::size.x = r.right - r.left;
    sys::screen::size.y = r.bottom - r.top;

    sys::app_instance app;

    WNDCLASS wc = { };
    wc.lpszClassName = TEXT("DPP IDE class name");
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = NULL; // If the class cursor is not NULL,
    // the system restores the class cursor each time the mouse is moved.
    RegisterClass(&wc);

    HWND hwnd = Hwnd = CreateWindowEx(
        WS_EX_APPWINDOW ,   // Optional styles
        wc.lpszClassName,   // Window class
        app.app->title.c_str(), // Window text
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




