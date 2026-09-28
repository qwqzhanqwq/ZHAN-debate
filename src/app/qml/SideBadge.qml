import QtQuick
import ZhanDebate

// 发言方色块：正方蓝、反方红、主持人为黑框 // Side badge: blue for Pro, red for Con, outlined for the chair
Rectangle {
    id: badge

    property string side: "neutral"
    readonly property bool neutral: side !== "pro" && side !== "con"
    readonly property color foreground: neutral ? Theme.ink : Theme.onAccent

    implicitWidth: Math.round(content.implicitWidth + Theme.unit * 2.5)
    implicitHeight: Math.round(Theme.unit * 4)
    color: neutral ? "transparent" : Theme.sideColor(side)
    border.width: neutral ? Math.max(1, Math.round(Theme.unit / 5)) : 0
    border.color: Theme.ink

    Row {
        id: content
        anchors.centerIn: parent
        spacing: Theme.unit

        Text {
            id: name
            text: Theme.sideName(badge.side)
            color: badge.foreground
            font.family: Theme.textFont
            font.bold: true
            font.pixelSize: Theme.small
        }

        Text {
            anchors.baseline: name.baseline
            text: Theme.sideNameEn(badge.side)
            color: badge.foreground
            font.family: Theme.mediumFont
            font.pixelSize: Theme.micro
            font.letterSpacing: Theme.micro * 0.08
        }
    }
}
