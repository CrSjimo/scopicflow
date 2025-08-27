project(ScopicFlowViews VERSION 0.0.1 LANGUAGES CXX)

file(GLOB_RECURSE _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files_abs *.qml *.js *.mjs)
set(_qml_files)
foreach(_file IN LISTS _qml_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _qml_files ${_rel_path})
endforeach()

scopic_flow_add_library(${PROJECT_NAME} AUTOGEN STATIC
    SOURCES ${_src}
    LINKS svscraft::Core ScopicFlowCore ScopicFlowInternal ScopicFlowInternalplugin
    QT_LINKS Core Gui Widgets Qml Quick QuickControls2
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick QuickControls2
)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(${PROJECT_NAME}
    URI dev.sjimo.ScopicFlow.Views
    QML_FILES ${_qml_files}
    NAMESPACE sflow
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/sjimo/ScopicFlow/Views
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
