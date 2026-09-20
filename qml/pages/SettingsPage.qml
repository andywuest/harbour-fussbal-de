import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Window 2.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Settings")
            }

            TextField {
                id: competitionIdField
                width: parent.width
                label: qsTr("Competition ID")
                placeholderText: qsTr("e.g. 125123")
                text: Window.window ? Window.window.competitionId : ""
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: focus = false
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Inactive) {
            if (Window.window)
                Window.window.competitionId = competitionIdField.text
        }
    }
}