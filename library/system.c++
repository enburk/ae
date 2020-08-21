#include <cstdio>
namespace ae::system
{
    void out (std::span<uint8_t> data)
    {
        std::fwrite(data.data(), data.size(), 1, stdout);
        std::fwrite("\n", 1, 1, stdout);
        std::fflush(stdout);
    }
    void err (std::span<uint8_t> data)
    {
        std::fwrite(data.data(), data.size(), 1, stderr);
        std::fwrite("\n", 1, 1, stderr);
        std::fflush(stderr);
    }
}
#ifdef SUBSYSTEM_CONSOLE
int main (int argc, char *argv[])
{
    ae::main_();
    std::puts("");
    std::puts("press Enter to close...");
    std::getc(stdin);
}
#else
#include "system/windows.c++"
#endif










