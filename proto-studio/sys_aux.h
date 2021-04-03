#pragma once
#include <atomic>
#include <thread>
#include <filesystem>
#include "data_struct_string.h"
#include "pix_color.h"
#include "pix_image.h"
namespace sys
{
    using namespace data;
    using namespace pix;
    using byte = uint8_t;

    namespace clipboard
    {
        void set (str);
        void set (pix::frame<RGBA>);
        namespace get {
            pix::image<RGBA> image ();
            str string ();
        }
    }

    namespace settings
    {
        str  load (str name, str default_value);
        int  load (str name, int default_value);
        void save (str name, str value);
        void save (str name, int value);
    }

    struct process
    {
        size_t handle = 0;
        struct options
        {
            bool hidden = false;
            std::filesystem::path out;
            int ms_wait_for_input_idle = 0;
        };
        process () noexcept = default;
        process (const process&) = delete;
        process (process&& p) noexcept { handle = p.handle; }
        process (std::filesystem::path, str args, options);
        process (std::filesystem::path, str args);
       ~process ();

        bool wait (int ms = max<int>());
    };

    struct directory_watcher
    {
        using path = std::filesystem::path; path dir;
        std::function<void(path, str)> action = [](path, str){};
        std::function<void(data::error)> error = [](data::error){};
        std::thread thread; void watch(); void cancel();
        std::atomic<bool> stop = false;
        ~directory_watcher(){cancel();}
    };

    namespace audio
    {
        struct player
        {
            player();
           ~player();

            void load(array<byte>, int channels, int samples, int bps);
            void play(double rise, double fade);
            void stop(double fade);
            double duration = 0.0;

            void* data_ = nullptr;
        };
    }
}

namespace pix
{
    using byte = sys::byte;

    expected<image<RGBA>> read   (std::filesystem::path);
    expected<nothing>     write  (frame<RGBA>, std::filesystem::path, int quality = -1);
    expected<array<byte>> pack   (frame<RGBA>, str format, int quality = -1);
    expected<image<RGBA>> unpack (byte* buffer, int size);
    expected<image<RGBA>> unpack (array<byte>::range_type);
    expected<XY>          size   (array<byte>::range_type);
}
