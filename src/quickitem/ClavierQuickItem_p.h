#ifndef SCOPIC_FLOW_CLAVIERQUICKITEM_P_H
#define SCOPIC_FLOW_CLAVIERQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>

namespace sflow {

    class ClavierQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT ClavierQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(ClavierQuickItem)
        Q_PROPERTY(ClavierViewModel *clavierViewModel READ clavierViewModel NOTIFY clavierViewModelChanged)
        Q_PROPERTY(ScrollBehaviorViewModel *scrollBehaviorViewModel READ scrollBehaviorViewModel NOTIFY scrollBehaviorViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel NOTIFY animationViewModelChanged)
        Q_PROPERTY(PaletteViewModel *paletteViewModel READ paletteViewModel NOTIFY paletteViewModelChanged)
        Q_PROPERTY(double keyHeight READ keyHeight NOTIFY keyHeightChanged)
        Q_PROPERTY(double viewportY READ viewportY NOTIFY viewportYChanged)
        Q_PROPERTY(int cursorNoteIndex READ cursorNoteIndex NOTIFY cursorNoteIndexChanged)
        Q_PROPERTY(sflow::ClavierQuickItem::LabelStrategy labelStrategy READ labelStrategy NOTIFY labelStrategyChanged)
        Q_PROPERTY(QString dummyKeyName READ dummyKeyName NOTIFY keyNameUpdated)
    public:
        explicit ClavierQuickItem(QQuickItem *parent = nullptr);
        ~ClavierQuickItem() override;

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *clavierViewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

        PaletteViewModel *paletteViewModel() const;
        void setPaletteViewModel(PaletteViewModel *paletteViewModel);

        double keyHeight() const;
        double viewportY() const;
        int cursorNoteIndex() const;

        enum LabelStrategy {
            None,
            C,
            All,
        };
        Q_ENUM(LabelStrategy)
        LabelStrategy labelStrategy() const;
        void setLabelStrategy(LabelStrategy strategy);

        int mapToKey(double y) const;

        QString dummyKeyName() const;

        Q_INVOKABLE QString keyName(int key) const;

    signals:
        void clavierViewModelChanged(ClavierViewModel *clavierViewModel);
        void scrollBehaviorViewModelChanged(ScrollBehaviorViewModel *scrollBehaviorViewModel);
        void animationViewModelChanged(AnimationViewModel *animationViewModel);
        void paletteViewModelChanged(PaletteViewModel *paletteViewModel);
        void keyHeightChanged(double keyHeight);
        void viewportYChanged(double viewportY);
        void cursorNoteIndexChanged(int cursorNoteIndex);
        void labelStrategyChanged(sflow::ClavierQuickItem::LabelStrategy labelStrategy);
        void noteOn(int key);
        void noteOff(int key);
        void keyNameUpdated();

    private:
        QScopedPointer<ClavierQuickItemPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERQUICKITEM_P_H
