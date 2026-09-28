import QtQuick
import QtQuick.Layouts
import ZhanDebate

// 自由辩论中一方的计时栏 // One side's column in free debate
Rectangle {
    id: panel

    property string side: "pro"
    property bool active: false
    property bool running: false
    property string timeText: "00:00"
    property real fraction: 1
    property bool warning: false
    property string turnText: "00:00"
    property bool turnLimited: true

    readonly property bool out: fraction <= 0
    readonly property color accent: Theme.sideColor(side)
    readonly property color foreground: active ? Theme.onAccent : Theme.ink
    readonly property color secondary: active ? Qt.rgba(1, 1, 1, 0.72) : Theme.muted

    color: active ? accent : "transparent"
    Behavior on color { ColorAnimation { duration: 160 } }

    // 顶部色条标明所属方 // Top bar in the side's colour
    Rectangle {
        width: parent.width
        height: Theme.unit * 0.75
        color: panel.accent
    }

    Item {
        anchors.fill: parent
        anchors.margins: Theme.unit * 3
        anchors.topMargin: Theme.unit * 3.5

        RowLayout {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: Theme.unit * 1.5

            Text {
                id: name
                text: Theme.sideName(panel.side)
                color: panel.foreground
                font.family: Theme.textFont
                font.bold: true
                font.pixelSize: Theme.unit * 4
            }
            Text {
                Layout.alignment: Qt.AlignBaseline
                text: Theme.sideNameEn(panel.side)
                color: panel.secondary
                font.family: Theme.mediumFont
                font.pixelSize: Theme.micro
                font.letterSpacing: Theme.micro * 0.08
            }
            Item {
                Layout.fillWidth: true
            }
            Text {
                Layout.alignment: Qt.AlignBaseline
                text: panel.out ? "已用完 Out"
                    : panel.active ? (panel.running ? "发言中 Speaking" : "待发言 Has the floor")
                    : "等待 Waiting"
                color: panel.foreground
                font.family: Theme.mediumFont
                font.pixelSize: Theme.small
            }
        }

        Clock {
            anchors.left: parent.left
            anchors.bottom: bar.top
            anchors.bottomMargin: Theme.unit * 2.5
            maxWidth: parent.width
            maxHeight: parent.height - header.height - bar.height - footer.height - Theme.unit * 8
            text: panel.timeText
            color: panel.active ? Theme.onAccent : panel.warning ? Theme.red : panel.out ? Theme.muted : Theme.ink
            blinking: panel.active && panel.running && panel.warning
        }

        ProgressRule {
            id: bar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: footer.top
            anchors.bottomMargin: Theme.unit * 1.5
            showScale: false
            barHeight: Theme.unit
            value: panel.fraction
            accent: panel.active ? Theme.onAccent : panel.accent
            trackColor: panel.active ? Qt.rgba(1, 1, 1, 0.28) : Theme.rule
        }

        RowLayout {
            id: footer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            spacing: Theme.unit

            Text {
                text: panel.active ? (panel.turnLimited ? "本轮剩余 Turn" : "对方已用完，可一次用完 No turn limit") : "剩余 Remaining"
                color: panel.secondary
                font.family: Theme.mediumFont
                font.pixelSize: Theme.micro
                font.letterSpacing: Theme.micro * 0.08
                font.capitalization: Font.AllUppercase
            }
            Item {
                Layout.fillWidth: true
            }
            Text {
                visible: panel.active && panel.turnLimited
                text: panel.turnText
                color: panel.foreground
                font.family: Theme.mediumFont
                font.pixelSize: Theme.body
            }
        }
    }
}
