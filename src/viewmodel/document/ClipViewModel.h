#ifndef SCOPIC_FLOW_CLIPVIEWMODEL_H
#define SCOPIC_FLOW_CLIPVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ClipViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit ClipViewModel(QObject *parent = nullptr);
        ~ClipViewModel() override;

        int position() const;
        int length() const;
        int clipStart() const;
        int maxLength() const;
        int trackNumber() const;
        QString name() const;
        bool selected() const;
        bool intermediate() const;
        bool overlapped() const;

    public slots:
        SFLOW_VIEWMODEL_SET_FUNC void setPosition(int position);
        SFLOW_VIEWMODEL_SET_FUNC void setLength(int length);
        SFLOW_VIEWMODEL_SET_FUNC void setClipStart(int clipStart);
        SFLOW_VIEWMODEL_SET_FUNC void setMaxLength(int maxLength);
        SFLOW_VIEWMODEL_SET_FUNC void setTrackNumber(int trackNumber);
        SFLOW_VIEWMODEL_SET_FUNC void setName(const QString &name);
        SFLOW_VIEWMODEL_SET_FUNC void setSelected(bool selected);
        SFLOW_VIEWMODEL_SET_FUNC void setIntermediate(bool intermediate);

    private slots:
        SFLOW_VIEWMODEL_SET_FUNC void setOverlapped(bool overlapped);

    signals:
        SFLOW_VIEWMODEL_SIGNAL void positionChanged(int position);
        SFLOW_VIEWMODEL_SIGNAL void lengthChanged(int length);
        SFLOW_VIEWMODEL_SIGNAL void clipStartChanged(int clipStart);
        SFLOW_VIEWMODEL_SIGNAL void maxLengthChanged(int maxLength);
        SFLOW_VIEWMODEL_SIGNAL void trackNumberChanged(int trackNumber);
        SFLOW_VIEWMODEL_SIGNAL void nameChanged(const QString &name);
        SFLOW_VIEWMODEL_SIGNAL void selectedChanged(bool selected);
        SFLOW_VIEWMODEL_SIGNAL void intermediateChanged(bool intermediate);
        SFLOW_VIEWMODEL_SIGNAL void overlappedChanged(bool overlapped);
    };

}

#endif //SCOPIC_FLOW_CLIPVIEWMODEL_H
