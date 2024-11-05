#ifndef SCOPIC_FLOW_CLAVIERWIDGET_H
#define SCOPIC_FLOW_CLAVIERWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class ClavierViewModel;
    class WheelModifierViewModel;

    class ClavierQuickItem;

    class SCOPIC_FLOW_EXPORT ClavierWidget : public QWidget {
        Q_OBJECT
        Q_PROPERTY(QColor whiteKeyBackgroundColor READ whiteKeyBackgroundColor WRITE setWhiteKeyBackgroundColor)
        Q_PROPERTY(QColor whiteKeyForegroundColor READ whiteKeyForegroundColor WRITE setWhiteKeyForegroundColor)
        Q_PROPERTY(QColor whiteKeyTextColor READ whiteKeyTextColor WRITE setWhiteKeyTextColor)
        Q_PROPERTY(QColor blackKeyBackgroundColor READ blackKeyBackgroundColor WRITE setBlackKeyBackgroundColor)
        Q_PROPERTY(QColor blackKeyForegroundColor READ blackKeyForegroundColor WRITE setBlackKeyForegroundColor)
        Q_PROPERTY(QColor blackKeyTextColor READ blackKeyTextColor WRITE setBlackKeyTextColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)

    public:
        explicit ClavierWidget(QWidget *parent = nullptr);
        ~ClavierWidget() override;

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *viewModel);

        WheelModifierViewModel *wheelModifierViewModel() const;
        void setWheelModifierViewModel(WheelModifierViewModel *viewModel);

        enum LabelStrategy {
            None,
            C,
            All,
        };
        Q_ENUM(LabelStrategy)
        LabelStrategy labelStrategy() const;
        void setLabelStrategy(LabelStrategy strategy);

        int mapToKey(double y) const;

    private:
        ClavierQuickItem *d;

        QColor whiteKeyBackgroundColor() const;
        void setWhiteKeyBackgroundColor(const QColor &color);
        QColor whiteKeyForegroundColor() const;
        void setWhiteKeyForegroundColor(const QColor &color);
        QColor whiteKeyTextColor() const;
        void setWhiteKeyTextColor(const QColor &color);
        QColor blackKeyBackgroundColor() const;
        void setBlackKeyBackgroundColor(const QColor &color);
        QColor blackKeyForegroundColor() const;
        void setBlackKeyForegroundColor(const QColor &color);
        QColor blackKeyTextColor() const;
        void setBlackKeyTextColor(const QColor &color);
        QColor borderColor() const;
        void setBorderColor(const QColor &color);
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERWIDGET_H
