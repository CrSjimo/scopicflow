project(ScopicFlowCore VERSION 0.0.1 LANGUAGES CXX)

qm_import(Qml)

file(GLOB_RECURSE _src *.h *.cpp *.qrc)

scopic_flow_add_library(ScopicFlowCore AUTOGEN
    SOURCES ${_src}
    LINKS svscraft::Core svscraft::Gui svscraft::Qml svscraft::Quick
    QT_LINKS Core Gui Widgets Qml Quick QuickControls2
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick QuickControls2
    INCLUDE_PRIVATE *
)

find_path(INTERVAL_TREE_INCLUDE_DIRS "interval-tree/interval_tree.hpp" REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${INTERVAL_TREE_INCLUDE_DIRS}>"
)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(${PROJECT_NAME}
    URI dev.sjimo.ScopicFlow
    QML_FILES ${_qml_files}
    RESOURCES ${_resource_files}
    NAMESPACE sflow
    OUTPUT_DIRECTORY ${QMSETUP_BUILD_DIR}/qml/dev/sjimo/ScopicFlow
)

qm_install_qml_modules(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
