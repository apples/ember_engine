
local paddle = {}

function paddle.update(eid, delta)
    local controller = entities:get_component(eid, component.controller)
    local body = entities:get_component(eid, component.rigid_body).body

    local target_vel = 0

    if controller.left then
        target_vel = -15
    elseif controller.right then
        target_vel = 15
    end

    local ix = (target_vel - body:GetLinearVelocity().x) * body:GetMass();
    local iy = (-body:GetLinearVelocity().y) * body:GetMass();
    body:ApplyLinearImpulseToCenter(b2Vec2.new(ix, iy), true)

    local t = body:GetTransform()
    body:SetTransform(b2Vec2.new(t.p.x, -14.5), t.q:GetAngle())
end

return paddle
