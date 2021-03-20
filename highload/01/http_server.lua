local fio = require('fio')

local file = fio.open('config.yml')

local raw_conf = file:read()

local yaml = require('yaml')

local conf = yaml.decode(raw_conf)


local function handler()
    http_client = require('http.client').new({max_connections = 1})
    req = http_client:request('GET',"google.com")
    return {
        status = req.status,
        reason = req.reason,
        headers = req.headers,
        body = req.body,
        proto = req.proto
    }
end

local router = require('http.router').new()
router:route({ method = 'GET', path = '/' }, handler)

local server = require('http.server').new(conf.proxy.bypass.host, conf.proxy.bypass.port)
server:set_router(router)

server:start()