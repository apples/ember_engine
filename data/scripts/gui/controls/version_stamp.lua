local vdom = require('vdom')

return function(props)
    return vdom.create_element(
        'label',
        {
            halign='right',
            valign='top',
            top=0,
            height = 24,
            text = props.version,
            color = '#f0f'
        }
    )
end

