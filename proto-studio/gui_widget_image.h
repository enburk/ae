#pragma once
#include <fstream>
#include "pix_sampling.h"
#include "gui_widget_canvas.h"
#include "gui_widgetarium.h"
namespace gui
{
    struct image final : widget<image>
    {
        unary_property<pix::frame<RGBA>> source;
        enum fitness { none, fill, contain, cover, scale_down };
        binary_property<fitness> fit = fill;

        pix::image<RGBA> resized_image;
        pix::frame<RGBA> resized_frame;
        XY shift;

        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override
        {
            auto frame = resized_frame
                .crop(XYWH(
                offset.x + shift.x,
                offset.y + shift.y,
                r.size.x, r.size.y));

            if (frame.size.x > 0
            &&  frame.size.y > 0)
                window.render(r, alpha, frame);
        }

        void on_change (void* what) override
        {
            if((what == &coord && coord.was.size != coord.now.size)
            ||  what == &source
            ||  what == &fit)
            {
                if (source.now.size.x == 0
                ||  source.now.size.y == 0
                ||  source.now.size == coord.now.size
                ||  fit.now == none || (fit.now == scale_down
                &&  source.now.size.x <= coord.now.size.x
                &&  source.now.size.y <= coord.now.size.y))
                {
                    resized_image.resize(XY());
                    resized_frame = source.now;
                }
                else if (fit.now == fill)
                {
                    resized_image = pix::resized(source.now, coord.now.size);
                    resized_frame = resized_image.crop();
                }
                else if (fit.now == cover)
                {
                    XY size = coord.now.size;
                    XY maxsize = source.now.size;
                    size = size.x/maxsize.x > size.y/maxsize.y ?
                    XY(maxsize.x * size.y / maxsize.y, size.y) :
                    XY(size.x, maxsize.y * size.x / maxsize.x);
                    resized_image = pix::resized(source.now, size);
                    resized_frame = resized_image.crop();
                }
                else // (fit.now == contain) // or scale_down and less
                {
                    XY size = coord.now.size;
                    XY maxsize = source.now.size;
                    size = size.x/maxsize.x < size.y/maxsize.y ?
                    XY(maxsize.x * size.y / maxsize.y, size.y) :
                    XY(size.x, maxsize.y * size.x / maxsize.x);
                    resized_image = pix::resized(source.now, size);
                    resized_frame = resized_image.crop();
                }

                shift = coord.now.size/2 - resized_frame.size/2;

                update();
            }
        }
    };

    struct media
    {
        enum class state { loading, playing, finished, failure };
    };

    struct player : widget<player>
    {
        std::atomic<media::state> state = media::state::finished;
        std::atomic<time> duration = time{};
        std::atomic<time> elapsed = time{};
        std::atomic<bool> pause = false;
        std::atomic<bool> stop = false;
        XY resolution;
        str error;

        image frames[2];
        pix::image<RGBA> sources[2];
        std::atomic<int> current_frame = 0;
        std::atomic<bool> frame_ready = false;
        property<time> timer;
        std::thread thread;
        using byte = sys::byte;
        array<byte> data;

        ~player () { reset(); }

        void load (pix::frame<RGBA> frame)
        {
            reset();
            int next = (current_frame + 1) % 2;
            sources[next].resize(frame.size);
            sources[next].crop().copy_from(frame);
            resolution = sources[next].size;
            state = media::state::finished;
            frame_ready = true;
        }
        void load (array<sys::byte>::range_type range)
        {
            reset();
            state = media::state::loading;
            thread = std::thread([this, range]()
            {
                try
                {
                    data.resize(range.size());
                    std::copy(range.begin(), range.end(), data.begin());
                    play();
                }
                catch (std::exception & e) {
                    error = e.what();
                    state = media::state::failure;
                }
            });
        }
        void load (std::filesystem::path path, int offset = 0, int length = 0)
        {
            reset();
            state = media::state::loading;
            thread = std::thread([this, path, offset, length]()
            {
                try
                {
                    std::ifstream ifstream (path, std::ios::binary);

                    int size = length;
                    if (size == 0)
                    {
                        ifstream.seekg(0, std::ios::end);
                        size = (int)ifstream.tellg();
                    }
                    ifstream.seekg(offset, std::ios::beg);

                    data.resize(size);
                    ifstream.read((char*)(data.data()), size);

                    play();
                }
                catch (std::exception & e) {
                    error = e.what();
                    state = media::state::failure;
                }
            });
        }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &coord && coord.was.size != coord.now.size)
            {
                if (frames[current_frame].alpha.to != 0)
                    frames[current_frame].coord = coord.now.local();
            }
            if (what == &timer && frame_ready)
            {
                frames[current_frame].hide();
                current_frame = (current_frame + 1) % 2;
                frames[current_frame].source = sources[current_frame];
                frames[current_frame].coord = coord.now.local();
                frames[current_frame].show();
                frame_ready = false;
            }
        }

        private: void reset ()
        {
            stop = true;
            if (thread.joinable())
                thread.join();

            data.clear();
            resolution = XY{};
            duration = time{};
            elapsed = time{};
            current_frame = 0;
            frame_ready = false;
            state = media::state::finished;
            error = "";
        }
        private: void play ()
        {
            if (data.size() > 5  
            &&  data[0] == 0x47  // G
            &&  data[1] == 0x49  // I
            &&  data[2] == 0x46  // F
            &&  data[3] == 0x38  // 8
            &&  data[4] == 0x39  // 9
            &&  data[5] == 0x61) // a
            {
                error = "don't support gif yet";
                state = media::state::failure;
            }
            else
            {
                int next = (current_frame + 1) % 2;
                sources[next] = pix::unpack(data.from(0)).value();
                resolution = sources[next].size;
                state = media::state::finished;
                frame_ready = true;
            }
        }
    };
}
