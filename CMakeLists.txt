cmake_minimum_required(VERSION 3.10)
project(Toolkit VERSION 2.0.0)

add_definitions(-DVERSION="${PROJECT_VERSION}")

# 1. 自动处理 Qt 的特殊工具 (moc, uic, rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 2. 找到 Qt 库 (以 Qt6 为例)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
# 3. 添加源文件
add_executable(Toolkit 
    #头文件
    headers/mainwindow.h
    headers/changelog.h
    headers/database.h

    #源文件
    sources/main.cpp
    sources/toolkit.cpp
    sources/changelog.cpp
    sources/database.cpp

    #UI文件
    assets/toolkit.ui
    assets/changelog.ui

    #资源文件
    resources.qrc
)

# 4. 链接 Qt 库
target_link_libraries(Toolkit PRIVATE Qt6::Widgets Qt6::Sql)

target_include_directories(Toolkit PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/headers")