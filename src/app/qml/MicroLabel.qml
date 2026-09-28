import QtQuick
import ZhanDebate

// 小号大写标签，瑞士风格的双语注释 // Small uppercase label used for bilingual captions
Text {
    color: Theme.muted
    font.family: Theme.mediumFont
    font.pixelSize: Theme.micro
    font.letterSpacing: Theme.micro * 0.08
    font.capitalization: Font.AllUppercase
    elide: Text.ElideRight
}
