
ffi = require('ffi')

ffi.cdef [[
	void out_log (const char *s);
]]

C = ffi.C

local old_assert = assert
function assert(f, ...)
	if f==nil then
		f = nil
	end
	return old_assert(f, ...)
end

local function _ss(x, ...)
	if select('#', ...) > 0 then
		return tostring(x), _ss(...)
	else
		return tostring(x)
	end
end
function _log(s, ...)
	C.out_log(s:format(_ss(...)))
end
function _showval(v)
	return ('"%s" (%d:%s)'):format(
		ffi.string(v.mv_data, v.mv_size), 
		tonumber(v.mv_size), 
		tostring(v.mv_data))
end
function _showrec(r, tag)
	print (tag or 'r: ', r)
	print ('	db: ', r.db)
	print ('	key: ', _showval(r.key))
	print ('	data: ', _showval(r.data))
	print ('	type: ', r.type)
end
function _buflen(s)
	if s == nil then
		return nil, 0
	end
	return ffi.cast("uint8_t *", s), #s
end

_log("start...")

-- dofile("tests/test_store.lua")
-- dofile("tests/test_hash.lua")
-- dofile("tests/test_api.lua")
dofile("tests/test_plank.lua")
print ("Ok.")
