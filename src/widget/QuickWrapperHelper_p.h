#ifndef SCOPIC_FLOW_QUICKWRAPPERHELPER_P_H
#define SCOPIC_FLOW_QUICKWRAPPERHELPER_P_H

#include <QtGlobal>

class QWidget;
class QQuickItem;

namespace sflow {

    class QuickWrapperHelper {
    public:
        static QPair<QWidget *, QQuickItem *> wrap(const QString &name);
    };

} // sflow

#endif //SCOPIC_FLOW_QUICKWRAPPERHELPER_P_H
