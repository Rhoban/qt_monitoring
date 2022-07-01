cmake_minimum_required(VERSION 3.1)
project(qt_monitoring)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOOUIC ON)

find_package(OpenCV REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS MultimediaWidgets)

#Enable C++17
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

set(SOURCES_DIRECTORIES
  src/qt_monitoring
  )

  #Include Sources sub sources
foreach (DIRECTORY ${SOURCES_DIRECTORIES})
  include (${DIRECTORY}/sources.cmake)
  set (PREFIXED_SOURCES)
  foreach (SOURCE ${SOURCES})
    set (PREFIXED_SOURCES ${PREFIXED_SOURCES} ${DIRECTORY}/${SOURCE})
  endforeach (SOURCE)
  set (ALL_SOURCES ${ALL_SOURCES} ${PREFIXED_SOURCES})
endforeach (DIRECTORY)

add_library (${PROJECT_NAME} SHARED ${ALL_SOURCES})
qt5_use_modules(${PROJECT_NAME} MultimediaWidgets)
target_link_libraries(${PROJECT_NAME} hl_monitoring rhoban_team_play)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src
)

add_executable(qt_monitor apps/qt_monitor.cpp)
target_link_libraries(qt_monitor ${PROJECT_NAME})
