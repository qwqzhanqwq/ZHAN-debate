import QtQuick
import QtQuick.Layouts
import ZhanDebate

// 自由辩论：正反双方各一栏，当前发言方整栏填色
// Free debate: one column per side; the side holding the floor is filled with its colour
Item {
    id: panel

    required property DebateController debate

    readonly property bool running: debate.state === "running"

    RowLayout {
        id: columns
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: footer.top
        anchors.bottomMargin: Theme.unit * 2.5
        spacing: Theme.unit * 3

        SidePanel {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1
            side: "pro"
            active: panel.debate.activeSide === "pro"
            running: panel.running
            timeText: panel.debate.proText
            fraction: panel.debate.proFraction
            warning: panel.debate.proWarning
            turnText: panel.debate.turnText
            turnLimited: panel.debate.turnLimited
        }

        SidePanel {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1
            side: "con"
            active: panel.debate.activeSide === "con"
            running: panel.running
            timeText: panel.debate.conText
            fraction: panel.debate.conFraction
            warning: panel.debate.conWarning
            turnText: panel.debate.turnText
            turnLimited: panel.debate.turnLimited
        }
    }

    RowLayout {
        id: footer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: Theme.unit

        MicroLabel {
            text: "合计已用 Elapsed"
        }
        Text {
            text: panel.debate.elapsedText + " / " + panel.debate.durationText
            color: Theme.ink
            font.family: Theme.mediumFont
            font.pixelSize: Theme.small
        }
        Item {
            Layout.fillWidth: true
        }
        MicroLabel {
            text: panel.debate.canSwitchSide ? "按 Tab 交换发言 Pass the floor" : "对方已无剩余时间 Opponent out of time"
        }
    }
}
