#include <vector>

namespace sys
{
    namespace memory
    {
        namespace heap
        {
            void* request (int64_t bytes);
            bool  release (void* pointer);
        }
    }

    namespace window
    {
        inline std::vector<uint32_t> image;
        namespace size { inline int x, y; };
        void redraw (int x, int y, int w, int h);

        namespace on
        {
            void size ();
            void draw (int x, int y, int w, int h);
        }
    }
}