import QtQuick
import ZhanDebate

// 顶栏：标志、名称与当前时间，下方一条粗线 // Top bar: mark, name and local time over a heavy rule
Item {
    id: header

    implicitHeight: Theme.unit * 6

    property string now: Qt.formatTime(new Date(), "HH:mm")

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: header.now = Qt.formatTime(new Date(), "HH:mm")
    }

    Row {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -Theme.unit * 0.5
        spacing: Theme.unit * 1.5

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.unit * 2.5
            height: width
            color: Theme.red
        }
        Text {
            id: wordmark
            anchors.verticalCenter: parent.verticalCenter
            text: "ZHAN"
            color: Theme.ink
            font.family: Theme.textFont
            font.bold: true
            font.pixelSize: Theme.lead
            font.letterSpacing: Theme.lead * 0.02
        }
        Text {
            anchors.baseline: wordmark.baseline
            text: "辩论赛计时器"
            color: Theme.ink
            font.family: Theme.mediumFont
            font.pixelSize: Theme.small
        }
        MicroLabel {
            anchors.baseline: wordmark.baseline
            text: "Debate Timer"
        }
    }

    Row {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -Theme.unit * 0.5
        spacing: Theme.unit * 1.5

        MicroLabel {
            anchors.baseline: time.baseline
            text: "当前时间 Local time"
        }
        Text {
            id: time
            text: header.now
            color: Theme.ink
            font.family: Theme.mediumFont
            font.pixelSize: Theme.lead
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: Math.max(2, Theme.unit * 0.4)
        color: Theme.ink
    }
}
