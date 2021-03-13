local function hello()
    return {
        status = 200,
        body = 'hello, world'
    }
end

local router = require('http.router').new()
router:route({ method = 'GET', path = '/hello' }, hello)

local server = require('http.server').new('localhost', 9000)
server:set_router(router)

server:start()
