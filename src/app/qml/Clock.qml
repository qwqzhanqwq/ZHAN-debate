import QtQuick
import ZhanDebate

// 大号倒计时数字：每个数字占固定宽度，读秒时版面不会左右晃动
// Large countdown numerals: every digit sits in a fixed-width cell so the layout never jitters
Item {
    id: root

    property string text: "00:00"
    property color color: Theme.ink
    property real maxWidth: 400
    property real maxHeight: 200
    property bool blinking: false

    // 以 100px 为基准测量字形比例 // Measure glyph proportions at a 100px reference size
    TextMetrics {
        id: reference
        font.family: Theme.displayFont
        font.pixelSize: 100
        text: "0"
    }
    readonly property real cellRatio: reference.advanceWidth / 100 * 0.98
    readonly property real colonRatio: reference.advanceWidth / 100 * 0.5
    readonly property real capRatio: Math.max(0.01, reference.tightBoundingRect.height / 100)
    readonly property real widthRatio: {
        let total = 0
        for (let i = 0; i < text.length; ++i)
            total += text.charAt(i) === ":" ? colonRatio : cellRatio
        return Math.max(0.01, total)
    }
    readonly property real pixelSize: Math.max(8, Math.min(maxWidth / widthRatio, maxHeight / capRatio))
    // 首个数字墨迹左缘到格子左缘的距离，用于与网格线精确对齐
    // Distance from the cell edge to the first digit's ink, used to sit the ink exactly on the grid line
    readonly property real inkLeft: (cellRatio * pixelSize - digit.advanceWidth) / 2 + digit.tightBoundingRect.x

    FontMetrics {
        id: metrics
        font.family: Theme.displayFont
        font.pixelSize: root.pixelSize
    }
    TextMetrics {
        id: digit
        font.family: Theme.displayFont
        font.pixelSize: root.pixelSize
        text: "0"
    }

    // 组件高度等于数字字形高度，便于按网格精确对齐
    // The item is exactly as tall as the digits so it can sit precisely on the grid
    implicitWidth: widthRatio * pixelSize
    implicitHeight: digit.tightBoundingRect.height
    width: implicitWidth
    height: implicitHeight

    Row {
        x: -root.inkLeft
        y: -(metrics.ascent + digit.tightBoundingRect.y)

        Repeater {
            model: root.text.length

            Text {
                required property int index
                readonly property string glyph: root.text.charAt(index)
                readonly property bool isColon: glyph === ":"

                width: (isColon ? root.colonRatio : root.cellRatio) * root.pixelSize
                // 单独排版时冒号偏低，上移到数字中线 // A lone colon sits low; lift it to the digits' middle
                y: isColon ? -(digit.tightBoundingRect.height - metrics.xHeight) / 2 : 0
                horizontalAlignment: Text.AlignHCenter
                text: glyph
                color: root.color
                font.family: Theme.displayFont
                font.pixelSize: root.pixelSize
            }
        }
    }

    onBlinkingChanged: if (!blinking) opacity = 1

    SequentialAnimation on opacity {
        running: root.blinking
        loops: Animation.Infinite
        NumberAnimation { to: 0.2; duration: 420; easing.type: Easing.InOutQuad }
        NumberAnimation { to: 1; duration: 420; easing.type: Easing.InOutQuad }
    }
}
