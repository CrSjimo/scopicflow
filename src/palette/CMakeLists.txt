file(GLOB_RECURSE _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files_abs *.qml *.js *.mjs)
set(_qml_files)
foreach(_file IN LISTS _qml_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _qml_files ${_rel_path})
endforeach()

scopic_flow_add_library(ScopicFlowPalette AUTOGEN STATIC
    SOURCES ${_src}
    LINKS svscraft::Core
    QT_LINKS Core Gui Widgets Qml Quick
    QT_INCLUDE_PRIVATE Core Gui Widgets Qml Quick
)

qt_add_qml_module(ScopicFlowPalette
    URI dev.sjimo.ScopicFlow.Palette
    QML_FILES ${_qml_files}
)