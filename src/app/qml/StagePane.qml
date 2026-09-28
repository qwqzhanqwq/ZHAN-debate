import QtQuick
import QtQuick.Layouts
import ZhanDebate

// 左侧主区域：阶段标题 + 大号倒计时 // Main area: stage heading plus the big countdown
Item {
    id: pane

    required property DebateController debate

    readonly property bool isSpeech: debate.kind === "speech"
    readonly property bool isFree: debate.kind === "free"
    readonly property bool isEnd: debate.kind === "end"
    readonly property bool finished: debate.state === "finished"
    readonly property color accent: Theme.sideColor(debate.side)

    // --- 标题区 // Heading ---
    Column {
        id: heading
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: Theme.unit * 1.5

        RowLayout {
            width: parent.width
            spacing: Theme.unit

            MicroLabel {
                text: "阶段 Stage"
            }
            Text {
                text: pane.isEnd ? "—" : Theme.pad2(pane.debate.stageIndex + 1) + " / " + Theme.pad2(pane.debate.stageCount - 1)
                color: Theme.ink
                font.family: Theme.mediumFont
                font.pixelSize: Theme.micro
                font.letterSpacing: Theme.micro * 0.08
            }
            Item {
                Layout.fillWidth: true
            }
            StateIndicator {
                runState: pane.debate.state
                over: pane.isEnd
            }
        }

        Row {
            width: parent.width
            spacing: Theme.unit * 2

            Text {
                id: title
                text: pane.debate.title
                color: Theme.ink
                font.family: Theme.textFont
                font.bold: true
                font.pixelSize: Theme.title
                font.letterSpacing: -Theme.title * 0.01
            }
            MicroLabel {
                anchors.baseline: title.baseline
                text: pane.debate.titleEn
            }
        }

        Row {
            visible: !pane.isEnd
            spacing: Theme.unit * 1.75

            // 主持人与自由辩论不需要发言方徽标 // No side badge for the chair or free debate
            SideBadge {
                id: badge
                side: pane.debate.side
                visible: pane.debate.side !== "neutral"
            }
            Text {
                height: badge.height
                verticalAlignment: Text.AlignVCenter
                text: pane.debate.speaker
                color: Theme.ink
                font.family: Theme.mediumFont
                font.pixelSize: Theme.lead
            }
        }
    }

    // --- 普通发言：大号倒计时 + 进度条 // Speech: big countdown plus progress rule ---
    Item {
        id: speechView
        visible: pane.isSpeech
        anchors.top: heading.bottom
        anchors.topMargin: Theme.unit * 4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Clock {
            id: clock
            anchors.left: parent.left
            anchors.bottom: speechMeta.top
            anchors.bottomMargin: Theme.unit * 3.5
            maxWidth: parent.width
            maxHeight: parent.height - speechMeta.height - progress.height - Theme.unit * 7
            text: pane.debate.remainingText
            color: pane.finished || pane.debate.warning ? Theme.red : Theme.ink
            blinking: pane.finished
        }

        RowLayout {
            id: speechMeta
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: progress.top
            anchors.bottomMargin: Theme.unit * 1.5
            spacing: Theme.unit

            MicroLabel {
                text: "已用 Elapsed"
            }
            Text {
                text: pane.debate.elapsedText
                color: Theme.ink
                font.family: Theme.mediumFont
                font.pixelSize: Theme.small
            }
            Item {
                Layout.fillWidth: true
            }
            MicroLabel {
                text: pane.debate.warning ? "最后 " + pane.debate.warningSec + " 秒 Final seconds" : "总时长 Total"
                color: pane.debate.warning ? Theme.red : Theme.muted
            }
            Text {
                text: pane.debate.durationText
                color: Theme.ink
                font.family: Theme.mediumFont
                font.pixelSize: Theme.small
            }
        }

        ProgressRule {
            id: progress
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            value: pane.debate.progress
            accent: pane.finished || pane.debate.warning ? Theme.red : pane.accent
            totalSec: pane.debate.durationSec
        }
    }

    // --- 自由辩论 // Free debate ---
    FreeDebatePanel {
        visible: pane.isFree
        anchors.top: heading.bottom
        anchors.topMargin: Theme.unit * 4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        debate: pane.debate
    }

    // --- 比赛结束 // Match over ---
    Item {
        visible: pane.isEnd
        anchors.top: heading.bottom
        anchors.topMargin: Theme.unit * 4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        TextMetrics {
            id: endMetrics
            font: endMark.font
            text: endMark.text
        }

        Text {
            id: endMark
            readonly property real size: Math.min(parent.width * 0.42, parent.height * 0.7)
            anchors.left: parent.left
            // 让字母 E 的墨迹贴齐网格线 // Sit the ink of the E on the grid line
            anchors.leftMargin: -endMetrics.tightBoundingRect.x
            anchors.bottom: endNote.top
            anchors.bottomMargin: Theme.unit * 3
            text: "End"
            color: Theme.ink
            font.family: Theme.displayFont
            font.pixelSize: size
            font.letterSpacing: -size * 0.03
        }

        // 红色方块作句号 // A red square serves as the full stop
        Rectangle {
            anchors.left: endMark.right
            anchors.leftMargin: endMark.size * 0.04
            anchors.bottom: endMark.baseline
            width: endMark.size * 0.14
            height: width
            color: Theme.red
        }

        Text {
            id: endNote
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            text: "全部 " + (pane.debate.stageCount - 1) + " 个阶段已完成。按两次 ⇧R 重新开始。"
            color: Theme.muted
            font.family: Theme.mediumFont
            font.pixelSize: Theme.body
        }
    }
}
