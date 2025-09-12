cmake_minimum_required(VERSION 3.16)

project(vision_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts Core Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts Core Network Sql)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        dbmanager.h
        dbmanager.cpp
)

file(GLOB DLG_SOURCES
    dlg/console/*.cpp
    dlg/console/*.h
    dlg/charts/*.cpp
    dlg/charts/*.h
    dlg/table/*.cpp
    dlg/table/*.h
    dlg/bar/*.cpp
    dlg/bar/*.h
    dlg/HeatMap/*.cpp
    dlg/HeatMap/*.h
    dlg/*.h
    dlg/*.cpp
    dlg/PacketSniffer/*.cpp
    dlg/PacketSniffer/*.h
    dlg/Topo/*.h
    dlg/Topo/*.cpp
    )


qt_add_executable(vision_client
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${DLG_SOURCES}
)

target_include_directories(vision_client PRIVATE 
                          "${PROJECT_SOURCE_DIR}/../library/include"
                          "${PROJECT_SOURCE_DIR}/../library/include/dlg")


target_link_libraries(vision_client PRIVATE 
                    Qt${QT_VERSION_MAJOR}::Widgets
                    Qt${QT_VERSION_MAJOR}::Charts
                    Qt${QT_VERSION_MAJOR}::Core
                    Qt${QT_VERSION_MAJOR}::Network
                    Qt${QT_VERSION_MAJOR}::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(vision_client PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


install(TARGETS vision_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(vision_client)
endif()


message(STATUS "Qt Version: ${Qt6_VERSION}")  
message(STATUS "QtCharts Included: ${Qt6Charts_FOUND}")  
