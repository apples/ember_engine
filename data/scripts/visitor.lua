local engine = require('engine')
local linq = require('linq')

local visitor = {}

function visitor.visit(coms, func)
    engine.entities:visit(
        function (eid)
            local first_missing = linq(coms)
                :where(function (com) return not engine.entities:has_component(eid, com) end)
                :first()

            if first_missing then
                return
            end

            local com_vals = linq(coms)
                :select(function (com) return engine.entities:get_component(eid, com) end)
                :tolist()

            func(eid, table.unpack(com_vals))
        end
    )
end

return visitor
