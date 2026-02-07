import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Views

ApplicationWindow {
    id: main
    visible: true
    width: 96
    height: 640

    required property ClavierViewModel clavierViewModel
    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property ClavierInteractionController clavierInteractionController

    Clavier {
        width: parent.width
        height: parent.height
        clavierViewModel: main.clavierViewModel
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        clavierInteractionController: main.clavierInteractionController
    }

}