#ifndef SCOPIC_FLOW_CLAVIERQUICKITEM_P_H
#define SCOPIC_FLOW_CLAVIERQUICKITEM_P_H

#include <QQuickItem>
#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class ClavierViewModel;

    class SCOPIC_FLOW_EXPORT ClavierPalette : public QObject {
        Q_OBJECT
        Q_PROPERTY(QColor whiteKeyBackgroundColor READ whiteKeyBackgroundColor WRITE setWhiteKeyBackgroundColor NOTIFY whiteKeyBackgroundColorChanged)
        Q_PROPERTY(QColor whiteKeyForegroundColor READ whiteKeyForegroundColor WRITE setWhiteKeyForegroundColor NOTIFY whiteKeyForegroundColorChanged)
        Q_PROPERTY(QColor whiteKeyTextColor READ whiteKeyTextColor WRITE setWhiteKeyTextColor NOTIFY whiteKeyTextColorChanged)
        Q_PROPERTY(QColor blackKeyBackgroundColor READ blackKeyBackgroundColor WRITE setBlackKeyBackgroundColor NOTIFY blackKeyBackgroundColorChanged)
        Q_PROPERTY(QColor blackKeyForegroundColor READ blackKeyForegroundColor WRITE setBlackKeyForegroundColor NOTIFY blackKeyForegroundColorChanged)
        Q_PROPERTY(QColor blackKeyTextColor READ blackKeyTextColor WRITE setBlackKeyTextColor NOTIFY blackKeyTextColorChanged)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)

    public:
        explicit ClavierPalette(QObject *parent = nullptr);
        ~ClavierPalette() override;

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

    signals:
        void whiteKeyBackgroundColorChanged(const QColor &color);
        void whiteKeyForegroundColorChanged(const QColor &color);
        void whiteKeyTextColorChanged(const QColor &color);
        void blackKeyBackgroundColorChanged(const QColor &color);
        void blackKeyForegroundColorChanged(const QColor &color);
        void blackKeyTextColorChanged(const QColor &color);
        void borderColorChanged(const QColor &color);

    private:
        QColor m_whiteKeyBackgroundColor;
        QColor m_whiteKeyForegroundColor;
        QColor m_whiteKeyTextColor;
        QColor m_blackKeyBackgroundColor;
        QColor m_blackKeyForegroundColor;
        QColor m_blackKeyTextColor;
        QColor m_borderColor;
    };

    class ClavierQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT ClavierQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(ClavierQuickItem)
        Q_PROPERTY(ClavierPalette *palette READ palette CONSTANT)
        Q_PROPERTY(double keyHeight READ keyHeight NOTIFY keyHeightChanged)
        Q_PROPERTY(double viewportY READ viewportY NOTIFY viewportYChanged)
        Q_PROPERTY(int cursorNoteIndex READ cursorNoteIndex NOTIFY cursorNoteIndexChanged)
        Q_PROPERTY(sflow::ClavierQuickItem::LabelStrategy labelStrategy READ labelStrategy NOTIFY labelStrategyChanged)
        Q_PROPERTY(QString dummyKeyName READ dummyKeyName NOTIFY keyNameUpdated)
    public:
        explicit ClavierQuickItem(QQuickItem *parent = nullptr);
        ~ClavierQuickItem() override;

        ClavierPalette *palette() const;

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *clavierViewModel);

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

        QString dummyKeyName() const;

        Q_INVOKABLE void moveViewBy(double deltaY, bool isAnimated = false);
        Q_INVOKABLE void zoomOnWheel(double ratio, double centerY, bool animated = false);
        Q_INVOKABLE QString keyName(int key) const;

    signals:
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
