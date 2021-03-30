local digest = require('digest')
local http_server = require('http.server')

local function handler()
    local str = string.format("I'm %s", 8080)
    for _ = 1, 1e2 do
        digest.sha512_hex(str)
    end

    return {
        body = str,
        status = 200,
    }
end

local httpd = http_server.new('localhost', '8080', {log_requests = false})

local router = require('http.router').new()
router:route({ method = 'GET', path = '/' }, handler)

httpd:set_router(router)
httpd:start()