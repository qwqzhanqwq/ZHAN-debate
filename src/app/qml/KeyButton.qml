import QtQuick
import ZhanDebate

// 平面按钮，右侧带快捷键提示。按钮不获取键盘焦点，空格键始终用于计时
// Flat button with a keycap hint. Buttons never take keyboard focus, so Space always drives the timer
Rectangle {
    id: button

    property string label
    property string keyText
    property bool primary: false
    property bool danger: false

    signal clicked()

    readonly property bool filled: primary || danger
    readonly property color foreground: danger ? Theme.onAccent : primary ? Theme.paper : Theme.ink

    // 取整到像素，避免边框落在半像素上发虚 // Snap to whole pixels so borders stay crisp
    implicitWidth: Math.round(content.implicitWidth + Theme.unit * 3.5)
    implicitHeight: Math.round(Theme.unit * 6)
    opacity: enabled ? 1 : 0.28
    color: danger ? Theme.red
        : primary ? Theme.ink
        : mouse.pressed ? Theme.rule
        : mouse.containsMouse ? Theme.hover
        : "transparent"
    border.width: filled ? 0 : Math.max(1, Math.round(Theme.unit / 5))
    border.color: Theme.ink

    Accessible.role: Accessible.Button
    Accessible.name: label

    Behavior on color { ColorAnimation { duration: 90 } }

    Row {
        id: content
        anchors.centerIn: parent
        spacing: Theme.unit * 1.25

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: button.label
            color: button.foreground
            font.family: Theme.mediumFont
            font.pixelSize: Theme.small * 1.05
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            visible: button.keyText.length > 0
            width: Math.max(keyLabel.implicitWidth + Theme.unit * 1.25, Theme.unit * 3)
            height: Theme.unit * 2.75
            color: "transparent"
            border.width: 1
            border.color: button.foreground
            opacity: 0.6

            Text {
                id: keyLabel
                anchors.centerIn: parent
                text: button.keyText
                color: button.foreground
                font.family: Theme.mediumFont
                font.pixelSize: Theme.micro
                font.letterSpacing: Theme.micro * 0.05
            }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: button.clicked()
    }
}
