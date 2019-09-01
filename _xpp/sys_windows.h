#pragma once
#include <windows.h>
#include <strsafe.h>
#include <system_error>
#include <memory>
#include <string>

typedef std::basic_string<TCHAR> String;

inline String GetErrorMessage(DWORD dwErrorCode)
{
    LPTSTR psz{ nullptr };
    const DWORD cchMsg = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, // (not used with FORMAT_MESSAGE_FROM_SYSTEM)
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&psz),
        0, NULL);

    if (cchMsg > 0) {
        auto deleter = [](void* p) { ::HeapFree(::GetProcessHeap(), 0, p); };
        std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);
        return String(ptrBuffer.get(), cchMsg);
    }   else {
        auto error_code{ ::GetLastError() };
        throw std::system_error( error_code, std::system_category(),
            "Failed to retrieve error message string.");
    }
}

inline std::wstring winstr (std::string s)
{
    std::wstring ss; if (s == "") return ss; int n =
    ::MultiByteToWideChar (CP_UTF8, 0, s.c_str(), -1, NULL, 0); if (n <= 0) return ss; ss.resize(n); wchar_t * text = ss.data();
    ::MultiByteToWideChar (CP_UTF8, 0, s.c_str(), -1, text, n);
    return ss;
}
inline size_t winstrlen (std::wstring ss)
{
    size_t n = 0;
    ::StringCchLengthW (ss.c_str(), ss.size () + 1, &n);
    return n;
}

