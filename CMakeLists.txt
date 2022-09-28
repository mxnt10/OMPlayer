cmake_minimum_required(VERSION 3.20)
project(OMPlayer)

include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

find_package(
        Qt5 COMPONENTS
        Core
        DBus
        Gui
        Quick
        Widgets
        REQUIRED
)

include_directories(
        /usr/include/qt5/QtAV
        /usr/include/qt5/QtAVWidgets
        common/MediaInfoLib/Source/MediaInfoDLL
#        common/Visualization
        common/ScreenSaver
        src/PlayListUtils
)

add_subdirectory( common/SingleApplication )
add_subdirectory( common/MediaInfoLib/Project/CMake common/MediaInfoLib/Source/MediaInfoDLL )

add_executable(
        OMPlayer
        src/main.cpp
        src/About.cpp          src/About.h
        src/Button.cpp         src/Button.h
        src/ClickableMenu.cpp  src/ClickableMenu.h
        src/EventFilter.cpp    src/EventFilter.h
        src/JsonTools.cpp      src/JsonTools.h
        src/Label.cpp          src/Label.h
        src/Player.cpp         src/Player.h
        src/PlayList.cpp       src/PlayList.h
        src/Settings.cpp       src/Settings.h
        src/Slider.cpp         src/Slider.h
        src/StatisticsView.cpp src/StatisticsView.h
        src/Translator.cpp     src/Translator.h
        src/TreeView.cpp       src/TreeView.h
        src/Utils.cpp          src/Utils.h

        src/PlayListUtils/PlayListDelegate.cpp
        src/PlayListUtils/PlayListDelegate.h
        src/PlayListUtils/PlayListItem.cpp
        src/PlayListUtils/PlayListItem.h
        src/PlayListUtils/PlayListModel.cpp
        src/PlayListUtils/PlayListModel.h

        common/ScreenSaver/powersaving.cpp
        common/ScreenSaver/powersaving.h
        common/ScreenSaver/screensaver.cpp
        common/ScreenSaver/screensaver.h

#        common/Visualization/visualisationrenderer.cpp
#        common/Visualization/visualisationrenderer.h
)

target_link_libraries(
        OMPlayer
        SingleApplication::SingleApplication
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Quick
        Qt5::Widgets
        QtAV
        QtAVWidgets
        mediainfo
        dl
)
