#ifndef SCOPIC_FLOW_CLAVIERINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_CLAVIERINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ClavierInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(AccidentalType accidentalType READ accidentalType WRITE setAccidentalType NOTIFY accidentalTypeChanged)
        Q_PROPERTY(LabelStrategy labelStrategy READ labelStrategy WRITE setLabelStrategy NOTIFY labelStrategyChanged)
        Q_PROPERTY(DisplayStyle displayStyle READ displayStyle WRITE setDisplayStyle NOTIFY displayStyleChanged)
    public:
        explicit ClavierInteractionController(QObject *parent = nullptr);
        ~ClavierInteractionController() override;

        enum AccidentalType {
            Flat,
            Sharp,
        };
        Q_ENUM(AccidentalType)

        enum LabelStrategy {
            LabelAll,
            LabelC,
            LabelNone,
        };
        Q_ENUM(LabelStrategy)

        enum DisplayStyle {
            Realistic,
            Simple,
        };
        Q_ENUM(DisplayStyle)

        void setAccidentalType(AccidentalType type);
        AccidentalType accidentalType() const;

        void setLabelStrategy(LabelStrategy strategy);
        LabelStrategy labelStrategy() const;

        void setDisplayStyle(DisplayStyle style);
        DisplayStyle displayStyle() const;

    Q_SIGNALS:
        void accidentalTypeChanged();
        void labelStrategyChanged();
        void displayStyleChanged();

        void hoverEntered(QQuickItem *clavier, int key);
        void hoverExited(QQuickItem *clavier, int key);
        void pressed(QQuickItem *clavier, int key);
        void released(QQuickItem *clavier, int key);
        void doubleClicked(QQuickItem *clavier, int key);

    private:
        AccidentalType m_accidentalType;
        LabelStrategy m_labelStrategy;
        DisplayStyle m_displayStyle;
    };

}

#endif //SCOPIC_FLOW_CLAVIERINTERACTIONCONTROLLER_H
