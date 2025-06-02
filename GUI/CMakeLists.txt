cmake_minimum_required(VERSION 3.25)
project(main LANGUAGES CXX C ASM)

set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS 
    Widgets 
    SerialPort
    Charts
)
qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    Widget/ValueEdit/valueEdit.cpp
    Widget/TopBar/topBar.cpp
    Widget/AddChartWindow/addChartWindow.cpp
    Serial/serial_ui.cpp
    Serial/serial.cpp
    

    Widget/Charts/chart.cpp
    Widget/Charts/Digital/digitalChart.cpp
    Widget/Charts/Analog/analogChart.cpp
)

include_directories(${PROJECT_NAME}
    .
    Widget/ValueEdit
    Widget/TopBar
    Widget/AddChartWindow
    Serial

    Widget/Charts
    Widget/Charts/Digital
    Widget/Charts/Analog
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)
