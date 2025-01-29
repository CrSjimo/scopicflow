qm_import(Preprocess)

set(SCOPIC_FLOW_PROJECT_DESCRIPTION "A GUI library of SVS edit views")
set(SCOPIC_FLOW_PROJECT_COPYRIGHT "Copyright 2024 CrSjimo")
set(SCOPIC_FLOW_GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../include)
set(SCOPIC_FLOW_BUILD_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../etc/include)

# ----------------------------------
# Configurations
# ----------------------------------

# ----------------------------------
# CMake API
# ----------------------------------
macro(scopic_flow_add_library _target)
    set(options AUTOGEN NO_SYNC_INCLUDE NO_WIN_RC STATIC)
    set(oneValueArgs SYNC_INCLUDE_PREFIX PREFIX)
    set(multiValueArgs SYNC_INCLUDE_OPTIONS)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(FUNC_AUTOGEN)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTOUIC ON)
        set(CMAKE_AUTORCC ON)
    endif()

    if(FUNC_STATIC OR SCOPIC_FLOW_BUILD_STATIC)
        set(_type STATIC)
    else()
        set(_type SHARED)
    endif()

    add_library(${_target} ${_type})

    if(WIN32 AND NOT FUNC_NO_WIN_RC AND(${_type} STREQUAL "SHARED"))
        qm_add_win_rc(${_target}
            NAME ${SCOPIC_FLOW_INSTALL_NAME}
            DESCRIPTION ${SCOPIC_FLOW_PROJECT_DESCRIPTION}
            COPYRIGHT ${SCOPIC_FLOW_PROJECT_COPYRIGHT}
        )
    endif()

    if(FUNC_PREFIX)
        set(_prefix_option PREFIX ${FUNC_PREFIX})
    else()
        set(_prefix_option)
    endif()

    # Set global definitions
    qm_export_defines(${_target} ${_prefix_option})

    # Configure target
    qm_configure_target(${_target} ${FUNC_UNPARSED_ARGUMENTS})

    # Add include directories
    target_include_directories(${_target} PRIVATE ${SCOPIC_FLOW_BUILD_INCLUDE_DIR})
    target_include_directories(${_target} PRIVATE .)

    if(FUNC_SYNC_INCLUDE_PREFIX)
        set(_inc_name ${FUNC_SYNC_INCLUDE_PREFIX})
    else()
        set(_inc_name ${_target})
    endif()

    set(_install_options)

    if(SCOPIC_FLOW_INSTALL)
        install(TARGETS ${_target}
            EXPORT ${SCOPIC_FLOW_INSTALL_NAME}Targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
        )

        target_include_directories(${_target} PUBLIC
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${SCOPIC_FLOW_INSTALL_NAME}>"
        )

        set(_install_options
            INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${SCOPIC_FLOW_INSTALL_NAME}/${_inc_name}"
        )
    endif()

    if(NOT FUNC_NO_SYNC_INCLUDE)
        # Generate a standard include directory in build directory
        qm_sync_include(. "${SCOPIC_FLOW_GENERATED_INCLUDE_DIR}/${_inc_name}" ${_install_options} FORCE
            ${FUNC_SYNC_INCLUDE_OPTIONS}
        )
        target_include_directories(${_target} PUBLIC
            "$<BUILD_INTERFACE:${SCOPIC_FLOW_GENERATED_INCLUDE_DIR}>"
        )
    endif()
endmacro()

# ----------------------------------
# Main Project
# ----------------------------------

add_subdirectory(internal)
add_subdirectory(component)
add_subdirectory(style)
add_subdirectory(viewmodel)

if(SCOPIC_FLOW_INSTALL)
    # Add install target
    set(_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${SCOPIC_FLOW_INSTALL_NAME})

    # Add version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${SCOPIC_FLOW_INSTALL_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Add configuration file
    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/${SCOPIC_FLOW_INSTALL_NAME}Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${SCOPIC_FLOW_INSTALL_NAME}Config.cmake"
        INSTALL_DESTINATION ${_install_dir}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    # Install cmake files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${SCOPIC_FLOW_INSTALL_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${SCOPIC_FLOW_INSTALL_NAME}ConfigVersion.cmake"
        DESTINATION ${_install_dir}
    )

    # Install cmake targets files
    install(EXPORT ${SCOPIC_FLOW_INSTALL_NAME}Targets
        FILE "${SCOPIC_FLOW_INSTALL_NAME}Targets.cmake"
        NAMESPACE ScopicFlow::
        DESTINATION ${_install_dir}
    )
endif()