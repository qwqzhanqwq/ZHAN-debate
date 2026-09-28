# 平台无关的计时核心，只依赖标准库 // Platform-independent timer core, standard library only
add_library(debate_core STATIC
    DebateRules.cpp
    DebateRules.h
    DebateTimer.cpp
    DebateTimer.h
)
target_include_directories(debate_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(debate_core PUBLIC cxx_std_17)
zhan_set_warnings(debate_core)
