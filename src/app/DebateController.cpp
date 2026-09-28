#include "DebateController.h"

#include <QVariantMap>

using debate::RunState;
using debate::Side;
using debate::StageKind;

namespace
{

QString toQString(const std::string& text)
{
    return QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size()));
}

QString formatMs(std::int64_t ms)
{
    return QString::fromLatin1(debate::formatClock(ms).c_str());
}

QString sideName(Side side)
{
    switch (side)
    {
    case Side::Pro:
        return QStringLiteral("pro");
    case Side::Con:
        return QStringLiteral("con");
    case Side::Neutral:
        break;
    }
    return QStringLiteral("neutral");
}

QString kindName(StageKind kind)
{
    switch (kind)
    {
    case StageKind::Speech:
        return QStringLiteral("speech");
    case StageKind::FreeDebate:
        return QStringLiteral("free");
    case StageKind::End:
        break;
    }
    return QStringLiteral("end");
}

}  // namespace

DebateController::DebateController(QObject* parent)
    : QObject(parent)
{
    // 刷新频率只影响进度条平滑度，计时精度由 QElapsedTimer 保证
    // The refresh rate only affects smoothness; accuracy comes from QElapsedTimer
    ticker_.setInterval(50);
    ticker_.setTimerType(Qt::PreciseTimer);
    connect(&ticker_, &QTimer::timeout, this, &DebateController::advance);
}

void DebateController::toggle()
{
    // 先结算已流逝的时间，避免暂停时丢失不足一次刷新的时间
    // Settle elapsed time first so pausing never drops a partial refresh interval
    if (timer_.state() == RunState::Running)
        timer_.tick(clock_.restart());
    timer_.toggle();
    syncTicker();
}

void DebateController::next()
{
    timer_.nextStage();
    syncTicker();
}

void DebateController::previous()
{
    timer_.previousStage();
    syncTicker();
}

void DebateController::goTo(int index)
{
    timer_.goToStage(index);
    syncTicker();
}

void DebateController::resetStage()
{
    timer_.resetStage();
    syncTicker();
}

void DebateController::resetAll()
{
    timer_.resetAll();
    syncTicker();
}

void DebateController::switchSide()
{
    if (timer_.state() == RunState::Running)
        timer_.tick(clock_.restart());
    timer_.switchSide();
    syncTicker();
}

void DebateController::advance()
{
    timer_.tick(clock_.restart());
    syncTicker();
}

void DebateController::syncTicker()
{
    bool running = timer_.state() == RunState::Running;
    if (running && !ticker_.isActive())
    {
        clock_.restart();
        ticker_.start();
    }
    else if (!running && ticker_.isActive())
    {
        ticker_.stop();
    }
    emit changed();
}

QString DebateController::title() const
{
    return toQString(timer_.stage().title);
}

QString DebateController::titleEn() const
{
    return toQString(timer_.stage().titleEn);
}

QString DebateController::speaker() const
{
    return toQString(timer_.stage().speaker);
}

QString DebateController::side() const
{
    return sideName(timer_.stage().side);
}

QString DebateController::kind() const
{
    return kindName(timer_.stage().kind);
}

QString DebateController::state() const
{
    switch (timer_.state())
    {
    case RunState::Running:
        return QStringLiteral("running");
    case RunState::Paused:
        return QStringLiteral("paused");
    case RunState::Finished:
        return QStringLiteral("finished");
    case RunState::Idle:
        break;
    }
    return QStringLiteral("idle");
}

QVariantList DebateController::stages() const
{
    QVariantList list;
    for (const debate::Stage& stage : timer_.rules().stages)
    {
        QVariantMap item;
        item.insert(QStringLiteral("title"), toQString(stage.title));
        item.insert(QStringLiteral("titleEn"), toQString(stage.titleEn));
        item.insert(QStringLiteral("speaker"), toQString(stage.speaker));
        item.insert(QStringLiteral("side"), sideName(stage.side));
        item.insert(QStringLiteral("kind"), kindName(stage.kind));
        // 自由辩论显示双方合计时长 // Free debate shows the combined time of both sides
        std::int64_t totalMs = stage.durationSec * 1000LL * (stage.kind == StageKind::FreeDebate ? 2 : 1);
        item.insert(QStringLiteral("durationText"), stage.kind == StageKind::End ? QString() : formatMs(totalMs));
        list.append(item);
    }
    return list;
}

QString DebateController::remainingText() const
{
    return formatMs(timer_.remainingMs());
}

QString DebateController::elapsedText() const
{
    std::int64_t total = timer_.durationMs() * (timer_.isFreeDebate() ? 2 : 1);
    std::int64_t left = timer_.isFreeDebate()
        ? timer_.sideRemainingMs(Side::Pro) + timer_.sideRemainingMs(Side::Con)
        : timer_.remainingMs();
    // 已用时间向下取整，与向上取整的剩余时间相加正好等于总时长
    // Elapsed rounds down so that it adds up with the rounded-up remaining time
    std::int64_t elapsed = (total - left) / 1000 * 1000;
    return formatMs(elapsed);
}

QString DebateController::durationText() const
{
    return formatMs(timer_.durationMs() * (timer_.isFreeDebate() ? 2 : 1));
}

QString DebateController::activeSide() const
{
    return sideName(timer_.activeSide());
}

QString DebateController::proText() const
{
    return formatMs(timer_.sideRemainingMs(Side::Pro));
}

QString DebateController::conText() const
{
    return formatMs(timer_.sideRemainingMs(Side::Con));
}

double DebateController::proFraction() const
{
    return sideFraction(Side::Pro);
}

double DebateController::conFraction() const
{
    return sideFraction(Side::Con);
}

bool DebateController::proWarning() const
{
    return sideWarning(Side::Pro);
}

bool DebateController::conWarning() const
{
    return sideWarning(Side::Con);
}

QString DebateController::turnText() const
{
    return formatMs(timer_.turnRemainingMs());
}

double DebateController::sideFraction(Side side) const
{
    std::int64_t total = timer_.durationMs();
    if (!timer_.isFreeDebate() || total <= 0)
        return 0.0;
    return static_cast<double>(timer_.sideRemainingMs(side)) / total;
}

bool DebateController::sideWarning(Side side) const
{
    std::int64_t left = timer_.sideRemainingMs(side);
    return timer_.isFreeDebate() && left > 0 && left <= timer_.rules().warningSec * 1000LL;
}
