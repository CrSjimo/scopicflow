project(tst_ScopicFlow_clippane)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

file(GLOB _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.qml *.js *.mjs)

add_executable(${PROJECT_NAME} ${_src})

qm_configure_target(${PROJECT_NAME}
    LINKS svscraft::Core svscraft::Quick ScopicFlowCore ScopicFlowCore ScopicFlowViews
    QT_LINKS Core Gui Widgets Qml Quick QuickWidgets QuickControls2
)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(${PROJECT_NAME}
    URI dev.sjimo.ScopicFlow.Test.ClipPane
    QML_FILES ${_qml_files}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)