#pragma once

#include "DebateTimer.h"

#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

// 把平台无关的 DebateTimer 暴露给 QML，并用单调时钟驱动计时
// Exposes the platform-independent DebateTimer to QML and drives it with a monotonic clock
class DebateController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    // 阶段信息 // Stage info
    Q_PROPERTY(int stageIndex READ stageIndex NOTIFY changed)
    Q_PROPERTY(int stageCount READ stageCount CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY changed)
    Q_PROPERTY(QString titleEn READ titleEn NOTIFY changed)
    Q_PROPERTY(QString speaker READ speaker NOTIFY changed)
    Q_PROPERTY(QString side READ side NOTIFY changed)    // "neutral" | "pro" | "con"
    Q_PROPERTY(QString kind READ kind NOTIFY changed)    // "speech" | "free" | "end"
    Q_PROPERTY(QString state READ state NOTIFY changed)  // "idle" | "running" | "paused" | "finished"
    Q_PROPERTY(QVariantList stages READ stages CONSTANT)

    // 计时 // Timing
    Q_PROPERTY(QString remainingText READ remainingText NOTIFY changed)
    Q_PROPERTY(QString elapsedText READ elapsedText NOTIFY changed)
    Q_PROPERTY(QString durationText READ durationText NOTIFY changed)
    Q_PROPERTY(int durationSec READ durationSec NOTIFY changed)
    Q_PROPERTY(double progress READ progress NOTIFY changed)
    Q_PROPERTY(bool warning READ warning NOTIFY changed)
    Q_PROPERTY(int warningSec READ warningSec CONSTANT)

    // 自由辩论 // Free debate
    Q_PROPERTY(QString activeSide READ activeSide NOTIFY changed)
    Q_PROPERTY(QString proText READ proText NOTIFY changed)
    Q_PROPERTY(QString conText READ conText NOTIFY changed)
    Q_PROPERTY(double proFraction READ proFraction NOTIFY changed)
    Q_PROPERTY(double conFraction READ conFraction NOTIFY changed)
    Q_PROPERTY(bool proWarning READ proWarning NOTIFY changed)
    Q_PROPERTY(bool conWarning READ conWarning NOTIFY changed)
    Q_PROPERTY(QString turnText READ turnText NOTIFY changed)
    Q_PROPERTY(bool turnLimited READ turnLimited NOTIFY changed)
    Q_PROPERTY(bool canSwitchSide READ canSwitchSide NOTIFY changed)

public:
    explicit DebateController(QObject* parent = nullptr);

    Q_INVOKABLE void toggle();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void goTo(int index);
    Q_INVOKABLE void resetStage();
    Q_INVOKABLE void resetAll();
    Q_INVOKABLE void switchSide();

    int stageIndex() const { return timer_.stageIndex(); }
    int stageCount() const { return timer_.stageCount(); }
    QString title() const;
    QString titleEn() const;
    QString speaker() const;
    QString side() const;
    QString kind() const;
    QString state() const;
    QVariantList stages() const;

    QString remainingText() const;
    QString elapsedText() const;
    QString durationText() const;
    int durationSec() const { return timer_.stage().durationSec; }
    double progress() const { return timer_.progress(); }
    bool warning() const { return timer_.isWarning(); }
    int warningSec() const { return timer_.rules().warningSec; }

    QString activeSide() const;
    QString proText() const;
    QString conText() const;
    double proFraction() const;
    double conFraction() const;
    bool proWarning() const;
    bool conWarning() const;
    QString turnText() const;
    bool turnLimited() const { return timer_.turnLimited(); }
    bool canSwitchSide() const { return timer_.canSwitchSide(); }

signals:
    void changed();

private:
    void advance();
    void syncTicker();
    double sideFraction(debate::Side side) const;
    bool sideWarning(debate::Side side) const;

    debate::DebateTimer timer_;
    QTimer ticker_;
    QElapsedTimer clock_;
};
