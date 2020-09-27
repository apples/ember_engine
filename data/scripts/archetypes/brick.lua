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

    local bodyDef = b2BodyDef.new()
    bodyDef.fixedRotation = true
    bodyDef.position:Set(transform.pos.x, transform.pos.y)

    local shape = b2PolygonShape.new()
    shape:SetAsBox(1, 0.5)

    local body = component.rigid_body.new()
    body.body = world:CreateBody(bodyDef)
    body.body:CreateFixture(shape, 1)

    local script = component.script.new()
    script.name = 'brick'

    local eid = entities:create_entity()
    entities:add_component(eid, sprite)
    entities:add_component(eid, transform)
    entities:add_component(eid, body)
    entities:add_component(eid, script)
end
