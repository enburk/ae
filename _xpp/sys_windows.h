#pragma once
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <system_error>
#include <memory>
#include <string>

typedef std::basic_string<TCHAR> String;

inline String GetErrorMessage(DWORD dwErrorCode)
{
    LPTSTR psz = nullptr;
    const DWORD n = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&psz), 0, NULL);

    if (n > 0) {
        auto deleter = [](void* p) { ::HeapFree(::GetProcessHeap(), 0, p); };
        std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);
        return String(ptrBuffer.get(), n);
    }
    else throw std::system_error(::GetLastError(), std::system_category(),
        "Failed to retrieve error message string.");
}

inline std::string utf8 (std::wstring s)
{
    std::string ss; if (s.empty()) return ss; int n =
    ::WideCharToMultiByte(CP_UTF8, 0, &s[0], (int)s.size(), NULL, 0, NULL, NULL); if (n <= 0) return ss; ss.resize(n); char * text = ss.data();
    ::WideCharToMultiByte(CP_UTF8, 0, &s[0], (int)s.size(), text, n, NULL, NULL);
    return ss;
}
inline std::wstring winstr (std::string s)
{
    std::wstring ss; if (s == "") return ss; int n =
    ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0); if (n <= 0) return ss; ss.resize(n); wchar_t * text = ss.data();
    ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, text, n);
    return ss;
}
inline size_t winstrlen (std::wstring ss)
{
    size_t n = 0; if(S_OK != ::StringCchLengthW (ss.c_str(), ss.size () + 1, &n)) return 0;
    return n;
}


