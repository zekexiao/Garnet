cmake_minimum_required(VERSION 4.0)
project(Garnet)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(GARNET_BUILD_TESTS "Build tests" ON)

find_package(Qt6 COMPONENTS Core Test Qml)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Test Qml)
endif ()

include_directories(/opt/homebrew/opt/mruby/include)
link_directories(/opt/homebrew/opt/mruby/lib)
enable_testing(true)

set(MRUBY_LIBRARIES mruby)
add_library(Garnet SHARED
        garnet/src/bridgecall.h garnet/src/bridgecall.cpp
        garnet/src/bridgeclass.h garnet/src/bridgeclass.cpp
        garnet/src/conversion.h garnet/src/conversion.cpp
        garnet/src/engine.h garnet/src/engine.cpp
        garnet/src/garnetlib_global.h
        garnet/src/utils.h garnet/src/utils.cpp
        garnet/src/variadicargument.h)

target_include_directories(Garnet PUBLIC garnet/include)
target_link_libraries(Garnet PRIVATE Qt::Core ${MRUBY_LIBRARIES})

if (GARNET_BUILD_TESTS)
    qt_add_resources(GarnetTestResources test/test.qrc)
    qt_add_executable(GarnetTest ${GarnetTestResources}
            test/test.cpp
            test/testgarnetbridgeclass.h test/testgarnetbridgeclass.cpp
            test/testgarnetengine.h test/testgarnetengine.cpp
            test/testgarnetvalue.h test/testgarnetvalue.cpp)

    add_test(NAME GarnetTest COMMAND GarnetTest)
    target_link_libraries(GarnetTest PRIVATE Garnet Qt::Test Qt::Qml)
endif ()
