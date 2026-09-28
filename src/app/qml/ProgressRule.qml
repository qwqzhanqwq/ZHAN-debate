import QtQuick
import ZhanDebate

// 线性进度条，下方每 30 秒一个刻度、每分钟一个数字
// Linear progress bar with a tick every 30 s and a numeral every minute
Item {
    id: root

    property real value: 0
    property color accent: Theme.ink
    property color trackColor: Theme.rule
    property color scaleColor: Theme.muted
    property int totalSec: 0
    property real barHeight: Theme.unit * 1.25
    property bool showScale: true

    readonly property int tickCount: showScale && totalSec > 0 ? Math.floor(totalSec / 30) + 1 : 0

    implicitHeight: barHeight + (tickCount > 0 ? Theme.unit * 3.75 : 0)

    Rectangle {
        width: parent.width
        height: root.barHeight
        color: root.trackColor
    }

    Rectangle {
        width: parent.width * Math.max(0, Math.min(1, root.value))
        height: root.barHeight
        color: root.accent
    }

    Repeater {
        model: root.tickCount

        Item {
            id: tick
            required property int index
            readonly property bool major: index % 2 === 0
            readonly property bool last: index * 30 >= root.totalSec

            x: Math.round(root.width * index * 30 / root.totalSec)
            y: root.barHeight

            Rectangle {
                x: tick.last ? -width : 0
                width: 1
                height: tick.major ? Theme.unit * 1.25 : Theme.unit * 0.6
                color: root.scaleColor
            }

            Text {
                visible: tick.major
                x: tick.index === 0 ? 0 : tick.last ? -width : -width / 2
                y: Theme.unit * 1.6
                text: tick.index / 2
                color: root.scaleColor
                font.family: Theme.mediumFont
                font.pixelSize: Theme.micro
            }
        }
    }
}
