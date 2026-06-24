import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    objectName: "defaultCover"

    Column {
        anchors.centerIn: parent
        width: parent.width
        spacing: Theme.paddingSmall

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: notesModel.count
            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeHuge
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: notesModel.count === 1 ? qsTr("note") : qsTr("notes")
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeSmall
        }
    }
}
