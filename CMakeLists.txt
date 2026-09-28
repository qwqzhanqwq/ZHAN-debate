cmake_minimum_required(VERSION 3.21)

project(ZhanDebate
    VERSION 2.0.0
    DESCRIPTION "Debate competition timer"
    LANGUAGES CXX)

include(CTest)

option(ZHAN_BUILD_APP "Build the Qt Quick desktop app (needs Qt 6.4+)" ON)

# 统一的编译警告与源码编码设置 // Shared warning and source-encoding settings
function(zhan_set_warnings target)
    if(MSVC)
        # 源码为 UTF-8（无 BOM） // Sources are UTF-8 without BOM
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_subdirectory(src/core)

if(ZHAN_BUILD_APP)
    add_subdirectory(src/app)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
