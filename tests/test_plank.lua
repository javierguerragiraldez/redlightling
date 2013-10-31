_log("API")

ffi.cdef (assert(io.open('plank.h')):read('*a'))


local arena = assert(C.plk_create(16384), "can't allocate 16KB plank")

assert(C.plk_alloc(arena, 32768)==nil, "shouldn't allocate 32K in a 16K plank")
local blk1 = assert(C.plk_alloc(arena, 1024), "can't allocate 1K in a 16K plank")