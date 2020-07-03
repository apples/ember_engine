local vdom = require('vdom')

local all_states = require('gui.all_states')

-- state_element: vdom.element
-- version: string
-- fps: string
local function root_component(props)
    local state_element = props.state_element;

    return vdom.create_element('widget', { width='100%', height='100%' },
        state_element,
        vdom.create_element(all_states, { version=props.version, fps=props.fps })
    )
end

local vdom_root = vdom.render(vdom.create_element(root_component, gui_state), root_widget)

function update_gui_state(state_element)
    vdom.render(
        vdom.create_element(
            root_component,
            {
                state_element=state_element,
                version=gui_state.version,
                fps=gui_state.fps
            }
        ),
        root_widget,
        vdom_root)

    vdom.flush_updates()
end
