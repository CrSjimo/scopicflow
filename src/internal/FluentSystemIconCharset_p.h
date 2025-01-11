#ifndef SCOPIC_FLOW_FLUENTSYSTEMICONCHARSET_P_H
#define SCOPIC_FLOW_FLUENTSYSTEMICONCHARSET_P_H

#include <QObject>
#include <qqmlintegration.h>

namespace sflow {

    class FluentSystemIconCharset : public QObject {
        Q_OBJECT
        QML_SINGLETON
        QML_ELEMENT
    public:
        explicit FluentSystemIconCharset(QObject *parent = nullptr);
        ~FluentSystemIconCharset() override;

        Q_INVOKABLE QString get(const QString &name) const;
    };

}

#endif //SCOPIC_FLOW_FLUENTSYSTEMICONCHARSET_P_H
