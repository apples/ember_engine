local new_ball = require('archetypes.ball')

local ball = {}

function ball.update(eid, delta)
    local transform = entities:get_component(eid, component.transform)
    local body = entities:get_component(eid, component.body)

    body.vel.y = body.vel.y + math.abs(body.vel.y)/body.vel.y * delta

    if transform.pos.y < -16 then
        lose_life()
        entities:destroy_entity(eid)
        new_ball()
    elseif transform.pos.y > 16 or transform.pos.x < -16 or transform.pos.x > 16 then
        entities:destroy_entity(eid)
        new_ball()
    end
end

return ball

