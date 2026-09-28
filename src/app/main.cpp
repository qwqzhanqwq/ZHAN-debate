#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QUrl>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("ZHAN Debate"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("辩论赛计时器"));
    QGuiApplication::setApplicationVersion(QStringLiteral(ZHAN_VERSION));
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/zhan-debate.png")));

    // 内置 Inter 字体用于拉丁字母与数字，中文自动回退到系统字体
    // Bundled Inter covers Latin text and digits; Chinese falls back to the system font
    const char* fonts[] = {
        ":/fonts/Inter-Regular.ttf",
        ":/fonts/Inter-Medium.ttf",
        ":/fonts/Inter-Bold.ttf",
        ":/fonts/InterDisplay-SemiBold.ttf",
    };
    for (const char* font : fonts)
        QFontDatabase::addApplicationFont(QString::fromLatin1(font));

    // 中文字形优先回退到简体中文黑体 // Prefer Simplified Chinese sans fonts for CJK glyphs
    const QStringList cjkFallbacks = {
        QStringLiteral("Microsoft YaHei UI"),
        QStringLiteral("Microsoft YaHei"),
        QStringLiteral("PingFang SC"),
        QStringLiteral("Noto Sans CJK SC"),
        QStringLiteral("Source Han Sans SC"),
        QStringLiteral("WenQuanYi Micro Hei"),
    };
    for (const char* family : {"Inter", "Inter Medium", "Inter Display SemiBold"})
        QFont::insertSubstitutions(QString::fromLatin1(family), cjkFallbacks);

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/qt/qml"));
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/ZhanDebate/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return 1;

    return QGuiApplication::exec();
}
