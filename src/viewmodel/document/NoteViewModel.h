#ifndef SCOPIC_FLOW_NOTEVIEWMODEL_H
#define SCOPIC_FLOW_NOTEVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT NoteViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit NoteViewModel(QObject *parent = nullptr);
        ~NoteViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, position, position, setPosition, resetPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, length, length, setLength, resetLength)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, key, key, setKey, resetKey)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QString, lyric, lyric, setLyric, resetLyric)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, selected, isSelected, setSelected, resetSelected)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, intermediate, isIntermediate, setIntermediate, resetIntermediate)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, overlapped, isOverlapped, setOverlapped, resetOverlapped)

    };

}

#endif //SCOPIC_FLOW_NOTEVIEWMODEL_H
