project(tst_ScopicFlow_SequenceSlicer)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

file(GLOB _src *.h *.cpp *.qrc)
file(GLOB_RECURSE _qml_files_abs *.qml *.js *.mjs)
set(_qml_files)
foreach(_file IN LISTS _qml_files_abs)
    file(RELATIVE_PATH _rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    list(APPEND _qml_files ${_rel_path})
endforeach()

add_executable(${PROJECT_NAME} ${_src})

qm_configure_target(${PROJECT_NAME}
    LINKS svscraft::Core ScopicFlow ScopicFlowplugin
    QT_LINKS Core Gui Widgets Qml Quick QuickWidgets
)

qt_add_qml_module(${PROJECT_NAME}
    URI dev.sjimo.ScopicFlow.Test.SequenceSlicer
    QML_FILES ${_qml_files}
)