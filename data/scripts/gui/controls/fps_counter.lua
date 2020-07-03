local vdom = require('vdom')

return function(props)
    return vdom.create_element(
        'label',
        {
            halign='right',
            valign='top',
            top = 24,
            height = 24,
            text = props.fps,
            color = '#f0f',
        }
    )
end
