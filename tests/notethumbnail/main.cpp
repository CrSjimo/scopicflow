#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

#include <ScopicFlowCore/RangeSequenceViewModel.h>
#include <ScopicFlowCore/NoteViewModel.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    RangeSequenceViewModel noteSequenceViewModel(&a);

    for (int i = 0; i < 600; ++i) {
        auto noteViewModel = new NoteViewModel(&a);
        noteViewModel->setKey(60 + i % 12);
        noteViewModel->setLyric(QString::number(i));
        noteViewModel->setAdditionalText(QString::number(i));
        noteViewModel->setPosition(240 * i);
        noteViewModel->setLength(240);
        noteViewModel->setAdditionalTextHighlighted(i % 5 == 0);
        noteSequenceViewModel.insertItem(noteViewModel);
    }

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"noteSequenceViewModel", QVariant::fromValue(&noteSequenceViewModel)},
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/NoteThumbnail/main.qml");

    return a.exec();
}
