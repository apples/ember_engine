local new_ball = require('archetypes.ball')

local ball = {}

function ball.update(eid, delta)
    local transform = entities:get_component(eid, component.transform)
    local body = entities:get_component(eid, component.rigid_body)

    local vy = body.body:GetLinearVelocity().y
    if vy ~= 0 then
        local iy = (math.abs(vy)/vy * delta) * body.body:GetMass()
        body.body:ApplyLinearImpulseToCenter(b2Vec2.new(0, iy), true)
    end

    if transform.pos.y < -16 then
        lose_life()
        queue_destroy(eid)
        new_ball()
    elseif transform.pos.y > 16 or transform.pos.x < -16 or transform.pos.x > 16 then
        queue_destroy(eid)
        new_ball()
    end
end

return ball

