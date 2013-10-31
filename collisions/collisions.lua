local ffi = require('ffi')
ffi.cdef [[
	void MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out);
	void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);
	void MurmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);
	
	uint64_t siphash(const uint8_t *in, size_t inlen, const uint64_t k[2]);
]]


local C = ffi.C

local function h1(s)
	local outbuf = ffi.new('uint32_t[4]')
	C.MurmurHash3_x64_128(ffi.cast("uint8_t *", s), #s, 41, outbuf)
	return ffi.string(outbuf, 16)
end

local sipkey = ffi.new('uint64_t[2]', 41, 42)
local function h2(s)
	local h = C.siphash(ffi.cast("uint8_t *", s), #s, sipkey)
	return tostring(h)
end

local function hex(s)
	return string.format(string.rep('%X', #s), string.byte(s, 1, #s))
end

local function pf(...)
	print (string.format(...))
end

local function randstr(maxlen, minlen)
	local l = math.random(minlen or 1, maxlen)
	local t={}
	for i=1,l do
		t[i] = string.char(math.random(97,122))
	end
	return table.concat(t)
end

local t0 = os.time()
local tp = t0
local revhsh = {}
local coll = {}
local rep = {}

local function checkhash(w)
	local hsh = h2(w)
	local pw = revhsh[hsh]
	if pw then
		if pw ~= w then
			coll[#coll+1] = pw
			coll[#coll+1] = w
			pf ("%s <=*=> %s", pw, w)
			pf ("[%s] === [%s]", hex(h1(pw)), hex(h1(w)))
		else
			rep[#rep+1] = pw
		end
	else
		revhsh[hsh] = w
	end
end


for i = 1, 1e6 do
	local w = randstr(10, 5)
-- 	pf ("%d: %s", i,w)
	checkhash(w)
end
pf ("colls: %d, reps: %d", #coll, #rep)


local n = 0
for w in io.lines('collisions/lexicon.txt') do
	n = n + 1
	if n %100 == 0 then
		local tn = os.time()
		local dt = os.difftime(tn, tp)
		if dt > 10 then
			tp = tn
			pf ("t: %f n: %d", os.difftime(tn, t0), n)
		end
	end
	checkhash(w)
end

pf ("t: %f n: %d", os.difftime(os.time(), t0), n)
pf ("colls: %d, reps: %d", #coll, #rep)
