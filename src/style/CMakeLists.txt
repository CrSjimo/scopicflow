file(GLOB_RECURSE _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files_abs *.qml *.js *.mjs)
set(_qml_files)
foreach(_file IN LISTS _qml_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _qml_files ${_rel_path})
endforeach()
foreach(_file IN LISTS _qml_files)
    file(STRINGS ${_file} _file_content LIMIT_COUNT 1 REGEX "pragma Singleton")

    if(_file_content)
        set_source_files_properties(${_file}
                PROPERTIES
                QT_QML_SINGLETON_TYPE TRUE)
    endif()
endforeach()

scopic_flow_add_library(ScopicFlowStyle AUTOGEN STATIC
    SOURCES ${_src}
    LINKS svscraft::Core
    QT_LINKS Core Gui Widgets Qml Quick
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick
)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(ScopicFlowStyle
    URI dev.sjimo.ScopicFlow.Style
    QML_FILES ${_qml_files}
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/sjimo/ScopicFlow/Style
)

set_target_properties(ScopicFlowStyle PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)