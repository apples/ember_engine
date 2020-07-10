
local brick = {}

function brick.physics_post_collide(eid, other)
    if entities:has_component(other, component.tag_brick_breaker) then
        entities:destroy_entity(eid)
    end
end

return brick

