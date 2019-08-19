#include "sys.h"

namespace sys::memory::heap
{
    void* request (int64_t bytes)
    {
        const int cache_line_size = 64;
        const int max_threads = 128;
        const int 
    }
            
    bool  release (void* pointer)
    {
    }
}