cmake_minimum_required(VERSION 3.20)
project(OMPlayer)


# Habilitando suporte LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)


# Definição de variáveis
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
find_package(Threads REQUIRED)


# Buscando sources do QT5
find_package(
        Qt5 COMPONENTS
        Core DBus Gui Quick Widgets
        REQUIRED
)


# Adicionando sources manualmente
include_directories(
        /usr/include/qt5/QtAV
        /usr/include/qt5/QtAVWidgets
        common/MediaInfoLib/Source/MediaInfoDLL
        common/ScreenSaver
        src/PlayListUtils
        src/Widgets
        src/Utils
)


# Adicionando recursos ao projeto
add_subdirectory( common/SingleApplication )
add_subdirectory( common/MediaInfoLib/Project/CMake common/MediaInfoLib/Source/MediaInfoDLL )


# Verificando compatibilidade de sistemas
if(WIN32)
    set(SRC_SCREENSAVER
        common/ScreenSaver/winscreensaver.cpp
        common/ScreenSaver/winscreensaver.h
    )
else()
    set(SRC_SCREENSAVER
        common/ScreenSaver/screensaver.cpp
        common/ScreenSaver/screensaver.h
    )
endif()


# Criando o executável do programa
add_executable(
        OMPlayer
        src/main.cpp
        src/About.cpp           src/About.h
        src/EventFilter.cpp     src/EventFilter.h
        src/MessageReceiver.cpp src/MessageReceiver.h
        src/Player.cpp          src/Player.h
        src/PlayList.cpp        src/PlayList.h
        src/Settings.cpp        src/Settings.h
        src/StatisticsView.cpp  src/StatisticsView.h
        src/Translator.cpp      src/Translator.h
        src/Worker.cpp          src/Worker.h

        src/PlayListUtils/PlayListDelegate.cpp
        src/PlayListUtils/PlayListDelegate.h
        src/PlayListUtils/PlayListItem.cpp
        src/PlayListUtils/PlayListItem.h
        src/PlayListUtils/PlayListModel.cpp
        src/PlayListUtils/PlayListModel.h

        src/Utils/JsonTools.cpp
        src/Utils/JsonTools.h
        src/Utils/Utils.cpp
        src/Utils/Utils.h

        src/Widgets/Button.cpp
        src/Widgets/Button.h
        src/Widgets/ClickableMenu.cpp
        src/Widgets/ClickableMenu.h
        src/Widgets/Label.cpp
        src/Widgets/Label.h
        src/Widgets/Slider.cpp
        src/Widgets/Slider.h
        src/Widgets/TreeView.cpp
        src/Widgets/TreeView.h

        common/ScreenSaver/powersaving.cpp
        common/ScreenSaver/powersaving.h
        ${SRC_SCREENSAVER}
)


# Verificando Ativação do LTO
if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET OMPlayer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()


# Linkando as bibliotecas
target_link_libraries(
        OMPlayer
        SingleApplication::SingleApplication
        Threads::Threads
        QtAV QtAVWidgets
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Quick
        Qt5::Widgets
        mediainfo dl
)
