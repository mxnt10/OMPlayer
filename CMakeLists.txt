cmake_minimum_required(VERSION 3.21)
project(OMPlayer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
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
        common/ScreenSaver
)

add_subdirectory(
        common/SingleApplication
)

add_executable(
        OMPlayer
        src/main.cpp        src/XTool.h
        src/About.cpp       src/About.h
        src/Button.cpp      src/Button.h
        src/EventFilter.cpp src/EventFilter.h
        src/JsonTools.cpp   src/JsonTools.h
        src/Label.cpp       src/Label.h
        src/Player.cpp      src/Player.h
        src/Settings.cpp    src/Settings.h
        src/Slider.cpp      src/Slider.h
        src/Utils.cpp       src/Utils.h

        src/PlayList.cpp         src/PlayList.h
        src/PlayListDelegate.cpp src/PlayListDelegate.h
        src/PlayListItem.cpp     src/PlayListItem.h
        src/PlayListModel.cpp    src/PlayListModel.h

        common/ScreenSaver/ScreenSaver.cpp
        common/ScreenSaver/ScreenSaver.h
)

target_link_libraries(
        OMPlayer
        SingleApplication::SingleApplication
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        QtAV
        QtAVWidgets
        mediainfo
        dl
        X11
        Xtst
)
