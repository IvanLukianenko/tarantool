local fio = require('fio')
local digest = require('digest')
local port = tonumber(arg[1])
local os = require('os')
local socket = require('socket')
local log = require('log')

if port == nil then
    error('Invalid port')
end


times = {}
local index = 0
local index_10mill = 0
local req_per_sec = 1
local work_dir = fio.pathjoin('data', port)
fio.mktree(work_dir)
box.cfg({
    listen = port,
    work_dir = work_dir,
})
box.schema.user.passwd('admin', 'test')

local function req_per_sec(times, actual_time)
    local rps = 1
    for i = #times-1, 1, -1 do
        if times[i] >= actual_time - 1 then
            rps = rps + 1
        else
            break
        end
    end
    return rps
end

function exec()
    local actual_time = (os.time())
    log.info(actual_time)
    times[index] = actual_time 
    index = index + 1
    rps = req_per_sec(times, actual_time)
    local str = string.format("<localhost:%s>:%d", port, rps)
    for _ = 1, 1e2 do
        digest.sha512_hex(str)
    end
    return str
end