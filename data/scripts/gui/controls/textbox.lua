local vdom = require('vdom')

function pick(src, ...)
    local rv = {}
    for _,k in ipairs({...}) do
        rv[k] = src[k]
    end
    return rv
end

return function(props)
    local panelprops = pick(props, 'height', 'width', 'left', 'right', 'bottom', 'top', 'halign', 'valign')

    panelprops.on_click = vdom.useCallback(function(widget, pos) focus_widget(widget) end, {})

    panelprops.on_textinput = vdom.useCallback(function(widget, text)
        props.on_change(props.value .. text)
    end, { props.on_change, props.value })

    panelprops.on_keydown = vdom.useCallback(function(widget, keyname)
        if keyname == 'Backspace' then
            props.on_change(string.sub(props.value, 1, -2))
            return true
        end
        return false
    end, { props.on_change, props.value })

    return vdom.create_element('panel', panelprops,
        vdom.create_element('label', { height=props.height, text=props.value })
    )
end
