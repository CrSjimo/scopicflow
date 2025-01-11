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
    LINKS svscraft::Core ScopicFlowViewModel ScopicFlowPalette ScopicFlowPaletteplugin
    QT_LINKS Core Gui Widgets Qml Quick
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick
)

qt_add_qml_module(ScopicFlowInternal
    URI dev.sjimo.ScopicFlow.Internal
    QML_FILES ${_qml_files}
    RESOURCES ${_resource_files}
)

find_path(INTERVAL_TREE_INCLUDE_DIRS "interval-tree/interval_tree.hpp" REQUIRED)

target_include_directories(ScopicFlowInternal PUBLIC
    "$<BUILD_INTERFACE:${INTERVAL_TREE_INCLUDE_DIRS}>"
)