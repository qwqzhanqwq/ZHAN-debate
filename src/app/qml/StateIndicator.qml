import QtQuick
import ZhanDebate

// 计时状态：方块 + 中英文 // Run state: a square marker plus bilingual label
Row {
    id: indicator

    property string runState: "idle"
    property bool over: false

    readonly property bool hot: !over && (runState === "running" || runState === "finished")
    readonly property var labels: over ? ["已结束", "Complete"]
        : runState === "running" ? ["计时中", "Running"]
        : runState === "paused" ? ["已暂停", "Paused"]
        : runState === "finished" ? ["时间到", "Time up"]
        : ["准备", "Ready"]

    spacing: Theme.unit

    Rectangle {
        id: marker
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.unit * 1.25
        height: width
        color: indicator.hot ? Theme.red : "transparent"
        border.width: indicator.hot ? 0 : Math.max(1, Math.round(Theme.unit / 5))
        border.color: Theme.ink

        SequentialAnimation on opacity {
            running: indicator.runState === "running" && !indicator.over
            loops: Animation.Infinite
            onRunningChanged: if (!running) marker.opacity = 1
            NumberAnimation { to: 0.15; duration: 600; easing.type: Easing.InOutSine }
            NumberAnimation { to: 1; duration: 600; easing.type: Easing.InOutSine }
        }
    }

    Text {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        text: indicator.labels[0]
        color: Theme.ink
        font.family: Theme.mediumFont
        font.pixelSize: Theme.small
    }

    MicroLabel {
        anchors.baseline: label.baseline
        text: indicator.labels[1]
    }
}
