#ifndef SCOPIC_FLOW_SEQUENCESLICERLOADER_P_P_H
#define SCOPIC_FLOW_SEQUENCESLICERLOADER_P_P_H

#include <ScopicFlowInternal/private/SequenceSlicerLoader_p.h>

#include <QHash>

#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>

namespace sflow {

    class SequenceSlicerLoaderContext : public QObject {
        Q_OBJECT
        QML_ANONYMOUS
        Q_PROPERTY(bool inRange READ isInRange NOTIFY inRangeChanged)
        Q_PROPERTY(QObject *viewModel READ viewModel NOTIFY viewModelChanged)
    public:
        explicit SequenceSlicerLoaderContext(QObject *parent = nullptr);
        ~SequenceSlicerLoaderContext() override;

        bool isInRange() const;
        void setInRange(bool inRange);

        QObject *viewModel() const;
        void setViewModel(QObject *viewModel);

    Q_SIGNALS:
        void inRangeChanged();
        void viewModelChanged();

    private:
        bool m_inRange{};
        QObject *m_viewModel{};
    };

    class SequenceSlicerLoaderPrivate {
        Q_DECLARE_PUBLIC(SequenceSlicerLoader)
    public:
        SequenceSlicerLoader *q_ptr;
        QPointer<QQmlComponent> delegate;
        QPointer<QObject> viewModel;
        QPointer<SliceableViewModelManipulatorInterface> handle;
        QPair<int, int> range;
        bool active{true};

        QHash<QObject *, QQuickItem *> visibleItems;
        QHash<QObject *, QQuickItem *> invisibleItems;

        void setHandle(SliceableViewModelManipulatorInterface *handle_);

        void onRangeChanged();
        void onDelegateChanged();
        void onHandleChanged();
        void onItemInserted(QObject *item);
        void onItemRemoved(QObject *item);
        void onItemUpdated(QObject *item);

        void temporarilyLoad(int first, int second);

        QQuickItem *createView(QObject *itemModel);
        void showViewIfExistsOrElseCreate(QObject *itemModel);
        void destroyView(QObject *itemModel);
        void hideView(QObject *itemModel);
        void hideAllVisible();

        bool inRange(QObject *itemModel) const;

    };
}

#endif //SCOPIC_FLOW_SEQUENCESLICERLOADER_P_P_H
