import QtQuick
import QtQuick.Window
import ZhanDebate

Window {
    id: window

    width: 1280
    height: 800
    minimumWidth: 960
    minimumHeight: 600
    visible: true
    title: "辩论赛计时器 — ZHAN Debate"
    color: Theme.paper

    readonly property bool fullScreen: visibility === Window.FullScreen
    property bool resetArmed: false

    function toggleFullScreen() {
        if (fullScreen)
            showNormal()
        else
            showFullScreen()
    }

    // 全部重置需在 3 秒内按两次，防止比赛中误触
    // Reset-all needs two presses within 3 s so it cannot be triggered by accident mid-match
    function requestResetAll() {
        if (resetArmed) {
            resetArmed = false
            debate.resetAll()
        } else {
            resetArmed = true
            disarmTimer.restart()
        }
    }

    Timer {
        id: disarmTimer
        interval: 3000
        onTriggered: window.resetArmed = false
    }

    // 网格单位随窗口缩放：1280×800 时为 8px // Grid unit scales with the window: 8px at 1280×800
    Binding {
        target: Theme
        property: "unit"
        value: Math.max(6, Math.min(window.width / 160, window.height / 100))
    }

    DebateController {
        id: debate
    }

    // --- 快捷键 // Keyboard shortcuts ---
    Shortcut { sequence: "Space"; onActivated: debate.toggle() }
    Shortcut { sequences: ["Right", "PgDown"]; onActivated: debate.next() }
    Shortcut { sequences: ["Left", "PgUp"]; onActivated: debate.previous() }
    Shortcut { sequences: ["Tab", "S"]; onActivated: debate.switchSide() }
    Shortcut { sequence: "R"; onActivated: debate.resetStage() }
    Shortcut { sequence: "Shift+R"; onActivated: window.requestResetAll() }
    Shortcut { sequence: "T"; onActivated: Theme.dark = !Theme.dark }
    Shortcut { sequences: ["F", "F11"]; onActivated: window.toggleFullScreen() }
    Shortcut { sequence: "Esc"; enabled: window.fullScreen; onActivated: window.showNormal() }

    // --- 12 栏网格：左 8 栏为当前阶段，右 4 栏为流程 // 12-column grid: 8 for the stage, 4 for the rundown ---
    Item {
        id: page

        readonly property real gutter: Theme.unit * 3
        readonly property real column: (width - gutter * 11) / 12

        anchors.fill: parent
        anchors.margins: Theme.unit * 5
        anchors.topMargin: Theme.unit * 3.5
        anchors.bottomMargin: Theme.unit * 3.5

        Header {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right
        }

        StagePane {
            anchors.top: header.bottom
            anchors.topMargin: Theme.unit * 4
            anchors.bottom: controls.top
            anchors.bottomMargin: Theme.unit * 4
            anchors.left: parent.left
            width: page.column * 8 + page.gutter * 7
            debate: debate
        }

        Rundown {
            anchors.top: header.bottom
            anchors.topMargin: Theme.unit * 4
            anchors.bottom: controls.top
            anchors.bottomMargin: Theme.unit * 4
            anchors.right: parent.right
            width: page.column * 4 + page.gutter * 3
            debate: debate
        }

        ControlBar {
            id: controls
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            debate: debate
            fullScreen: window.fullScreen
            resetArmed: window.resetArmed
            onResetAllRequested: window.requestResetAll()
            onThemeToggled: Theme.dark = !Theme.dark
            onFullScreenToggled: window.toggleFullScreen()
        }
    }
}
