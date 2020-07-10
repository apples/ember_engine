local new_ball = require('archetypes.ball')
local new_paddle = require('archetypes.paddle')
local new_brick = require('archetypes.brick')

local scene = {}

local function make_walls()
    local transform = component.transform.new()
    transform.pos.y = 16

    local body = component.body.new()
    body.type = 0
    body.layer = 2
    body.size.x = 32
    body.size.y = 2

    local eid = entities:create_entity()
    entities:add_component(eid, transform)
    entities:add_component(eid, body)

    local function spawn_vertical_wall(x)
        local transform = component.transform.new()
        transform.pos.x = x

        local body = component.body.new()
        body.type = 0
        body.layer = 2
        body.size.x = 2
        body.size.y = 32

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
