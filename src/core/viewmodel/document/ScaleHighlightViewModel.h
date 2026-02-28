#ifndef SCOPIC_FLOW_SCALEHIGHLIGHTVIEWMODEL_H
#define SCOPIC_FLOW_SCALEHIGHLIGHTVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ScaleHighlightViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
        Q_PROPERTY(int cMask READ cMask WRITE setCMask NOTIFY cMaskChanged)
    public:
        explicit ScaleHighlightViewModel(QObject *parent = nullptr);
        ~ScaleHighlightViewModel() override;

        int position() const;
        void setPosition(int position);

        int length() const;
        void setLength(int length);

        int cMask() const;
        void setCMask(int cMask);

    Q_SIGNALS:
        void positionChanged();
        void lengthChanged();
        void cMaskChanged();

    private:
        int m_position;
        int m_length;
        int m_cMask;
    };

}

#endif //SCOPIC_FLOW_SCALEHIGHLIGHTVIEWMODEL_H
