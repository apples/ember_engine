return function(x, y)
    local sprite = component.sprite.new()
    sprite.texture = "sprites";
    sprite.size.y = 1/4
    sprite.size.x = 1/2
    sprite.frames:add(4)

    local transform = component.transform.new()
    transform.pos.x = x
    transform.pos.y = y
    transform.scl.x = 2

    local body = component.body.new()
    body.type = 0
    body.layer = 2
    body.size.x = 2
    body.size.y = 1
    body.collides_with[1] = true

    local script = component.script.new()
    script.name = 'brick'

    local eid = entities:create_entity()
    entities:add_component(eid, sprite)
    entities:add_component(eid, transform)
    entities:add_component(eid, body)
    entities:add_component(eid, script)
end
