#pragma once
#include <atomic>
#include <thread>
#include <filesystem>
#include "aux_string.h"
#include "aux_utils.h"
#include "pix_color.h"
#include "pix_image.h"
namespace sys
{
    using pix::XY;
    using pix::XYWH;
    using pix::XYXY;
    using pix::RGBA;
    using pix::MONO;
    using namespace aux;

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
        process (std::filesystem::path, str args, options=options{});
       ~process ();

        bool wait (int ms = max<int>());
    };

    struct directory_watcher
    {
        using path = std::filesystem::path; path dir;
        std::function<void(path, str)> action = [](path, str){};
        std::function<void(aux::error)> error = [](aux::error){};
        std::thread thread; void watch(); void cancel();
        std::atomic<bool> stop = false;
        ~directory_watcher(){cancel();}
    };
}