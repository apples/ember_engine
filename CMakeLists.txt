cmake_minimum_required(VERSION 3.12)
project(Ember)

set(EMBER_WASM_ENABLE_EXCEPTIONS ON CACHE BOOL "Enable exceptions for WASM builds")
set(EMBER_DATA_DIR "${CMAKE_SOURCE_DIR}/data" CACHE PATH "Data Directory")
set(EMBER_DATA_SRC "${CMAKE_SOURCE_DIR}/data_src" CACHE PATH "Data Source Directory")
set(EMBER_DATA_DST "${CMAKE_BINARY_DIR}/data" CACHE PATH "Data Output Directory")
set(BLENDER_EXPORT_PY "${CMAKE_SOURCE_DIR}/blender-scripts/export.py" CACHE PATH "Blender export script")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable exceptions globally
if(EMSCRIPTEN AND EMBER_WASM_ENABLE_EXCEPTIONS)
    add_compile_options(
        "-fexceptions"
        "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
    )
endif()

add_subdirectory(ext/ginseng)
add_subdirectory(ext/lua)
add_subdirectory(ext/sol2)

target_compile_definitions(sol2 INTERFACE SOL_CXX17_FEATURES=1)

if(EMSCRIPTEN)
    find_package(PythonInterp 2 REQUIRED)
    find_package(Blender 2.80 REQUIRED)

    set(EMBER_WWW_DIR "${CMAKE_BINARY_DIR}/www" CACHE PATH "Output Directory")

    include(BlenderExports)

    add_subdirectory(ext/glm)
    add_subdirectory(ext/lodepng)
    add_subdirectory(ext/sushi)
    add_subdirectory(ext/msdfgen)
    add_subdirectory(ext/soloud)

    # Static Files
    file(GLOB_RECURSE EMBER_STATIC CONFIGURE_DEPENDS static/*)
    add_custom_target(ember_static
        COMMENT "Copying static files"
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/static "${EMBER_WWW_DIR}"
        SOURCES ${EMBER_STATIC})

    # Export Blender Models
    set(EMBER_MODEL_OUTPUTS)
    file(GLOB_RECURSE EMBER_BLENDS CONFIGURE_DEPENDS "${EMBER_DATA_SRC}/models/*.blend")

    foreach(BLEND_FILE ${EMBER_BLENDS})
        get_filename_component(BLEND_FILE_NAME ${BLEND_FILE} NAME_WLE)
        if(${BLEND_FILE_NAME} MATCHES "^_")
            continue()
        endif()
        blender_export_file(
            OUT
            "${BLEND_FILE}"
            iqm
            "${EMBER_DATA_SRC}/models"
            "${EMBER_DATA_DST}/models"
            "${EMBER_DATA_DST}/textures")
        list(APPEND EMBER_MODEL_OUTPUTS ${OUT})
    endforeach()

    # Static Data Files
    file(GLOB_RECURSE EMBER_DATA_FILES CONFIGURE_DEPENDS ${EMBER_DATA_DIR}/*)
    list(APPEND EMBER_DATA_FILES ${EMBER_MODEL_OUTPUTS})
    set(FILE_PACKAGER $ENV{EMSDK}/upstream/emscripten/tools/file_packager.py)
    set(EMBER_DATA_FILE ${EMBER_WWW_DIR}/ember_game.data)
    set(EMBER_DATA_LOADER ${EMBER_WWW_DIR}/ember_game.data.js)
    add_custom_command(
        OUTPUT ${EMBER_DATA_FILE} ${EMBER_DATA_LOADER}
        COMMAND "${PYTHON_EXECUTABLE}"
            "${FILE_PACKAGER}"
            "${EMBER_DATA_FILE}"
            --preload
                "${EMBER_DATA_DIR}@data"
                "${EMBER_DATA_DST}@data"
            "--js-output=${EMBER_DATA_LOADER}"
        COMMENT "Packaging data files"
        DEPENDS ${EMBER_DATA_FILES})
    add_custom_target(ember_data
        SOURCES ${EMBER_DATA_FILES}
        DEPENDS ${EMBER_DATA_FILE} ${EMBER_DATA_LOADER})

    # Emscripten Ports
    string(CONCAT EMSCRIPTEN_PORTS_FLAGS
        " -s USE_SDL=2"
        " -s USE_SDL_IMAGE=2"
        " -s USE_SDL_NET=2"
        " -s USE_FREETYPE=1")

    # Client JS
    file(GLOB_RECURSE EMBER_JS "src/*.js")

    # Client C++
    file(GLOB_RECURSE EMBER_SRCS CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
    add_executable(ember_game ${EMBER_SRCS} ${EMBER_JS})
    string(CONCAT EMBER_LINK_FLAGS
        " -s TOTAL_MEMORY=33554432"
        " -s DISABLE_EXCEPTION_CATCHING=0"
        " -s FORCE_FILESYSTEM=1"
        "${EMSCRIPTEN_PORTS_FLAGS}")
    string(CONCAT EMBER_LINK_FLAGS_DEBUG
        " -g4"
        " -s ASSERTIONS=1"
        " --source-map-base file://${CMAKE_BINARY_DIR}/")
    set_target_properties(ember_game PROPERTIES
        SUFFIX .js
        COMPILE_FLAGS "${EMBER_COMPILE_FLAGS}"
        LINK_FLAGS "${EMBER_LINK_FLAGS}"
        LINK_FLAGS_DEBUG "${EMBER_LINK_FLAGS_DEBUG}"
        RUNTIME_OUTPUT_DIRECTORY "${EMBER_WWW_DIR}")
    target_compile_options(ember_game PRIVATE "-std=c++17" "SHELL:${EMSCRIPTEN_PORTS_FLAGS}" "$<$<CONFIG:DEBUG>:-O1>")
    em_link_js_library(ember_game ${EMBER_JS})
    target_link_libraries(ember_game
        ginseng
        sushi
        sol2
        msdfgen
        soloud)
    add_dependencies(ember_game ember_static ember_data)
else()
    message(FATAL_ERROR "You're on your own for this one")
endif()
