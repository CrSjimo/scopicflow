import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import SVSCraft
import SVSCraft.UIComponents
import dev.sjimo.ScopicFlow.Views
import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

ApplicationWindow {
    id: main
    visible: true
    width: 1024
    height: 800
    LayoutMirroring.childrenInherit: true
    required property QtObject interactionControllerNotifier
    required property QtObject transactionControllerNotifier
    required property QtObject timeViewModel
    required property QtObject arrangementTimeViewModel
    required property QtObject timeLayoutViewModel
    required property QtObject arrangementTimeLayoutViewModel
    required property QtObject clavierViewModel
    required property QtObject trackListViewModel
    required property QtObject trackListLayoutViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject labelSequenceBehaviorViewModel
    required property QtObject arrangementLabelSequenceBehaviorViewModel
    required property QtObject playbackViewModel
    required property QtObject scrollBehaviorViewModel
    property QtObject noteSequenceViewModel: null
    required property QtObject pianoRollNoteAreaBehaviorViewModel
    required property QtObject backNoteSequenceViewModel
    required property QtObject backPianoRollNoteAreaBehaviorViewModel
    required property QtObject clipSequenceViewModel
    required property QtObject clipPaneBehaviorViewModel
    required property QtObject mixerLayoutViewModel
    required property QtObject busTrackListViewModel
    required property QtObject busMixerLayoutViewModel
    required property QtObject levelTimer
    required property QtObject anchoredCurveViewModel
    required property QtObject parameterRangeViewModel
    readonly property double minimumPanelSize: 100

    Menu {
        id: contextMenu
        width: 400
        property list<string> a: []
        Instantiator {
            model: contextMenu.a
            delegate: MenuItem {
                text: modelData
            }
            onObjectAdded: (index, object) => contextMenu.insertItem(index, object)
            onObjectRemoved: (index, object) => contextMenu.removeItem(object)
        }
    }

    Connections {
        target: main.interactionControllerNotifier
        function onItemInteracted (interactionType, model, index, containerModel, flags) {
            let interactionTypeText = ["II_Pressed", "II_Released", "II_Canceled", "II_HoverEntered", "II_HoverExited", "II_Clicked", "II_DoubleClicked", "II_ContextMenu", "II_ItemContextMenu"][interactionType]
            console.log(`Item interacted: type=${interactionTypeText}, model=${model}, index=${index}, containerModel=${containerModel}, flags=${flags}`)
            if (/ContextMenu/.test(interactionTypeText)) {
                contextMenu.a = [`Item Context Menu (${interactionTypeText})`, `Model: ${model}`, `Index: ${index}`, `Container Model: ${containerModel}`, `Flags: ${flags}`]
                contextMenu.popup()
            }
        }
        function onSceneInteracted (interactionType, sceneModel, behaviorModel, position, value, flags) {
            let interactionTypeText = ["II_Pressed", "II_Released", "II_Canceled", "II_HoverEntered", "II_HoverExited", "II_Clicked", "II_DoubleClicked", "II_ContextMenu", "II_ItemContextMenu"][interactionType]
            console.log(`Scene interacted: type=${interactionTypeText}, sceneModel=${sceneModel}, behaviorModel=${behaviorModel}, position=${position}, value=${value}, flags=${flags}`)
            if (/ContextMenu/.test(interactionTypeText)) {
                contextMenu.a = [`Scene Context Menu (${interactionTypeText})`, `Scene Model: ${sceneModel}`, `Behavior Model: ${behaviorModel}`, `Position: ${position}`, `Value: ${value}`, `Flags: ${flags}`]
                contextMenu.popup()
            }
        }
    }

    Connections {
        target: main.transactionControllerNotifier
        function onTransactionAboutToBegin() {
            console.log("Transaction about to begin")
        }
        function onTransactionCommitted() {
            console.log("Transaction committed")
        }
        function onTransactionAborted() {
            console.log("Transaction aborted")
        }
    }

    component SettingsPanel: ScrollView {
        id: settingsScrollView
        Item {
            implicitWidth: settingsScrollView.width
            implicitHeight: settingsColumnLayout.implicitHeight + 16
            ColumnLayout {
                id: settingsColumnLayout
                width: parent.width - 16
                anchors.centerIn: parent
                spacing: 8
                GroupBox {
                    title: "Alignment"
                    Layout.fillWidth: true
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        FormGroup {
                            label: "Piano Roll Position Alignment"
                            columnItem: SpinBox {
                                from: 1
                                to: 1920
                                value: main.timeLayoutViewModel.positionAlignment
                                onValueChanged: main.timeLayoutViewModel.positionAlignment = value
                            }
                            Layout.fillWidth: true
                        }
                        FormGroup {
                            label: "Arrangement Position Alignment"
                            columnItem: SpinBox {
                                from: 1
                                to: 1920
                                value: main.arrangementTimeLayoutViewModel.positionAlignment
                                onValueChanged: main.arrangementTimeLayoutViewModel.positionAlignment = value
                            }
                            Layout.fillWidth: true
                        }
                    }
                }
                GroupBox {
                    title: "Scroll"
                    Layout.fillWidth: true
                    ListModel {
                        id: modifierListModel
                        ListElement { value: Qt.AltModifier; text: "Alt" }
                        ListElement { value: Qt.ShiftModifier; text: "Shift" }
                        ListElement { value: Qt.ControlModifier; text: "Ctrl" }
                    }
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        FormGroup {
                            label: "Alternate Axis"
                            rowItem: ComboBox {
                                model: modifierListModel
                                textRole: "text"
                                valueRole: "value"
                                Component.onCompleted: currentIndex = indexOfValue(main.scrollBehaviorViewModel.alternateAxisModifier)
                                onActivated: main.scrollBehaviorViewModel.alternateAxisModifier = currentValue
                            }
                            Layout.fillWidth: true
                        }
                        FormGroup {
                            label: "Zoom"
                            rowItem: ComboBox {
                                model: modifierListModel
                                textRole: "text"
                                valueRole: "value"
                                Component.onCompleted: currentIndex = indexOfValue(main.scrollBehaviorViewModel.zoomModifier)

                                onActivated: main.scrollBehaviorViewModel.zoomModifier = currentValue
                            }
                            Layout.fillWidth: true
                        }
                        FormGroup {
                            label: "Page"
                            rowItem: ComboBox {
                                model: modifierListModel
                                textRole: "text"
                                valueRole: "value"
                                Component.onCompleted: currentIndex = indexOfValue(main.scrollBehaviorViewModel.pageModifier)
                                onActivated: main.scrollBehaviorViewModel.pageModifier = currentValue
                            }
                            Layout.fillWidth: true
                        }
                        FormGroup {
                            label: "Use page modifier as alternate axis zoom"
                            rowItem: Switch {
                                checked: main.scrollBehaviorViewModel.usePageModifierAsAlternateAxisZoom
                                onCheckedChanged: main.scrollBehaviorViewModel.usePageModifierAsAlternateAxisZoom = checked
                            }
                            Layout.fillWidth: true
                        }
                        FormGroup {
                            label: "Middle button drag"
                            rowItem: Switch {
                                checked: !main.scrollBehaviorViewModel.affectVelocity
                                onCheckedChanged: main.scrollBehaviorViewModel.affectVelocity = !checked
                            }
                            Layout.fillWidth: true
                        }
                        FormGroup {
                            label: "Pinch decomposed"
                            rowItem: Switch {
                                checked: main.scrollBehaviorViewModel.pinchDecomposed
                                onCheckedChanged: main.scrollBehaviorViewModel.pinchDecomposed = checked
                            }
                            Layout.fillWidth: true
                        }
                    }
                }
                GroupBox {
                    title: "Locale"
                    Layout.fillWidth: true
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        FormGroup {
                            label: "Mirroring"
                            rowItem: Switch {
                                checked: main.LayoutMirroring.enabled
                                onCheckedChanged: main.LayoutMirroring.enabled = checked
                            }
                            Layout.fillWidth: true
                        }
                    }
                }
                GroupBox {
                    title: "Level Meter Demo"
                    Layout.fillWidth: true
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        FormGroup {
                            label: "Enabled"
                            rowItem: Switch {
                                checked: main.levelTimer.active
                                onCheckedChanged: () => {
                                    if (checked)
                                        main.levelTimer.start()
                                    else
                                        main.levelTimer.stop()
                                }
                            }
                            Layout.fillWidth: true
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
    component ArrangementPanel: SplitView {
        orientation: Qt.Horizontal
        ColumnLayout {
            SplitView.preferredWidth: 360
            SplitView.minimumWidth: 256
            spacing: 0
            Item {
                Layout.fillWidth: true
                height: clipPane.y - arrangementTimeline.y
            }
            TrackList {
                id: trackList
                Layout.fillHeight: true
                Layout.fillWidth: true
                trackListViewModel: main.trackListViewModel
                trackListLayoutViewModel: main.trackListLayoutViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                interactionControllerNotifier: main.interactionControllerNotifier
                transactionControllerNotifier: main.transactionControllerNotifier
                trackExtraDelegate: Rectangle {
                    required property QtObject trackViewModel
                    anchors.fill: parent
                    clip: true
                    color: "red"
                    Text {
                        anchors.centerIn: parent
                        text: "Custom Track Extra Delegate\n" +
                            `name = ${parent.trackViewModel.name}\n` +
                            "msr = " +
                            (parent.trackViewModel.mute ? "[M]" : "") +
                            (parent.trackViewModel.solo ? "[S]" : "") +
                            (parent.trackViewModel.record ? "[R]" : "") + "\n" +
                            `gain = ${parent.trackViewModel.gain}\n` +
                            `pan = ${parent.trackViewModel.pan}\n` +
                            `selected = ${parent.trackViewModel.selected}\n` +
                            `intermediate = ${parent.trackViewModel.intermediate}`
                    }
                }
            }
        }
        ColumnLayout {
            spacing: 0
            Timeline {
                id: arrangementTimeline
                Layout.fillWidth: true

                timeViewModel: main.arrangementTimeViewModel
                timeLayoutViewModel: main.arrangementTimeLayoutViewModel
                playbackViewModel: main.playbackViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                interactionControllerNotifier: main.interactionControllerNotifier
            }
            LabelSequence {
                id: arrangementLabelSequence
                Layout.fillWidth: true

                timeViewModel: main.arrangementTimeViewModel
                timeLayoutViewModel: main.arrangementTimeLayoutViewModel
                playbackViewModel: main.playbackViewModel
                labelSequenceViewModel: main.labelSequenceViewModel
                labelSequenceBehaviorViewModel: main.arrangementLabelSequenceBehaviorViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                interactionControllerNotifier: main.interactionControllerNotifier
                transactionControllerNotifier: main.transactionControllerNotifier
            }
            ClipPane {
                id: clipPane
                Layout.fillWidth: true
                Layout.fillHeight: true
                timeViewModel: main.arrangementTimeViewModel
                timeLayoutViewModel: main.arrangementTimeLayoutViewModel
                playbackViewModel: main.playbackViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                trackListViewModel: main.trackListViewModel
                trackListLayoutViewModel: main.trackListLayoutViewModel
                clipSequenceViewModel: main.clipSequenceViewModel
                clipPaneBehaviorViewModel: main.clipPaneBehaviorViewModel

                clipGraph: ScopicFlowInternal.NoteThumbnail {
                    anchors.fill: parent
                    required property QtObject model
                    noteSequenceViewModel: model?.noteSequenceViewModel ?? null
                    position: model.clipStart
                    length: model.length
                }

                property QtObject currentClip: null
                Connections {
                    target: clipPane.currentClip
                    function onPositionChanged() {
                        main.pianoRollNoteAreaBehaviorViewModel.offset = target.position - target.clipStart
                    }
                    function onClipStartChanged() {
                        main.pianoRollNoteAreaBehaviorViewModel.offset = target.position - target.clipStart
                    }
                }

                Connections {
                    target: main.trackListViewModel?.handle.items[clipPane.currentClip?.trackNumber ?? 0] ?? null
                    function onColorChanged() {
                        main.pianoRollNoteAreaBehaviorViewModel.color = target.color
                    }
                }

                onClipDoubleClicked: (model) => {
                    currentClip = model
                    main.noteSequenceViewModel = model.noteSequenceViewModel
                    main.pianoRollNoteAreaBehaviorViewModel.offset = model.position - model.clipStart
                    main.pianoRollNoteAreaBehaviorViewModel.color = main.trackListViewModel.handle.items[model.trackNumber].color
                    main.timeViewModel.start = model.position
                }

            }
        }
    }
    component PianoRollPanel: GridLayout {
        columnSpacing: 0
        rowSpacing: 0
        LayoutMirroring.enabled: false

        Clavier {
            id: clavier
            Layout.row: 0
            Layout.rowSpan: 3
            Layout.column: 0
            Layout.minimumWidth: 96
            Layout.fillHeight: true

            clavierViewModel: main.clavierViewModel
            scrollBehaviorViewModel: main.scrollBehaviorViewModel
            interactionControllerNotifier: main.interactionControllerNotifier
            topExpansion: pianoRollSplitView.y - timeline.y
            bottomExpansion: pianoRollSplitView.height - pianoRollRealViewport.height
        }

        Timeline {
            id: timeline
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true

            timeViewModel: main.timeViewModel
            timeLayoutViewModel: main.timeLayoutViewModel
            playbackViewModel: main.playbackViewModel
            scrollBehaviorViewModel: main.scrollBehaviorViewModel
            interactionControllerNotifier: main.interactionControllerNotifier
        }

        LabelSequence {
            id: labelSequence
            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true

            timeViewModel: main.timeViewModel
            timeLayoutViewModel: main.timeLayoutViewModel
            playbackViewModel: main.playbackViewModel
            labelSequenceViewModel: main.labelSequenceViewModel
            labelSequenceBehaviorViewModel: main.labelSequenceBehaviorViewModel
            scrollBehaviorViewModel: main.scrollBehaviorViewModel
            interactionControllerNotifier: main.interactionControllerNotifier
            transactionControllerNotifier: main.transactionControllerNotifier
        }

        SplitView {
            id: pianoRollSplitView
            orientation: Qt.Vertical
            Layout.row: 2
            Layout.column: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            Item {
                id: pianoRollRealViewport
                SplitView.fillHeight: true
                PianoRoll {
                    id: pianoRoll
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: pianoRollSplitView.height

                    timeViewModel: main.timeViewModel
                    timeLayoutViewModel: main.timeLayoutViewModel
                    playbackViewModel: main.playbackViewModel
                    clavierViewModel: main.clavierViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel

                    viewItems: [
                        Component {
                            PianoRollNoteArea {
                                anchors.fill: parent
                                z: 2
                                timeViewModel: main.timeViewModel
                                timeLayoutViewModel: main.timeLayoutViewModel
                                clavierViewModel: main.clavierViewModel
                                noteSequenceViewModel: main.noteSequenceViewModel
                                pianoRollNoteAreaBehaviorViewModel: main.pianoRollNoteAreaBehaviorViewModel
                            }
                        },
                        Component {
                            PianoRollNoteArea {
                                anchors.fill: parent
                                opacity: 0.5
                                z: 1
                                timeViewModel: main.timeViewModel
                                timeLayoutViewModel: main.timeLayoutViewModel
                                clavierViewModel: main.clavierViewModel
                                noteSequenceViewModel: main.backNoteSequenceViewModel
                                pianoRollNoteAreaBehaviorViewModel: main.backPianoRollNoteAreaBehaviorViewModel
                            }
                        }
                    ]

                    bottomMargin: height - parent.height
                }
            }
            Pane {
                SplitView.preferredHeight: 128
                focusPolicy: Qt.StrongFocus
                background.opacity: 0.75
                padding: 0
                ParameterPanel {
                    anchors.fill: parent
                }
            }
        }
    }
    component MixerPanel: SplitView {
        orientation: Qt.Horizontal
        ThemedItem.dividerStroke: SVS.DS_Border
        Theme.splitterHandleSize: 2
        Mixer {
            SplitView.fillWidth: true
            trackListViewModel: main.trackListViewModel
            trackListLayoutViewModel: main.mixerLayoutViewModel
            scrollBehaviorViewModel: main.scrollBehaviorViewModel
            interactionControllerNotifier: main.interactionControllerNotifier
            transactionControllerNotifier: main.transactionControllerNotifier
        }
        Mixer {
            SplitView.maximumWidth: implicitWidth
            trackListViewModel: main.busTrackListViewModel
            trackListLayoutViewModel: main.busMixerLayoutViewModel
            scrollBehaviorViewModel: main.scrollBehaviorViewModel
            interactionControllerNotifier: main.interactionControllerNotifier
            transactionControllerNotifier: main.transactionControllerNotifier
        }
    }

    component ParameterPanel: ScopicFlowInternal.AnchoredCurve {
        timeViewModel: main.timeViewModel
        timeLayoutViewModel: main.timeLayoutViewModel
        anchoredCurveViewModel: main.anchoredCurveViewModel
        parameterRangeViewModel: main.parameterRangeViewModel
        strokeColors: [Qt.rgba(1, 1, 1, 1)]
        fillColor: Qt.rgba(1, 1, 1, 0.25)
        ScopicFlowInternal.AnchorEditor {
            anchors.fill: parent
            timeViewModel: main.timeViewModel
            timeLayoutViewModel: main.timeLayoutViewModel
            anchoredCurveViewModel: main.anchoredCurveViewModel
            parameterRangeViewModel: main.parameterRangeViewModel
            nodeColor: Qt.rgba(0.5, 0.5, 0.5, 1)
            nodeBorderColor: Qt.rgba(1, 1, 1, 1)
            nodeSelectedColor: Theme.accentColor
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        ThemedItem.splitHandleEnabled: rightDock.panelOpened

        SplitView {
            SplitView.minimumWidth: leftDock.SplitView.minimumWidth + main.minimumPanelSize
            SplitView.fillWidth: true
            ThemedItem.splitHandleEnabled: leftDock.panelOpened
            DockingView {
                id: leftDock
                property double preferredPanelSize: 400
                SplitView.minimumWidth: barSize + (panelOpened ? main.minimumPanelSize : 0)
                SplitView.preferredWidth: barSize + (panelOpened ? preferredPanelSize : 0)
                SplitView.maximumWidth: !panelOpened ? barSize : Infinity
                clip: true
                panelSize: width - barSize
                onPanelSizeChanged: () => {
                    if (panelSize > 0)
                        preferredPanelSize = panelSize
                }
                DockingPane {
                    title: "设置"
                    icon.source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/Settings16Filled.svg"
                    SettingsPanel {
                        anchors.fill: parent
                    }
                }
                DockingStretch {
                }
            }
            SplitView {
                id: middleSplitView
                orientation: Qt.Vertical
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                SplitView.minimumWidth: main.minimumPanelSize
                ThemedItem.splitHandleVisible: topDock.panelOpened || bottomDock.panelOpened
                Item {
                    SplitView.minimumHeight: !bottomDock.panelOpened ? middleSplitView.height - bottomDock.barSize - 1 : topDock.barSize + (topDock.panelOpened ? main.minimumPanelSize : 0)
                    SplitView.preferredHeight: (middleSplitView.height - 1) / 2
                    SplitView.maximumHeight: Math.max(SplitView.minimumHeight, !topDock.panelOpened ? topDock.barSize : Infinity)
                    DockingView {
                        id: topDock
                        width: parent.width
                        anchors.top: parent.top
                        edge: Qt.TopEdge
                        property double preferredPanelSize: 400
                        panelSize: parent.height - barSize
                        onPanelSizeChanged: () => {
                            if (panelSize > 0)
                                preferredPanelSize = panelSize
                        }
                        DockingPane {
                            title: "编曲面板"
                            icon.source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/GanttChart20Filled.svg"
                            Component.onCompleted: Docking.dockingView.showPane(this)
                            header: ToolBar {
                                Row {
                                    anchors.fill: parent
                                    spacing: 4
                                    ToolButton {
                                        text: "Pointer"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.clipPaneBehaviorViewModel.mouseBehavior === 1
                                        onClicked: main.clipPaneBehaviorViewModel.mouseBehavior = 1
                                    }
                                    ToolButton {
                                        text: "Pen"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.clipPaneBehaviorViewModel.mouseBehavior === 2
                                        onClicked: main.clipPaneBehaviorViewModel.mouseBehavior = 2
                                    }
                                    ToolButton {
                                        text: "Eraser"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.clipPaneBehaviorViewModel.mouseBehavior === 3
                                        onClicked: main.clipPaneBehaviorViewModel.mouseBehavior = 3
                                    }
                                    ToolButton {
                                        text: "Scissor"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.clipPaneBehaviorViewModel.mouseBehavior === 4
                                        onClicked: main.clipPaneBehaviorViewModel.mouseBehavior = 4
                                    }
                                }
                            }
                            ArrangementPanel {
                                anchors.fill: parent
                            }
                        }
                        DockingStretch {
                        }
                    }
                    Rectangle {
                        width: parent.width
                        height: 1
                        anchors.top: topDock.bottom
                        color: Theme.splitterColor
                        visible: !topDock.panelOpened
                    }
                }
                Item {
                    SplitView.minimumHeight: bottomDock.barSize + (bottomDock.panelOpened ? main.minimumPanelSize : 0)
                    SplitView.preferredHeight: (middleSplitView.height - 1) / 2
                    SplitView.maximumHeight: Math.max(SplitView.minimumHeight, !bottomDock.panelOpened ? bottomDock.barSize : Infinity)
                    DockingView {
                        id: bottomDock
                        width: parent.width
                        anchors.bottom: parent.bottom
                        edge: Qt.BottomEdge
                        property double preferredPanelSize: 400
                        panelSize: parent.height - barSize
                        onPanelSizeChanged: () => {
                            if (panelSize > 0)
                                preferredPanelSize = panelSize
                        }
                        DockingPane {
                            title: "钢琴卷帘"
                            icon.source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/Midi20Filled.svg"
                            Component.onCompleted: Docking.dockingView.showPane(this)
                            header: ToolBar {
                                Row {
                                    anchors.fill: parent
                                    spacing: 4
                                    ToolButton {
                                        text: "Pointer"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior === 1
                                        onClicked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior = 1
                                    }
                                    ToolButton {
                                        text: "Pen"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior === 2
                                        onClicked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior = 2
                                    }
                                    ToolButton {
                                        text: "Eraser"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior === 3
                                        onClicked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior = 3
                                    }
                                    ToolButton {
                                        text: "Scissor"
                                        checkable: true
                                        autoExclusive: true
                                        checked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior === 4
                                        onClicked: main.pianoRollNoteAreaBehaviorViewModel.mouseBehavior = 4
                                    }
                                }
                            }
                            PianoRollPanel {
                                anchors.fill: parent
                            }
                        }
                        DockingPane {
                            title: "混音器"
                            icon.source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/Midi20Filled.svg"
                            Component.onCompleted: Docking.dockingView.showPane(this)
                            header: ToolBar {
                                Row {
                                    anchors.fill: parent
                                    spacing: 4
                                    ToolButton {
                                        text: "Mute All"
                                        checkable: true
                                    }
                                    ToolButton {
                                        text: "Solo All"
                                        checkable: true
                                    }
                                    ToolButton {
                                        text: "Record All"
                                        checkable: true
                                    }
                                    ToolButton {
                                        text: "Exclusive Solo"
                                        checkable: true
                                    }
                                }
                            }
                            MixerPanel {
                                anchors.fill: parent
                            }
                        }
                        DockingStretch {
                        }
                    }
                    Rectangle {
                        width: parent.width
                        height: 1
                        anchors.bottom: bottomDock.top
                        color: Theme.splitterColor
                        visible: !bottomDock.panelOpened
                    }
                }
            }
        }

        DockingView {
            id: rightDock
            SplitView.minimumWidth: barSize + (panelOpened ? main.minimumPanelSize : 0)
            SplitView.preferredWidth: barSize + (panelOpened ? preferredPanelSize : 0)
            SplitView.maximumWidth: !panelOpened ? barSize : Infinity
            clip: true
            edge: Qt.RightEdge
            property double preferredPanelSize: 400
            panelSize: width - barSize
            onPanelSizeChanged: () => {
                if (panelSize > 0)
                    preferredPanelSize = panelSize
            }
            DockingStretch {
            }
            DockingPane {
                title: "设置"
                icon.source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/Settings16Filled.svg"
                SettingsPanel {
                    anchors.fill: parent
                }
            }
        }
    }

}