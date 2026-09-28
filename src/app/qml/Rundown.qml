import QtQuick
import ZhanDebate

// 右侧流程表，可点击跳转（计时中不可跳转，防止误触）
// Rundown on the right; click a row to jump (disabled while running to avoid slips)
Item {
    id: rundown

    required property DebateController debate

    readonly property int count: debate.stages.length
    readonly property real rowHeight: Math.min(Theme.unit * 6.5, (height - header.height - Theme.unit * 1.5) / Math.max(1, count))

    Row {
        id: header
        spacing: Theme.unit
        height: Theme.unit * 3

        Text {
            id: headerTitle
            text: "流程"
            color: Theme.ink
            font.family: Theme.textFont
            font.bold: true
            font.pixelSize: Theme.small
        }
        MicroLabel {
            anchors.baseline: headerTitle.baseline
            text: "Rundown"
        }
    }

    Rectangle {
        id: headerRule
        anchors.top: header.bottom
        anchors.topMargin: Theme.unit * 1.25
        width: parent.width
        height: Math.max(2, Theme.unit / 4)
        color: Theme.ink
    }

    Column {
        anchors.top: headerRule.bottom
        width: parent.width

        Repeater {
            model: rundown.debate.stages

            Item {
                id: row

                required property int index
                required property var modelData

                readonly property bool current: index === rundown.debate.stageIndex
                readonly property bool done: index < rundown.debate.stageIndex
                readonly property bool isEnd: modelData.kind === "end"
                readonly property bool clickable: rundown.debate.state !== "running" && !current
                readonly property color foreground: current ? Theme.paper : done ? Theme.muted : Theme.ink

                width: parent.width
                height: rundown.rowHeight

                Rectangle {
                    anchors.fill: parent
                    color: row.current ? Theme.ink : mouse.containsMouse && row.clickable ? Theme.hover : "transparent"
                }

                Text {
                    id: number
                    x: Theme.unit
                    anchors.verticalCenter: parent.verticalCenter
                    width: Theme.unit * 4
                    text: row.isEnd ? "■" : Theme.pad2(row.index + 1)
                    color: row.isEnd ? Theme.red : row.foreground
                    opacity: row.current ? 1 : 0.8
                    font.family: Theme.mediumFont
                    font.pixelSize: Theme.small
                }

                // 发言方标记 // Side marker
                Rectangle {
                    id: marker
                    anchors.left: number.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: Theme.unit
                    height: width
                    visible: row.modelData.side !== "neutral"
                    color: Theme.sideColor(row.modelData.side)
                    opacity: row.done ? 0.45 : 1
                }

                Text {
                    id: titleText
                    anchors.left: marker.right
                    anchors.leftMargin: Theme.unit * 1.5
                    anchors.verticalCenter: parent.verticalCenter
                    text: row.modelData.title
                    color: row.foreground
                    font.family: row.current ? Theme.textFont : Theme.mediumFont
                    font.bold: row.current
                    font.pixelSize: Theme.small
                }

                Text {
                    anchors.left: titleText.right
                    anchors.leftMargin: Theme.unit
                    anchors.right: duration.left
                    anchors.rightMargin: Theme.unit
                    anchors.baseline: titleText.baseline
                    text: row.modelData.speaker
                    color: row.current ? Theme.paper : Theme.muted
                    opacity: row.current ? 0.75 : 1
                    elide: Text.ElideRight
                    font.family: Theme.mediumFont
                    font.pixelSize: Theme.micro * 1.1
                }

                Text {
                    id: duration
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.unit
                    anchors.baseline: titleText.baseline
                    text: row.modelData.durationText
                    color: row.foreground
                    font.family: Theme.mediumFont
                    font.pixelSize: Theme.small
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: Theme.rule
                    visible: !row.current
                }

                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: row.clickable
                    cursorShape: row.clickable ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: rundown.debate.goTo(row.index)
                }
            }
        }
    }
}
