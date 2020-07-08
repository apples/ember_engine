return function()
    local sprite = component.sprite.new()
    sprite.texture = "sprites";
    sprite.size.y = 1/4
    sprite.frames:add(1)

    local transform = component.transform.new()
    transform.pos.y = -14.5
    transform.scl.x = 4

    local body = component.body.new()
    body.type = 1
    body.layer = 1
    body.size.x = 4
    body.size.y = 1
    body.collides_with[3] = true

    local controller = component.controller.new()

    local script = component.script.new()
    script.name = 'paddle'

    local eid = entities:create_entity()
    entities:add_component(eid, sprite)
    entities:add_component(eid, transform)
    entities:add_component(eid, body)
    entities:add_component(eid, controller)
    entities:add_component(eid, script)
end
