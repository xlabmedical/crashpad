cmake_minimum_required(VERSION 3.12)
project(
  "xQt" VERSION 0.0.0
  DESCRIPTION "Qt extensions for xlabMedical"
  HOMEPAGE_URL "https://github.com/xlabmedical/RealGuide/xQt"
  LANGUAGES CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
  # sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/callback_dispatcher_internal.cpp
  # headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/callback_dispatcher.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/callback_dispatcher_internal.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/callback_event.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/callback_respondable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/event_dispatcher.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/post_callback.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/xQt/send_callback.hpp
  )
# Allow linking when xQt is a subproject
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets Qt5::Core)

