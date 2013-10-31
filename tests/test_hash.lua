_log("hash")

ffi.cdef [[
	void initHashSeed(void);
	uint64_t siphash(const uint8_t *in, size_t inlen, const uint64_t k[2]);
]]

C.initHashSeed()
local sipkey = ffi.new('uint64_t[2]', 41, 42)
local r2 = assert(C.rl_new_record(h), "can't alloc record 2")
C.rl_set_key(r2, _buflen("key1"))

local shv = C.siphash(r2.key.mv_data, r2.key.mv_size, sipkey)
_log("siphash: %s", tostring(shv))

local function cmp128(a,b)
	return a[0]==b[0] 
		and a[1]==b[1]
		and a[2]==b[2]
		and a[3]==b[3]
end

local function allhashes(k)
	local shv = C.siphash(ffi.cast("uint8_t *", k), #k, sipkey)
	_log("siphash(%q):	%s", k, tostring(shv))
end
allhashes("")
allhashes("cataract")
allhashes("periti")
allhashes("roquette")
allhashes("skivie")
allhashes("shawl")
allhashes("stormbound")
allhashes("dowlases")
allhashes("tramontane")
allhashes("cricketings")
allhashes("twanger")
allhashes("longans")
allhashes("whigs")
