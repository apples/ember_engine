
local physics = {}

function physics.pre_collide(script_name, a, b)
    local script = require(script_name)
    if script.physics_pre_collide then
        script.physics_pre_collide(a, b)
    end
end

function physics.post_collide(script_name, a, b)
    local script = require(script_name)
    if script.physics_post_collide then
        script.physics_post_collide(a, b)
    end
end

return physics
