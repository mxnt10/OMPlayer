cmake_minimum_required(VERSION 3.20)
project(OMPlayer VERSION 2.0)


# Para adicionar módulos extras
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Deps_File)


# Se precisar está aqui. Verifica a versão do gcc.
if (CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    string(REPLACE "\n" "" GCC_VERSION ${GCC_VERSION})
    message(STATUS "Found GCC version: " ${GCC_VERSION})
endif ()


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
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
message(STATUS "Current Directory: " ${PROJECT_SOURCE_DIR})


# Habilitando thread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)


# Suporte a notificações
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OTHER_LIBS REQUIRED libnotify)
endif ()


# Buscando sources do QT5
find_package(Qt5 COMPONENTS Core DBus Gui Quick Widgets REQUIRED)


# Adicionando sources manualmente
include_directories(
        /usr/include/qt5/QtAV
        /usr/include/qt5/QtAVWidgets
        ${PROJECT_SOURCE_DIR}/common/MediaInfoLib/Source/MediaInfoDLL
        ${PROJECT_SOURCE_DIR}/common/ScreenSaver
        ${PROJECT_SOURCE_DIR}/src/PlayListUtils
        ${PROJECT_SOURCE_DIR}/src/PropertyEditor
        ${PROJECT_SOURCE_DIR}/src/Widgets
        ${PROJECT_SOURCE_DIR}/src/Utils
)


# Adicionando recursos ao projeto
add_subdirectory(common/SingleApplication)
add_subdirectory(common/MediaInfoLib/Project/CMake common/MediaInfoLib/Source/MediaInfoDLL)


# Adicionando sources de screensaver
file(GLOB POWER_SAVING ${PROJECT_SOURCE_DIR}/common/ScreenSaver/powersaving*)
update_deps_file(${POWER_SAVING})


# Verificando plataforma
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SRC_SCREENSAVER ${POWER_SAVING}
            ${PROJECT_SOURCE_DIR}/common/ScreenSaver/winscreensaver.cpp
            ${PROJECT_SOURCE_DIR}/common/ScreenSaver/winscreensaver.h
        )
else ()
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(SRC_SCREENSAVER ${POWER_SAVING}
                ${PROJECT_SOURCE_DIR}/common/ScreenSaver/screensaver.cpp
                ${PROJECT_SOURCE_DIR}/common/ScreenSaver/screensaver.h
            )
    endif ()
endif ()


# Buscando fontes do programa em src
file(GLOB SRC_FILES LIST_DIRECTORIES FALSE ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.cpp)
update_deps_file(${SRC_FILES})


# Buscando utilitários da playlist
file(GLOB PLS_FILES ${PROJECT_SOURCE_DIR}/src/PlayListUtils/*.h ${PROJECT_SOURCE_DIR}/src/PlayListUtils/*.cpp)
update_deps_file(${PLS_FILES})


# Buscando widgets do programa
file(GLOB WIDGETS_FILES ${PROJECT_SOURCE_DIR}/src/Widgets/*.h ${PROJECT_SOURCE_DIR}/src/Widgets/*.cpp)
update_deps_file(${WIDGETS_FILES})


# Criando o executável do programa
add_executable(
        OMPlayer
        ${SRC_FILES} ${PLS_FILES} ${WIDGETS_FILES} ${SRC_SCREENSAVER}
        ${PROJECT_SOURCE_DIR}/src/PropertyEditor/PropertyEditor.cpp
        ${PROJECT_SOURCE_DIR}/src/PropertyEditor/PropertyEditor.h
        ${PROJECT_SOURCE_DIR}/src/Utils/JsonTools.cpp
        ${PROJECT_SOURCE_DIR}/src/Utils/JsonTools.h
        ${PROJECT_SOURCE_DIR}/src/Utils/Utils.cpp
        ${PROJECT_SOURCE_DIR}/src/Utils/Utils.h
        $<$<PLATFORM_ID:Linux>:${PROJECT_SOURCE_DIR}/src/Utils/Notify.h>
)


# Verificando Ativação do LTO
if (supported)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET OMPlayer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif ()


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
        $<$<PLATFORM_ID:Linux>:${OTHER_LIBS_LIBRARIES}>
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_include_directories(OMPlayer PRIVATE ${OTHER_LIBS_INCLUDE_DIRS})
endif ()
