#ifndef SCOPIC_FLOW_SEQUENCESLICERLOADER_P_H
#define SCOPIC_FLOW_SEQUENCESLICERLOADER_P_H

#include <QQuickItem>

namespace sflow {

    class SequenceSlicerLoaderPrivate;
    class SequenceSlicerLoaderContext;

    class SequenceSlicerLoader : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT
        QML_ATTACHED(SequenceSlicerLoaderContext)
        Q_DECLARE_PRIVATE(SequenceSlicerLoader)
        Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
        Q_PROPERTY(QObject *viewModel READ viewModel WRITE setViewModel NOTIFY viewModelChanged)
        Q_PROPERTY(QList<int> range READ range WRITE setRange NOTIFY rangeChanged)

    public:
        explicit SequenceSlicerLoader(QQuickItem *parent = nullptr);
        ~SequenceSlicerLoader() override;

        static SequenceSlicerLoaderContext *qmlAttachedProperties(QObject *object);

        QQmlComponent *delegate() const;
        void setDelegate(QQmlComponent *delegate);

        QObject *viewModel() const;
        void setViewModel(QObject *viewModel);

        QList<int> range() const;
        void setRange(const QList<int> &range);

        Q_INVOKABLE QQuickItem *itemForModel(QObject *model);

    signals:
        void delegateChanged();
        void viewModelChanged();
        void rangeChanged();

    private:
        QScopedPointer<SequenceSlicerLoaderPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_SEQUENCESLICERLOADER_P_H
