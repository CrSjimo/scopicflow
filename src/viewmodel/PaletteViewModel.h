#ifndef SCOPIC_FLOW_PALETTEVIEWMODEL_H
#define SCOPIC_FLOW_PALETTEVIEWMODEL_H

#include <QObject>
#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT PaletteViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(QObject *palette READ palette WRITE setPalette NOTIFY paletteChanged)
    public:
        explicit PaletteViewModel(QObject *parent = nullptr);
        ~PaletteViewModel() override;

        QObject *palette() const;
        void setPalette(QObject *palette);

    signals:
        void paletteChanged(QObject *palette);

    private:
        QObject *m_palette;
    };

} // sflow

#endif //SCOPIC_FLOW_PALETTEVIEWMODEL_H
