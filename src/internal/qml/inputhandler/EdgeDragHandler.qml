import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

DispatchedDragHandler {
    id: handler
    enum Edge {
        LeftEdge,
        RightEdge
    }

    property var controller: null
    property SelectionController selectionController: null
    property Item paneItem: null
    property var viewModel: null
    property TimeManipulator timeManipulator: null
    required property int edge
    property QtObject unitedExtendItem: null
    property int unitedExtendRestrict: 0
    CursorBinding.cursorShape: Qt.SizeHorCursor
    CursorBinding.enabled: dragged

    signal updateUnitedExtendRequested()

    onDragStarted: () => {
        controller.adjustLengthStarted(paneItem, viewModel)
        if (selectionController && controller.clickSelectable) {
            selectionController.selectByMouse(viewModel, Qt.RightButton, modifiers)
        }
        let united = modifiers & Qt.AltModifier
        if (united) {
            unitedExtendItem = null
            unitedExtendRestrict = 0
            updateUnitedExtendRequested()
        }
    }

    onDragMoved: (x) => {
        let parentX = mapToItem(paneItem, x, 0).x;
        edgeDragScroller.determine(parentX, paneItem.width, 0, 0, triggered => {
            if (!triggered) {
                let alignedTick = Math.max(timeManipulator.alignPosition(timeManipulator.mapToPosition(0), ScopicFlow.AO_Ceil), Math.min(timeManipulator.alignPosition(timeManipulator.mapToPosition(parentX)), timeManipulator.alignPosition(timeManipulator.mapToPosition(paneItem.width), ScopicFlow.AO_Floor)));
                edgeDragScroller.extendSelectionTo(alignedTick);
            }
        });
    }

    onDragFinished: () => {
        edgeDragScroller.running = false
        controller.adjustLengthCommitted(paneItem, viewModel)
    }

    onDragCanceled: () => {
        edgeDragScroller.running = false
        controller.adjustLengthCanceled(paneItem, viewModel)
    }

    DragScroller {
        id: edgeDragScroller

        function extendSelectionTo(position) {
            let selection = handler.selectionController.getSelectedItems()
            let hasClipStart = selection.length && typeof (selection[0].clipStart) === "number"
            let hasMaxLength = selection.length && typeof (selection[0].maxLength) === "number"
            if (handler.edge === EdgeDragHandler.LeftEdge) {
                if (position !== handler.viewModel.position) {
                    let deltaPosition = position -  handler.viewModel.position;
                    for (let note of selection) {
                        if (note.position + deltaPosition < 0)
                            return;
                        if (hasClipStart && note.clipStart + deltaPosition < 0)
                            return;
                        if (deltaPosition > note.length - handler.timeManipulator.timeLayoutViewModel.positionAlignment)
                            return;
                    }
                    if (unitedExtendRestrict) {
                        if (unitedExtendItem.position + unitedExtendItem.length === selection[0].position && unitedExtendItem.length + deltaPosition <= unitedExtendRestrict && unitedExtendItem.length + deltaPosition >= handler.timeManipulator.timeLayoutViewModel.positionAlignment) {
                            unitedExtendItem.length += deltaPosition;
                        }
                    }
                    for (let note of selection) {
                        note.position += deltaPosition;
                        if (hasClipStart)
                            note.clipStart += deltaPosition;
                        note.length -= deltaPosition;
                    }
                }
            } else if (handler.edge === EdgeDragHandler.RightEdge) {
                if (position !== handler.viewModel.position + handler.viewModel.length) {
                    let deltaPosition = position - (handler.viewModel.position + handler.viewModel.length);
                    for (let note of selection) {
                        if (note.position + note.length + deltaPosition < 0)
                            return;
                        if (note.position + note.length + deltaPosition < note.position + handler.timeManipulator.timeLayoutViewModel.positionAlignment)
                            return;
                        if (hasMaxLength && note.length + deltaPosition + (note.clipStart ?? 0) > note.maxLength)
                            return;
                    }
                    if (unitedExtendRestrict) {
                        if (unitedExtendItem.position === selection[0].position + selection[0].length && unitedExtendItem.length - deltaPosition <= unitedExtendRestrict && unitedExtendItem.length - deltaPosition >= handler.timeManipulator.timeLayoutViewModel.positionAlignment) {
                            unitedExtendItem.length -= deltaPosition;
                            unitedExtendItem.position += deltaPosition;
                        }
                    }
                    for (let note of selection) {
                        note.length += deltaPosition;
                    }
                }
            }
        }

        onMoved: function (deltaX) {
            handler.timeManipulator.moveViewBy(deltaX);
            let alignedTick = deltaX < 0 ? handler.timeManipulator.alignPosition(handler.timeManipulator.mapToPosition(0), ScopicFlow.AO_Ceil) : handler.timeManipulator.alignPosition(handler.timeManipulator.mapToPosition(handler.paneItem.width), ScopicFlow.AO_Floor);
            extendSelectionTo(alignedTick)
        }
    }
}