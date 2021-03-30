local log = require('log')
local netbox = require('net.box')
local http_server = require('http.server')

local hosts = {
    'admin:test@localhost:3301',
    'admin:test@localhost:3302',
    'admin:test@localhost:3303',
}

function add_connection(host)
    local conn = netbox.connect(host)
    assert(conn)
    log.info('Connected to %s', host)
    table.insert(connections, conn)
end

connections = {}

for _, host in ipairs(hosts) do
    add_connection(host)
end

function remove_connection(conn_num)
    if conn_num >= #connections or conn_num <= 0 then
        print("No such connection")
    else
        table.remove(connections, conn_num)
    ends
end

local req_num = 1
local function handler()
    local conn = connections[req_num]
    if req_num == #connections then
        req_num = 1
    else
        req_num = req_num + 1
    end

    if conn:is_connected() == false then
        log.info(req_num)
        if req_num == 1 then
            remove_connection(#connections)
        else
            remove_connection(req_num-1)
        end
        req_num = 1 
        result = handler()

        return {
            body = result.body,
            status = result.status,
        }

    else
        local result = conn:call('exec')

        return {
            body = result,
            status = 200,
        }
    end
end

local httpd = http_server.new('0.0.0.0', '8080', {log_requests = false})
local router = require('http.router').new()
router:route({ method = 'GET', path = '/' }, handler)

httpd:set_router(router)
httpd:start()