cmake_minimum_required(VERSION 3.5)

project(read_data LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.


set(GLOB CLASS_SOURCES
    classes/.*)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(read_data
                    classes/)

add_executable(read_data
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    myglwidget.cpp
    myglwidget.h
    ${CLASS_SOURCES}
)


target_link_libraries(read_data PRIVATE Qt5::Widgets)
