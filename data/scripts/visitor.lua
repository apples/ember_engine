local linq = require('linq')

local visitor = {}

function visitor.visit(entities, coms, func)
    entities:visit(
        function (eid)
            local first_missing = linq(coms)
                :where(function (com) return not entities:has_component(eid, com) end)
                :first()

            if first_missing then
                return
            end

            local com_vals = linq(coms)
                :select(function (com) return entities:get_component(eid, com) end)
                :tolist()

            func(eid, table.unpack(com_vals))
        end
    )
end

return visitor
