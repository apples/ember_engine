local vdom = require('vdom')

-- filled: boolean
-- left: number
local function heart(props)
    return vdom.create_element('label', {
        color = '#f00',
        text = props.filled and '[#]' or '[ ]',
        height = 32,
        left = props.left,
        valign = 'top',
        halign = 'left',
    })
end

-- health: number
-- max_health: number
-- score: number
return function(props)
    -- Make hearts
    local hearts = {}
    for i = 1, props.health do
        hearts[i] = vdom.create_element(heart, { filled = true, left = i * 32 })
    end
    for i = props.health + 1, props.max_health do
        hearts[i] = vdom.create_element(heart, { filled = false, left = i * 32 })
    end

    return vdom.create_element('widget', { width = '100%', height = '100%' },
        hearts,
        vdom.create_element('label', { text = score, valign = 'top', halign = 'right', size = 32 })
    )
end
