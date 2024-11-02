#ifndef SCOPIC_FLOW_QUICKWRAPPERHELPER_P_H
#define SCOPIC_FLOW_QUICKWRAPPERHELPER_P_H

#include <QQuickWidget>
#include <QQuickView>

namespace sflow {

    class QuickWrapperHelper {
    public:
        static inline QPair<QWidget *, QQuickItem *> wrap(const QString &name) {
#if 1
            auto w = new QQuickWidget(QUrl(QString("qrc:/ScopicFlow/private/%1.qml").arg(name)));
            w->setFormat(QSurfaceFormat::defaultFormat());
            w->setResizeMode(QQuickWidget::SizeRootObjectToView);
            return {w, w->rootObject()};
#else
            auto w = new QQuickView(QUrl(QString("qrc:/ScopicFlow/private/%1.qml").arg(name)));
            w->setResizeMode(QQuickView::SizeRootObjectToView);
            return {QWidget::createWindowContainer(w), w->rootObject()};
#endif
        }
    };

} // sflow

#endif //SCOPIC_FLOW_QUICKWRAPPERHELPER_P_H
