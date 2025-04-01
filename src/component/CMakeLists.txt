file(GLOB_RECURSE _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files_abs *.qml *.js *.mjs)
set(_qml_files)
foreach(_file IN LISTS _qml_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _qml_files ${_rel_path})
endforeach()

scopic_flow_add_library(ScopicFlow AUTOGEN STATIC
    SOURCES ${_src}
    SYNC_INCLUDE_PREFIX ScopicFlowComponent
    LINKS svscraft::Core ScopicFlowInternal ScopicFlowInternalplugin
    QT_LINKS Core Gui Widgets Qml Quick
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick
)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(ScopicFlow
    URI dev.sjimo.ScopicFlow
    QML_FILES ${_qml_files}
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/sjimo/ScopicFlow
)

set_target_properties(ScopicFlow PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
