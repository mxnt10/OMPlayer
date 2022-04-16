cmake_minimum_required(VERSION 3.21)
project(OMPlayer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(
        PythonLibs
        REQUIRED
)

find_package(
        Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED
)

include_directories(
        /usr/include/qt5/QtAV
        /usr/include/qt5/QtAVWidgets
        /usr/include/MediaInfo
        /usr/include/MediaInfoDLL
        ${PYTHON_INCLUDE_DIRS}
)

add_executable(
        OMPlayer
        src/main.cpp
        src/Player.cpp
        src/Player.h
        src/Slider.cpp
        src/Slider.h
        src/Utils.cpp
        src/Utils.h
        src/Button.cpp
        src/Button.h
        src/Widget.cpp
        src/Widget.h
        src/Defines.h
        src/Defines.cpp
        src/PlayList.cpp
        src/PlayList.h
        src/PlayListDelegate.cpp
        src/PlayListDelegate.h
        src/PlayListItem.cpp
        src/PlayListItem.h
        src/PlayListModel.cpp
        src/PlayListModel.h
        src/ListView.cpp
        src/ListView.h)

target_link_libraries(
        OMPlayer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        QtAV
        QtAVWidgets
        python3.9
        mediainfo
        dl
        z
)
