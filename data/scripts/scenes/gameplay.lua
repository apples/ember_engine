local new_ball = require('archetypes.ball')
local new_paddle = require('archetypes.paddle')
local new_brick = require('archetypes.brick')

local scene = {}

local function make_walls()
    do
        local transform = component.transform.new()
        transform.pos.y = 16

        local bodyDef = b2BodyDef.new()
        bodyDef.fixedRotation = true
        bodyDef.position:Set(transform.pos.x, transform.pos.y)

        local shape = b2PolygonShape.new()
        shape:SetAsBox(16, 1)

        local body = component.rigid_body.new()
        body.body = world:CreateBody(bodyDef)
        body.body:CreateFixture(shape, 1)

        local eid = entities:create_entity()
        entities:add_component(eid, transform)
        entities:add_component(eid, body)
    end

    local function spawn_vertical_wall(x)
        local transform = component.transform.new()
        transform.pos.x = x

        local bodyDef = b2BodyDef.new()
        bodyDef.fixedRotation = true
        bodyDef.position:Set(transform.pos.x, transform.pos.y)

        local shape = b2PolygonShape.new()
        shape:SetAsBox(1, 16)

        local body = component.rigid_body.new()
        body.body = world:CreateBody(bodyDef)
        body.body:CreateFixture(shape, 1)

        local eid = entities:create_entity()
        entities:add_component(eid, transform)
        entities:add_component(eid, body)
    end

    spawn_vertical_wall(16)
    spawn_vertical_wall(-16)
end

function scene.init()
    print('Initializing gameplay...')

    new_ball()
    new_paddle()
    make_walls()

    for r = 1, 5 do
        for c = 1, 11 do
            new_brick(-12 + c * 2, 4 + r)
        end
    end

    print('Initialized gameplay.')
end

return scene
