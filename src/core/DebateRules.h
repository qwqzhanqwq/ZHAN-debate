#pragma once

#include <string>
#include <vector>

namespace debate
{

// 发言方 // Side
enum class Side
{
    Neutral,  // 主持人等中立角色 // Moderator or other neutral role
    Pro,      // 正方 // Affirmative
    Con       // 反方 // Negative
};

// 阶段类型 // Stage kind
enum class StageKind
{
    Speech,      // 单人发言，固定时长 // Single speaker, fixed duration
    FreeDebate,  // 自由辩论，双方各自计时、轮流发言 // Both sides keep their own clock and alternate
    End          // 比赛结束 // Match is over
};

// 辩论阶段 // Debate stage
struct Stage
{
    StageKind kind;
    Side side;
    std::string title;    // 阶段名（UTF-8），如“立论阶段” // Stage name (UTF-8)
    std::string titleEn;  // 英文阶段名，用于双语排版 // English name for bilingual layout
    std::string speaker;  // 发言人，如“正方一辩” // Speaker
    int durationSec;      // Speech：本阶段时长；FreeDebate：每方总时长 // Speech: stage length; FreeDebate: time per side
};

// 赛制 // Debate format
struct Rules
{
    std::vector<Stage> stages;       // 最后一个阶段应为 End // The last stage should be End
    int freeDebateTurnLimitSec = 60; // 自由辩论单次发言上限，0 表示不限 // Max length of one free-debate turn, 0 = unlimited
    int warningSec = 30;             // 剩余多少秒开始警示 // Remaining seconds that trigger the warning

    // 默认赛制 // Default format
    static Rules standard();
};

}  // namespace debate
