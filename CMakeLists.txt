cmake_minimum_required(VERSION 3.16)

project(firewall VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 添加 WinDivert 的路径
set(WINDIVERT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/WinDivert/include)
set(WINDIVERT_LIB_DIR ${CMAKE_SOURCE_DIR}/WinDivert/lib)
set(WINDIVERT_SYS_DIR ${CMAKE_SOURCE_DIR}/WinDivert/sys)
set(WINDIVERT_DLL_DIR ${CMAKE_SOURCE_DIR}/WinDivert/dll)

# 包含 WinDivert 的头文件路径
include_directories(${WINDIVERT_INCLUDE_DIR})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        logo.rc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(firewall
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        rule.h rule.cpp rule.ui
        res.qrc
        firewall.h
        firewall.cpp
        logpage.h logpage.cpp logpage.ui

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET firewall APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(firewall SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(firewall
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(firewall PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# 链接 WinDivert 库
target_link_libraries(${PROJECT_NAME} PRIVATE ${WINDIVERT_LIB_DIR}/WinDivert.lib)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.firewall)
endif()
set_target_properties(firewall PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS firewall
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(firewall)
endif()
