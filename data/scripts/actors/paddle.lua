
local paddle = {}

function paddle.update(eid, delta)
    local controller = entities:get_component(eid, component.controller)
    local body = entities:get_component(eid, component.body)

    if controller.left then
        body.vel.x = -15
    elseif controller.right then
        body.vel.x = 15
    else
        body.vel.x = 0
    end
end

return paddle

