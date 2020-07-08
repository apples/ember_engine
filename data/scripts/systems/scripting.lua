local visitor = require('visitor')

local scripting = {}

function scripting.visit(delta)
    visitor.visit(
        entities,
        {component.script},
        function (eid, script)
            local script_impl = require('actors.' .. script.name)
            if script_impl.update then
                local success, ret = pcall(script_impl.update, eid, delta)
                if not success then
                    print('ERROR: systems.scripting: actors.'..script.name..': '..ret)
                end
            end
        end
    )
end

return scripting
