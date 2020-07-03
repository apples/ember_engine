local engine = require('engine')
local visitor = require('visitor')

local snakes = {}

function snakes.visit()
    visitor.visit(
        {component.snake},
        function (eid, script)
            if script.next_tick == 0 then
                local script_impl = require('actors.' .. script.name)
                local success, ret = pcall(script_impl.update, eid, engine.field)
                if not success then
                    print('ERROR: systems.snakes: actors.'..script.name..': '..ret)
                end
            elseif script.next_tick > 0 then
                script.next_tick = script.next_tick - 1
            end
        end
    )
end

return snakes
