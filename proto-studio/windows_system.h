#include <map>
#include "sys_aux.h"
#include "windows_aux.h"

sys::process::process (std::filesystem::path path, str args, options opt)
{
    HANDLE out = 0; if (opt.out != std::filesystem::path{})
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;       

        out = CreateFileW(
            winstr(opt.out.string()).c_str(),
            FILE_APPEND_DATA,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            &sa,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi; 
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO); 

    if (opt.out != std::filesystem::path{}) {
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput = NULL;
        si.hStdError = out;
        si.hStdOutput = out;
    }

    DWORD flags = 0; if (opt.hidden) flags = CREATE_NO_WINDOW;

    auto ws = winstr(" " + args);
    array<WCHAR> buffer; buffer.resize((int)(ws.size()));
    std::copy(ws.begin(), ws.end(), buffer.begin());

    BOOL rc = CreateProcessW(
        winstr(path.string()).c_str(),
        buffer.data.data(),
        NULL, NULL, TRUE, flags,
        NULL, NULL, &si, &pi);

    if (!rc) throw std::runtime_error(
        GetErrorMessage(::GetLastError()));

    handle = (size_t)(pi.hProcess);
    if (out != 0) CloseHandle(out);
    ::CloseHandle(pi.hThread);

    if (opt.ms_wait_for_input_idle > 0) ::WaitForInputIdle(pi.hProcess, 
        opt.ms_wait_for_input_idle);
}
bool sys::process::wait (int ms)
{
    if (handle == 0) return false;
    DWORD time = ms == max<int>() ? INFINITE : ms;
    auto rc = ::WaitForSingleObject((HANDLE)(handle), time);
    return rc == WAIT_OBJECT_0 || rc == WAIT_ABANDONED_0;

}
sys::process::~process ()
{
    if (handle != 0) ::CloseHandle((HANDLE)(handle));
}

///////////////////////////////////////////////////////////////////////////////

void sys::directory_watcher::watch ()
{
    thread = std::thread([this]()
    {
        std::array<std::byte, 64*1024> buffer; DWORD bytes_written = 0;

        auto handle = ::CreateFile(dir.string().c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if (handle == INVALID_HANDLE_VALUE) { error(GetErrorMessage(::GetLastError())); return; }

        while (!stop)
        {
            auto return_code = ::ReadDirectoryChangesW
            (
                handle, buffer.data(),
                (DWORD)(buffer.size()),
                TRUE, // recursive
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                &bytes_written,
                NULL, // synchronous calls
                NULL // completion routine
            );

            if (return_code == 0) { error(GetErrorMessage(::GetLastError())); return; }
            if (bytes_written == 0) { error("buffer too big or too small"); return; }

            size_t buffer_offset = 0;

            while (!stop)
            {
                if (buffer_offset == bytes_written) break;
                if (buffer_offset >= bytes_written) {
                    error("offset out of range");
                    return;
                }

                auto info = (PFILE_NOTIFY_INFORMATION)(buffer.data() + buffer_offset);

                if (info->NextEntryOffset > 0) buffer_offset +=
                    info->NextEntryOffset; else buffer_offset =
                    bytes_written;

                str what = "unknown";

                switch (info->Action) {
                case FILE_ACTION_ADDED:            what = "added"; break;
                case FILE_ACTION_RENAMED_OLD_NAME: what = "renamed (was)"; break;
                case FILE_ACTION_RENAMED_NEW_NAME: what = "renamed (now)"; break;
                case FILE_ACTION_REMOVED:          what = "removed"; break;
                case FILE_ACTION_MODIFIED:         what = "modified"; break;
                }

                std::basic_string<WCHAR> filename(info->FileName, info->FileNameLength/2);

                action(dir/filename, what);
            }
        }
    });
};
void sys::directory_watcher::cancel ()
{
    stop = true;
    if (thread.joinable()) {
        ::CancelSynchronousIo(thread.native_handle());
        thread.join();
    }
}

///////////////////////////////////////////////////////////////////////////////

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
        str s = unwinstr(std::wstring(ClipboardText(h).text));
        s.erase_if([](auto c){ return c == '\r'; });
        return s;
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

///////////////////////////////////////////////////////////////////////////////

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
