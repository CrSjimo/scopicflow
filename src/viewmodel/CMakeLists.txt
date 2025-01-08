file(GLOB_RECURSE _src *.h *.cpp *.qrc)

scopic_flow_add_library(ScopicFlowViewModel AUTOGEN
    SOURCES ${_src}
    SYNC_INCLUDE_PREFIX ScopicFlow
    LINKS svscraft::Core
    QT_LINKS Core Gui Widgets Qml Quick
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick
)

find_path(INTERVAL_TREE_INCLUDE_DIRS "interval-tree/interval_tree.hpp" REQUIRED)

target_include_directories(ScopicFlowViewModel PUBLIC
    "$<BUILD_INTERFACE:${INTERVAL_TREE_INCLUDE_DIRS}>"
)
