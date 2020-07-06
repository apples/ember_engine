
local scene = {}

local function make_paddle()
    local sprite = component.sprite.new()
    sprite.texture = "sprites";
    sprite.size.y = 1/4
    sprite.frames:add(1)

    local transform = component.transform.new()
    transform.scl.x = 4

    local test = entities:create_entity()
    entities:add_component(test, sprite)
    entities:add_component(test, transform)
end

local function make_ball()
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

    local test = entities:create_entity()
    entities:add_component(test, sprite)
    entities:add_component(test, transform)
end

function scene.init()
    print('Initializing gameplay...')

    make_ball()

    print('Initialized gameplay.')
end

return scene
