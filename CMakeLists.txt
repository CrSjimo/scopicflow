cmake_minimum_required(VERSION 3.17)

project(ScopicFlow VERSION 0.0.1.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# ----------------------------------
# Build Options
# ----------------------------------
option(SCOPIC_FLOW_BUILD_STATIC "Build static libraries" OFF)
option(SCOPIC_FLOW_BUILD_TRANSLATIONS "Build translations" ON)
option(SCOPIC_FLOW_BUILD_TESTS "Build test cases" OFF)
option(SCOPIC_FLOW_BUILD_DOCUMENTATIONS "Build documentations" OFF)
option(SCOPIC_FLOW_INSTALL "Install library" ON)

# ----------------------------------
# CMake Settings
# ----------------------------------
if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /manifest:no")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "d")
    endif()
endif()

if(SCOPIC_FLOW_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
endif()

# ----------------------------------
# Project Variables
# ----------------------------------
set(SCOPIC_FLOW_VERSION ${PROJECT_VERSION})
set(SCOPIC_FLOW_INSTALL_NAME ${PROJECT_NAME})
set(SCOPIC_FLOW_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ----------------------------------
# Find basic dependencies
# ----------------------------------
find_package(qmsetup REQUIRED)

qm_import(Filesystem)

if (NOT DEFINED QMSETUP_BUILD_DIR)
    set(QMSETUP_BUILD_DIR "${CMAKE_BINARY_DIR}/out")
endif()

qm_init_directories()

# ----------------------------------
# Main Project
# ----------------------------------
add_subdirectory(src)

if(SCOPIC_FLOW_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# ----------------------------------
# Documentation
# ----------------------------------
if(SCOPIC_FLOW_BUILD_DOCUMENTATIONS)
    if(NOT DOXYGEN_EXECUTABLE)
        find_package(Doxygen REQUIRED)
    endif()

    set(_install_options)

    if(SCOPIC_FLOW_INSTALL)
        set(_install_options INSTALL_DIR share/doc/${SCOPIC_FLOW_INSTALL_NAME})
    endif()

    set(SCOPIC_FLOW_DOXYGEN_TAGFILE ${CMAKE_BUILD_SHARE_DIR}/doc/${SCOPIC_FLOW_INSTALL_NAME}/scopicflow_tagfile.xml
        CACHE FILEPATH "scopicflow doxygen tag file path" FORCE
    )

    set(_doc_targets ScopicFlow)

    qm_import(Doxygen)
    qm_setup_doxygen(ScopicFlow_RunDoxygen
        NAME "Scopic Flow"
        DESCRIPTION "A GUI library of SVS edit views"
        MDFILE README.md
        OUTPUT_DIR ${CMAKE_BUILD_SHARE_DIR}/doc/${SCOPIC_FLOW_INSTALL_NAME}
        INPUT src doc
        TARGETS ${_doc_targets}
        DEPENDS ${_doc_targets}
        NO_EXPAND_MACROS Q_OBJECT Q_GADGET Q_DECLARE_TR_FUNCTIONS
        COMPILE_DEFINITIONS Q_SIGNALS=Q_SIGNALS Q_SLOTS=Q_SLOTS
        GENERATE_TAGFILE "${SCOPIC_FLOW_DOXYGEN_TAGFILE}"
        ${_install_options}
    )
endif()