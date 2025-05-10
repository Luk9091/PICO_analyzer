cmake_minimum_required(VERSION 3.25)
project(main LANGUAGES CXX C ASM)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS 
    Widgets 
    SerialPort
    Charts
)
qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    Widget/Charts/Digital/digitalChart.cpp
    Widget/ValueEdit/valueEdit.cpp
    Widget/TopBar/topBar.cpp
    Serial/serial_ui.cpp
    Serial/serial.cpp
)

include_directories(${PROJECT_NAME}
    .
    Widget/Charts/Digital
    Widget/ValueEdit
    Widget/TopBar
    Serial
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)
