cmake_minimum_required(VERSION 3.21)
project(OMPlayer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

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
)

add_subdirectory(
        ./common/SingleApplication
)

add_executable(
        OMPlayer
        src/main.cpp
        src/Button.cpp   src/Button.h
        src/Defines.cpp  src/Defines.h
        src/ListView.cpp src/ListView.h
        src/Player.cpp   src/Player.h
        src/Receiver.cpp src/Receiver.h
        src/Slider.cpp   src/Slider.h
        src/Utils.cpp    src/Utils.h
        src/Widget.cpp   src/Widget.h

        src/PlayList.cpp         src/PlayList.h
        src/PlayListDelegate.cpp src/PlayListDelegate.h
        src/PlayListItem.cpp     src/PlayListItem.h
        src/PlayListModel.cpp    src/PlayListModel.h
)

target_link_libraries(
        OMPlayer
        SingleApplication::SingleApplication
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        QtAV
        QtAVWidgets
        X11
        Xtst
        mediainfo
        dl
        z
)
