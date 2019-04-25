cmake_minimum_required(VERSION 3.1)
project(qt_monitoring)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOOUIC ON)

find_package(OpenCV REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  hl_monitoring
  )

find_package(Qt5 REQUIRED COMPONENTS MultimediaWidgets)

#Enable C++17
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

set(DELEGATE_INCLUDE_DIRS
  src
  ${OpenCV_DIR}
)

set(DELEGATE_LIBRARIES
  ${OpenCV_LIBS}
  )

catkin_package(
  INCLUDE_DIRS ${DELEGATE_INCLUDE_DIRS}
  LIBRARIES ${PROJECT_NAME} ${DELEGATE_LIBRARIES}
  )

include_directories(
  ${DELEGATE_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
  ${Qt5MultimediaWidgets_INCLUDE_DIRS}
  )

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

set(LINKED_LIBRARIES ${catkin_LIBRARIES} ${DELEGATE_LIBRARIES})

add_library (${PROJECT_NAME} SHARED ${ALL_SOURCES})
qt5_use_modules(${PROJECT_NAME} MultimediaWidgets)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

add_executable(qt_monitor apps/qt_monitor.cpp)
target_link_libraries(qt_monitor ${PROJECT_NAME} ${catkin_LIBRARIES})
