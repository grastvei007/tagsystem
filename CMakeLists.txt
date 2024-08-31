cmake_minimum_required(VERSION 3.16)
project(tagsystem)

#include_directories($ENV{JUNE_ROOT})
#link_directories($ENV{DEV_LIBS})

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Xml REQUIRED)
find_package(Qt6 COMPONENTS WebSockets REQUIRED)

qt_add_library(tagsystem SHARED
    serverconnectwidget.ui
    tagselectview.ui
    tagsocketlistview.ui

    serverconnectwidget.cpp
    tag.cpp
    taglist.cpp
    taglistsortfilterproxymodel.cpp
    taglistview.cpp
    taglisttablemodel.cpp
    tagselectview.cpp
    tagsocket.cpp
    tagsocketlist.cpp
    tagsocketlisttablemodel.cpp
    tagsocketlistview.cpp
    clientinformation.cpp

    serverconnectwidget.h
    taglistsortfilterproxymodel.h
    tagsystem_global.h
    tag.h
    taglist.h
    taglistview.h
    taglisttablemodel.h
    tagselectview.h
    tagsocket.h
    tagsocketlist.h
    tagsocketlisttablemodel.h
    tagsocketlistview.h
    clientinformation.h
)

target_link_libraries(tagsystem Qt6::Core)
target_link_libraries(tagsystem Qt6::Widgets)
target_link_libraries(tagsystem Qt6::Xml)
target_link_libraries(tagsystem Qt6::WebSockets)

target_include_directories(tagsystem INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET tagsystem
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tagsystem> $ENV{DEV_LIBS} )


