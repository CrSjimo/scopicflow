#ifndef SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_H
#define SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_H

#include <QObject>
#include <qqmlintegration.h>

namespace sflow {

    class RangeSequenceViewModel;
    class RubberBandLayerQuickItem;

    class PianoRollNoteAreaRubberBandHelperPrivate;

    class PianoRollNoteAreaRubberBandHelper : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(PianoRollNoteAreaRubberBandHelper)
        Q_PRIVATE_SLOT(d_func(), void handleItemUpdated())
        Q_PROPERTY(RangeSequenceViewModel *noteSequenceViewModel READ noteSequenceViewModel WRITE setNoteSequenceViewModel NOTIFY noteSequenceViewModelChanged)
        Q_PROPERTY(RubberBandLayerQuickItem *rubberBandLayer READ rubberBandLayer WRITE setRubberBandLayer NOTIFY rubberBandLayerChanged)
    public:
        explicit PianoRollNoteAreaRubberBandHelper(QObject *parent = nullptr);
        ~PianoRollNoteAreaRubberBandHelper() override;

        RangeSequenceViewModel *noteSequenceViewModel() const;
        void setNoteSequenceViewModel(RangeSequenceViewModel *noteSequenceViewModel);

        RubberBandLayerQuickItem *rubberBandLayer() const;
        void setRubberBandLayer(RubberBandLayerQuickItem *rubberBandLayer);

    signals:
        void noteSequenceViewModelChanged();
        void rubberBandLayerChanged();

    private:
        QScopedPointer<PianoRollNoteAreaRubberBandHelperPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_H
