#include "QuickWrapperHelper_p.h"

#include <QWidget>
#include <QQuickView>
#include <QQmlEngine>

namespace sflow {
    QPair<QWidget *, QQuickItem *> QuickWrapperHelper::wrap(const QString &name) {
        auto w = new QQuickView;
        w->setSource(QUrl(QString("qrc:/ScopicFlow/private/%1.qml").arg(name)));
        w->setResizeMode(QQuickView::SizeRootObjectToView);
        return {QWidget::createWindowContainer(w), w->rootObject()};
    }

}