local fio = require('fio')
local io = require('io')
local yaml = require('yaml')

local file
local conf_name

repeat 
    conf_name = io.read()
    conf_name = tostring(conf_name)
    file = fio.open(conf_name)
    if file == nil then
        print("No such config file")
    end

    start, end_ = conf_name:find(".yml")

    if end_ ~= #conf_name or start == nil then
        print("No yaml file")
    end 
until file ~= nil and end_ == #conf_name

local raw_conf = file:read()

local conf = yaml.decode(raw_conf)

if conf == nil then
    print("some problems")
end

local http_client = require('http.client').new({max_connections = 1})

local function handler()
    req = http_client:request('GET',conf.proxy.url)
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