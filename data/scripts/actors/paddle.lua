
local paddle = {}

function paddle.update(eid, delta)
    local controller = entities:get_component(eid, component.controller)
    local body = entities:get_component(eid, component.rigid_body)

    local target_vel = 0

    if controller.left then
        target_vel = -15
    elseif controller.right then
        target_vel = 15
    end

    local ix = (target_vel - body.body:GetLinearVelocity().x) * body.body:GetMass();
    body.body:ApplyLinearImpulseToCenter(b2Vec2.new(ix, 0), true)
end

return paddle

