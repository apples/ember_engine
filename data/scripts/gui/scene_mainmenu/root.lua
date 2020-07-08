local vdom = require('vdom')

-- start_game: () => ()
return function(props)
    return vdom.create_element('widget', { width = '100%', height = '100%' },
        vdom.create_element(
            'label',
            {
                halign = 'center',
                bottom = 720/2 + 80,
                color = '#fff',
                text = 'Sample Game',
                height = 24,
            }
        ),
        vdom.create_element(
            'label',
            {
                halign = 'center',
                bottom = 720/2 - 20,
                color = '#fff',
                text = '[NEW GAME]',
                height = 24,
                on_click = props.start_game,
            }
        )
    )
end
