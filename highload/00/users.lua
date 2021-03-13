local clock = require('clock')
local digest = require('digest')

local function get_space()
    local space = box.space.users
    if space ~= nil then
        return space
    end

    box.begin()

    space = box.schema.space.create('users')

    space:format({
        { name = 'email',       type = 'string',    is_nullable = false },
        { name = 'name',        type = 'string',    is_nullable = false },
        { name = 'password',    type = 'string',    is_nullable = false },
        { name = 'created_at',  type = 'unsigned',  is_nullable = false },
        { name = 'last_login',  type = 'unsigned',  is_nullable = true }
    })

    space:create_index('email', {
        type = 'HASH',
        unique = true,
        if_not_exists = true,
        parts = {{ field = 'email', type = 'string', collation = 'unicode_ci' }}
    })

    box.commit()

    return space
end

local function get_password_hash(password)
    local salt = 'C9DFB8DADFDA4F8EBD4EA545ACD4197B'
    return digest.sha512_hex(password .. salt)
end

local function add(email, name, password)
    if email == nil or name == nil or password == nil then
        return nil, 'invalid usage'
    end

    local space = get_space()

    local user_info, err = space:insert({
        email,
        name,
        get_password_hash(password),
        clock.time64(),
        box.NULL
    })

    if err ~= nil then
        return nil, "can't add user: " .. err
    end

    user_info = user_info:tomap({ names_only = true })
    user_info.password = nil
    return user_info
end

local function login(email, password)
    if email == nil or password == nil then
        return nil, 'invalid usage'
    end

    local space = get_space()

    local user_info = space:get(email)
    if user_info == nil then
        return nil, 'unknown user'
    end

    if get_password_hash(password) ~= user_info.password then
        return nil, 'password mismatch'
    end

    user_info = space:update(email, {{ '=', 'last_login', clock.time64() }})
    user_info = user_info:tomap({ names_only = true })
    user_info.password = nil
    return user_info
end

return {
    add = add,
    login = login
}