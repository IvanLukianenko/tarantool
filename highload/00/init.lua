local json = require('json')
local users = require('users')

local function make_response(code, body)
    return {
        status = code,
        headers = { ['content-type'] = 'application/json; charset=utf-8' },
        body = json.encode(body)
    }
end

local function add(req)
    local ok, user_info = pcall(json.decode, req:read())
    if ok ~= true then
        return make_response(400, { error = user_info })
    end

    if user_info.email == nil
        or user_info.name == nil
        or user_info.password == nil
    then
        return make_response(400, { error = 'invalid usage' })
    end

    local ok, user_info, err = pcall(users.add,
        user_info.email, user_info.name, user_info.password)

    if ok ~= true then
        return make_response(500, { error = user_info })
    end

    if err ~= nil then
        return make_response(400, { error = err })
    end

    return make_response(201, user_info)
end

local function login(req)
    local email = req:query_param('email')
    local password = req:query_param('password')

    if email == nil or password == nil then
        return make_response(400, { error = 'invalid usage' })
    end

    local ok, user_info, err = pcall(users.login, email, password)

    if ok ~= true then
        return make_response(500, { error = user_info })
    end

    if err ~= nil then
        return make_response(400, { error = err })
    end

    return make_response(200, user_info)
end

box.cfg()

local router = require('http.router').new()
router:route({ method = 'POST', path = '/add' }, add)
router:route({ method = 'GET', path = '/login' }, login)

local server = require('http.server').new('localhost', 9000)
server:set_router(router)

server:start()