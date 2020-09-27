return function()
    local sprite = component.sprite.new()
    sprite.texture = "sprites";
    sprite.size.y = 1/4
    sprite.frames:add(1)

    local transform = component.transform.new()
    transform.pos.y = -14.5
    transform.scl.x = 4

    local bodyDef = b2BodyDef.new()
    bodyDef.type = b2BodyType.b2_dynamicBody
    bodyDef.fixedRotation = true
    bodyDef.position:Set(transform.pos.x, transform.pos.y)
    bodyDef.linearDamping = 1

    local shape = b2PolygonShape.new()
    shape:SetAsBox(2, 0.5)

    local body = component.rigid_body.new()
    body.body = world:CreateBody(bodyDef)
    body.body:CreateFixture(shape, 1)

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
