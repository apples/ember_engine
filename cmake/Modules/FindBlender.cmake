# - Find blender
# This module attempts to find Blender software
# FindBlender provides the following variables:
#
# BLENDER_FOUND                 - has Blender been found
# BLENDER_EXECUTABLE            - Path to Blender executable
# BLENDER_VERSION               - Version of Blender

find_program(BLENDER_EXECUTABLE blender
    PATHS "C:\\Program Files\\Blender Foundation\\Blender 2.82\\")

if(BLENDER_EXECUTABLE)
# Get the python path and version from blender
    execute_process(COMMAND ${BLENDER_EXECUTABLE} --version
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE  ERROR)

    if(RESULT MATCHES 0)
        string(REGEX MATCH "Blender ([0-9]+\\.[0-9]+)" BLENDER_VERSION ${OUTPUT})
        set(BLENDER_VERSION ${CMAKE_MATCH_1})
    elseif(NOT blender_FIND_QUIETLY)
        message(WARNING "Blender config failure:\n${ERROR_VALUE}")
    endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(BLENDER
    REQUIRED_VARS
        BLENDER_EXECUTABLE
        BLENDER_VERSION
    VERSION_VAR BLENDER_VERSION
    HANDLE_COMPONENTS)