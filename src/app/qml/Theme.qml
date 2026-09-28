pragma Singleton
import QtQuick

// 瑞士风格设计令牌：纸白、墨黑、瑞士红，以及随窗口缩放的网格单位
// Swiss-style design tokens: paper, ink, Swiss red, and a grid unit that scales with the window
QtObject {
    property bool dark: false
    // 网格单位，1280×800 时为 8px // Grid unit, 8px at 1280×800
    property real unit: 8

    readonly property color paper: dark ? "#0E0E0D" : "#F2F0EB"
    readonly property color ink: dark ? "#F2F0EB" : "#111111"
    readonly property color muted: dark ? "#8C8A85" : "#74726C"
    readonly property color rule: dark ? "#302F2C" : "#D4D1C9"
    readonly property color hover: dark ? "#1E1E1C" : "#E6E3DC"
    readonly property color red: dark ? "#FF3A2F" : "#E3000F"
    readonly property color blue: dark ? "#5A7BFF" : "#1537C4"
    readonly property color onAccent: "#FFFFFF"

    readonly property string textFont: "Inter"
    readonly property string mediumFont: "Inter Medium"
    readonly property string displayFont: "Inter Display SemiBold"

    // 字号，以网格单位计 // Type scale in grid units
    readonly property real micro: unit * 1.4
    readonly property real small: unit * 1.75
    readonly property real body: unit * 2.25
    readonly property real lead: unit * 2.75
    readonly property real title: unit * 5

    function sideColor(side) {
        return side === "pro" ? blue : side === "con" ? red : ink
    }
    function sideName(side) {
        return side === "pro" ? "正方" : side === "con" ? "反方" : "主持"
    }
    function sideNameEn(side) {
        return side === "pro" ? "PRO" : side === "con" ? "CON" : "CHAIR"
    }
    function pad2(n) {
        return n < 10 ? "0" + n : "" + n
    }
}
