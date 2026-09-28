#include "DebateRules.h"

namespace debate
{

Rules Rules::standard()
{
    Rules rules;
    rules.stages = {
        // 类型                 发言方        阶段名        英文名                 发言人       时长(秒)
        // Kind                 Side          Title         English                Speaker      Seconds
        {StageKind::Speech,     Side::Neutral, "主持人开场", "Opening",            "主持人",    120},
        {StageKind::Speech,     Side::Pro,     "立论阶段",   "Constructive",       "正方一辩",  180},
        {StageKind::Speech,     Side::Con,     "立论阶段",   "Constructive",       "反方一辩",  180},
        {StageKind::Speech,     Side::Pro,     "驳论阶段",   "Rebuttal",           "正方二辩",  120},
        {StageKind::Speech,     Side::Con,     "驳论阶段",   "Rebuttal",           "反方二辩",  120},
        {StageKind::Speech,     Side::Pro,     "质询阶段",   "Cross-examination",  "正方三辩",  120},
        {StageKind::Speech,     Side::Con,     "质询阶段",   "Cross-examination",  "反方三辩",  120},
        {StageKind::FreeDebate, Side::Neutral, "自由辩论",   "Free debate",        "正反双方",  300},
        {StageKind::Speech,     Side::Con,     "总结陈词",   "Closing",            "反方四辩",  180},
        {StageKind::Speech,     Side::Pro,     "总结陈词",   "Closing",            "正方四辩",  180},
        {StageKind::End,        Side::Neutral, "比赛结束",   "End",                "",          0},
    };
    return rules;
}

}  // namespace debate
