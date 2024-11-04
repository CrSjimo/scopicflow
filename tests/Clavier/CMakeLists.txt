project(tst_ScopicFlow_Clavier)

set(CMAKE_AUTORCC ON)

file(GLOB _src *.h *.cpp *.qrc)

add_executable(${PROJECT_NAME} ${_src})

qm_configure_target(${PROJECT_NAME}
    LINKS ScopicFlow svscraft::Core
    QT_LINKS Core Gui Widgets Qml Quick QuickWidgets
)