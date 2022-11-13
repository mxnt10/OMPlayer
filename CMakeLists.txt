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
    pkg_check_modules(LIB_NOTIFY REQUIRED libnotify)
endif ()


# Buscando sources do Qt5 e QtAV
find_package(Qt5 COMPONENTS Core DBus Gui Quick Widgets REQUIRED)
find_package(QtAV REQUIRED)


# Adicionando sources manualmente
include_directories(
        ${QTAV_INCLUDE_DIRS} ${QTAVWIDGETS_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/common/MediaInfoLib/Source/MediaInfoDLL
        ${PROJECT_SOURCE_DIR}/common/ScreenSaver
        ${PROJECT_SOURCE_DIR}/src/PlayListUtils
        ${PROJECT_SOURCE_DIR}/src/Widgets
        ${PROJECT_SOURCE_DIR}/src/Utils
)


# Adicionando recursos ao projeto
add_subdirectory(${PROJECT_SOURCE_DIR}/common/SingleApplication)
add_subdirectory(
        ${PROJECT_SOURCE_DIR}/common/MediaInfoLib/Project/CMake
        ${PROJECT_SOURCE_DIR}/common/MediaInfoLib/Source/MediaInfoDLL
)


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
file(GLOB SRC_FILES LIST_DIRECTORIES FALSE ${PROJECT_SOURCE_DIR}/src/*.*)
update_deps_file(${SRC_FILES})


# Buscando utilitários da playlist
file(GLOB PLS_FILES ${PROJECT_SOURCE_DIR}/src/PlayListUtils/*.*)
update_deps_file(${PLS_FILES})


# Buscando widgets do programa
file(GLOB WIDGETS_FILES ${PROJECT_SOURCE_DIR}/src/Widgets/*.*)
update_deps_file(${WIDGETS_FILES})


# Buscando utilitários do programa
file(GLOB UTILS_FILES ${PROJECT_SOURCE_DIR}/src/Utils/*.*)
string(REGEX MATCHALL "${PROJECT_SOURCE_DIR}/src/Utils/[U|J|P][a-zA-Z]*[.][a-zA-Z]*" UTILS_FILES ${UTILS_FILES})
update_deps_file(${UTILS_FILES})


# Criando o executável do programa
add_executable(
        OMPlayer
        ${SRC_FILES} ${PLS_FILES} ${WIDGETS_FILES} ${UTILS_FILES} ${SRC_SCREENSAVER}
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
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Quick
        Qt5::Widgets
        mediainfo dl
        ${QTAV} ${QTAVWIDGETS_LIBRARIES}
        $<$<PLATFORM_ID:Linux>:${LIB_NOTIFY_LIBRARIES}>
)


# Incluindo diretório do libnotify
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_include_directories(OMPlayer PRIVATE ${LIB_NOTIFY_INCLUDE_DIRS})
endif ()
