return function()
    local sprite = component.sprite.new()
    sprite.texture = "sprites";
    sprite.size.x = 1/8
    sprite.size.y = 1/8
    sprite.frames:add(1)
    sprite.frames:add(0)
    sprite.frames:add(8)
    sprite.frames:add(0)

    local transform = component.transform.new()
    transform.scl.x = 0.5
    transform.scl.y = 0.5

    local body = component.body.new()
    body.type = 2
    body.layer = 0
    body.size.x = 0.5
    body.size.y = 0.5
    body.vel.x = (math.random() * 2 - 1) * 15
    body.vel.y = -20
    body.collides_with[1] = true
    body.collides_with[2] = true
    body.collides_with[3] = true

    local script = component.script.new()
    script.name = 'ball'

    local eid = entities:create_entity()
    entities:add_component(eid, sprite)
    entities:add_component(eid, transform)
    entities:add_component(eid, body)
    entities:add_component(eid, script)
    entities:add_component(eid, component.tag_brick_breaker.new())
end
