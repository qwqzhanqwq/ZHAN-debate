import QtQuick
import ZhanDebate

// 底部操作栏 // Bottom control bar
Item {
    id: bar

    required property DebateController debate
    property bool fullScreen: false
    property bool resetArmed: false

    signal resetAllRequested()
    signal themeToggled()
    signal fullScreenToggled()

    readonly property string primaryLabel: debate.kind === "end" ? "已结束"
        : debate.state === "running" ? "暂停"
        : debate.state === "paused" ? "继续"
        : debate.state === "finished" ? "下一阶段"
        : "开始"

    implicitHeight: Theme.unit * 6 + Theme.unit * 2.5

    Rectangle {
        width: parent.width
        height: 1
        color: Theme.ink
    }

    Row {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        spacing: Math.round(Theme.unit * 1.25)

        KeyButton {
            primary: true
            label: bar.primaryLabel
            keyText: "Space"
            enabled: bar.debate.kind !== "end"
            onClicked: bar.debate.toggle()
        }
        KeyButton {
            label: "上一阶段"
            keyText: "←"
            enabled: bar.debate.stageIndex > 0
            onClicked: bar.debate.previous()
        }
        KeyButton {
            label: "下一阶段"
            keyText: "→"
            enabled: bar.debate.stageIndex < bar.debate.stageCount - 1
            onClicked: bar.debate.next()
        }
        KeyButton {
            visible: bar.debate.kind === "free"
            label: "交换发言"
            keyText: "Tab"
            enabled: bar.debate.canSwitchSide
            onClicked: bar.debate.switchSide()
        }
        KeyButton {
            label: "重置本阶段"
            keyText: "R"
            enabled: bar.debate.kind !== "end"
            onClicked: bar.debate.resetStage()
        }
    }

    Row {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: Math.round(Theme.unit * 1.25)

        KeyButton {
            danger: bar.resetArmed
            label: bar.resetArmed ? "再按一次确认" : "全部重置"
            keyText: "⇧R"
            onClicked: bar.resetAllRequested()
        }
        KeyButton {
            label: Theme.dark ? "浅色" : "深色"
            keyText: "T"
            onClicked: bar.themeToggled()
        }
        KeyButton {
            label: bar.fullScreen ? "退出全屏" : "全屏"
            keyText: "F"
            onClicked: bar.fullScreenToggled()
        }
    }
}
