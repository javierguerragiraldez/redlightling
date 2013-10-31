_log("API")

ffi.cdef (assert(io.open('api.h')):read('*a'))

local function _c(err)
	if err then
		print ('err:', err)
	end
	return err
end

local function _args(...)
	local n = select('#', ...)
	local ret = ffi.new('arg_t[?]', n)
	for i = 1, n do
		ret[i-1].data, ret[i-1].size = _buflen(select(i, ...))
	end
	return n, ret
end

_log("que falle abriendo...")
assert (C.rl_new_conn('nohay') == nil)

_log("ahora si debe porder")
local h = assert (C.rl_new_conn('testapidb/'))

_log("no hace nada...")
assert (C.rl_do(h, 'c_NOOP', _args()) == C.noErr)

do
	_log("lee inexistente")
	local n, a = _args('keyX', nil)
	assert(_c(C.rl_do(h, C.c_GET, n, a)) == -30798)			--MDB_NOTFOUND
	assert(a[1].data == nil)
	assert(a[1].size == 0)
end

_log("graba alguito")
assert (C.rl_do(h, C.c_SET, _args('key1', 'data1')) == C.noErr)

do
	_log("lo vuelve a leer")
	local n, a = _args('key1', nil)
	assert(_c(C.rl_do(h, C.c_GET, n, a)) == C.noErr)
	assert (ffi.string(a[1].data, a[1].size) == 'data1')
end

assert(_c(C.rl_do(h, C.c_DELETE, _args('key1'))) == C.noErr)
do
	_log("ya no existe")
	local n, a = _args('key1', nil)
	assert(_c(C.rl_do(h, C.c_GET, n, a)) == -30798)			--MDB_NOTFOUND
	assert(a[1].data == nil)
	assert(a[1].size == 0)
end
