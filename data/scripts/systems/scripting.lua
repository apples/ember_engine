local engine = require('engine')
local visitor = require('visitor')

local scripting = {}

function scripting.visit(delta)
    visitor.visit(
        {component.script},
        function (eid, script)
            if script.next_tick == 0 then
                local script_impl = require('actors.' .. script.name)
                local success, ret = pcall(script_impl.update, eid, delta, engine.entities)
                if not success then
                    print('ERROR: systems.scripting: actors.'..script.name..': '..ret)
                end
            elseif script.next_tick > 0 then
                script.next_tick = script.next_tick - 1
            end
        end
    )
end

return scripting
