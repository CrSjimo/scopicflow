file(GLOB_RECURSE _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files_abs *.qml *.js *.mjs)
set(_qml_files)
foreach(_file IN LISTS _qml_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _qml_files ${_rel_path})
endforeach()
file(GLOB_RECURSE _resource_files_abs assets/*)
set(_resource_files)
foreach(_file IN LISTS _resource_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _resource_files ${_rel_path})
endforeach()

scopic_flow_add_library(ScopicFlowInternal AUTOGEN STATIC
    SOURCES ${_src}
    LINKS svscraft::Quick ScopicFlowViewModel ScopicFlowStyle ScopicFlowStyleplugin
    QT_LINKS Core Gui Widgets Qml Quick QuickControls2
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick QuickControls2
)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(ScopicFlowInternal
    URI dev.sjimo.ScopicFlow.Internal
    QML_FILES ${_qml_files}
    RESOURCES ${_resource_files}
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/sjimo/ScopicFlow/Internal
)

find_path(INTERVAL_TREE_INCLUDE_DIRS "interval-tree/interval_tree.hpp" REQUIRED)

target_include_directories(ScopicFlowInternal PUBLIC
    "$<BUILD_INTERFACE:${INTERVAL_TREE_INCLUDE_DIRS}>"
)

set_target_properties(ScopicFlowInternal PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)