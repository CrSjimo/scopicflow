#ifndef SCOPIC_FLOW_SEQUENCESLICERLOADER_P_H
#define SCOPIC_FLOW_SEQUENCESLICERLOADER_P_H

#include <QQuickItem>

namespace sflow {

    class SequenceSlicerLoaderPrivate;

    class SequenceSlicerLoader : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(SequenceSlicerLoader)
        Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
        Q_PROPERTY(QObject *handle READ handle WRITE setHandle NOTIFY handleChanged)
        Q_PROPERTY(QList<int> range READ range WRITE setRange NOTIFY rangeChanged)

    public:
        explicit SequenceSlicerLoader(QQuickItem *parent = nullptr);
        ~SequenceSlicerLoader() override;

        QQmlComponent *delegate() const;
        void setDelegate(QQmlComponent *delegate);

        QObject *handle() const;
        void setHandle(QObject *handle);

        QList<int> range() const;
        void setRange(const QList<int> &range);

    signals:
        void delegateChanged();
        void handleChanged();
        void rangeChanged();

    private:
        QScopedPointer<SequenceSlicerLoaderPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_SEQUENCESLICERLOADER_P_H
