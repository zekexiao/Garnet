cmake_minimum_required(VERSION 3.24)
project(Garnet)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(GARNET_BUILD_TESTS "Build tests" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

if (NOT MRUBY_FOUND)
    if (NOT MRUBY_ROOT)
        include_directories(/opt/homebrew/opt/mruby/include)
        link_directories(/opt/homebrew/opt/mruby/lib)
    else ()
        include_directories(${MRUBY_ROOT}/include)
        link_directories(${MRUBY_ROOT}/lib)
    endif ()
endif ()

add_library(Garnet SHARED
        garnet/src/bridgecall.h garnet/src/bridgecall.cpp
        garnet/src/bridgeclass.h garnet/src/bridgeclass.cpp
        garnet/src/conversion.h garnet/src/conversion.cpp
        garnet/src/engine.h garnet/src/engine.cpp
        garnet/src/garnetlib_global.h
        garnet/src/utils.h garnet/src/utils.cpp
        garnet/src/variadicargument.h)

set_property(TARGET Garnet PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_include_directories(Garnet PUBLIC garnet/include)
target_link_libraries(Garnet PRIVATE Qt${QT_VERSION_MAJOR}::Core mruby)

if (GARNET_BUILD_TESTS)
    enable_testing(true)
    find_package(Qt NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Qml)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Qml)
    qt_add_resources(GarnetTestResources test/test.qrc)
    qt_add_executable(GarnetTest ${GarnetTestResources}
            test/test.cpp
            test/testgarnetbridgeclass.h test/testgarnetbridgeclass.cpp
            test/testgarnetengine.h test/testgarnetengine.cpp
            test/testgarnetvalue.h test/testgarnetvalue.cpp)

    add_test(NAME GarnetTest COMMAND GarnetTest)
    target_link_libraries(GarnetTest PRIVATE Garnet Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Qml)
endif ()
