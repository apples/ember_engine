local vdom = require('vdom')
local version_stamp = require('gui.controls.version_stamp')
local fps_counter = require('gui.controls.fps_counter')

-- version: string
-- fps: string
return function(props)
    return vdom.create_element('widget', { width = '100%', height = '100%' },
        vdom.create_element(version_stamp, { version = props.version }),
        vdom.create_element(fps_counter, { fps = props.fps })
    )
end
