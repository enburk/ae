#include <vector>

namespace sys
{
    namespace window
    {
        inline std::vector<uint32_t> image;
        struct size { inline static int x, y; };
        void redraw (int x, int y, int w, int h);

        namespace on
        {
            void size ();
            void draw (int x, int y, int w, int h);
        }
    }
}