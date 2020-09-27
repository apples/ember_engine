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

    local bodyDef = b2BodyDef.new()
    bodyDef.type = b2BodyType.b2_dynamicBody
    bodyDef.fixedRotation = true
    bodyDef.bullet = true
    bodyDef.position:Set(transform.pos.x, transform.pos.y)
    bodyDef.linearVelocity:Set((math.random() * 2 - 1) * 15, -20)

    local shape = b2PolygonShape.new()
    shape:SetAsBox(0.25, 0.25)

    local fixture = b2FixtureDef.new()
    fixture.shape = shape
    fixture.friction = 0
    fixture.restitution = 1
    fixture.density = 1

    local body = component.rigid_body.new()
    body.body = world:CreateBody(bodyDef)
    body.body:CreateFixture(fixture)

    local script = component.script.new()
    script.name = 'ball'

    local eid = entities:create_entity()
    entities:add_component(eid, sprite)
    entities:add_component(eid, transform)
    entities:add_component(eid, body)
    entities:add_component(eid, script)
    entities:add_component(eid, component.tag_brick_breaker.new())
end
