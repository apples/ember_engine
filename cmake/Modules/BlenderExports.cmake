
function(blender_export_file OUTPUT BLEND_FILE FILETYPE BASE_DIR MODEL_DIR TEXTURE_DIR)
    get_filename_component(BASE_NAME "${BLEND_FILE}" NAME_WLE)
    file(RELATIVE_PATH RELATIVE_PATH "${BASE_DIR}" "${BLEND_FILE}")
    string(REGEX REPLACE "[.]blend$" "" PATH_NAME "${RELATIVE_PATH}")

    set(MODEL_FILE ${MODEL_DIR}/${PATH_NAME}.${FILETYPE})
    set(TEXTURE_FILE ${TEXTURE_DIR}/${PATH_NAME}.png)

    add_custom_command(
        OUTPUT "${MODEL_FILE}" "${TEXTURE_FILE}"
        COMMAND "${BLENDER_EXECUTABLE}"
            "${BLEND_FILE}"
            --background
            --python-exit-code 1
            --python "${BLENDER_EXPORT_PY}"
            --
            "${BASE_NAME}"
            "${MODEL_FILE}"
            "${TEXTURE_FILE}"
        COMMENT "Exporting model ${BLEND_FILE} (${FILETYPE})"
        DEPENDS "${BLEND_FILE}" "${BLENDER_EXPORT_PY}")

    set(${OUTPUT} "${MODEL_FILE}" "${TEXTURE_FILE}" PARENT_SCOPE)
endfunction()
