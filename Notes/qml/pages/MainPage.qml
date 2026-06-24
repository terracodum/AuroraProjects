import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    objectName: "mainPage"
    allowedOrientations: Orientation.All

    SilicaListView {
        id: listView
        anchors.fill: parent
        model: notesModel
        spacing: Theme.paddingMedium

        header: PageHeader {
            title: notesModel.count > 0
                   ? qsTr("Notes") + " (" + notesModel.count + ")"
                   : qsTr("Notes")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("New note")
                onClicked: pageStack.push(Qt.resolvedUrl("NewNotePage.qml"))
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No notes yet")
            hintText: qsTr("Pull down to create the first note")
        }

        delegate: ListItem {
            id: delegate
            width: listView.width
            contentHeight: card.height + Theme.paddingSmall
            // подсветку рисуем сами на карточке, штатную убираем
            _backgroundColor: "transparent"

            onClicked: pageStack.push(Qt.resolvedUrl("NotePage.qml"), {
                noteText: model.text,
                noteDate: model.createdAt
            })

            Rectangle {
                id: card
                anchors.verticalCenter: parent.verticalCenter
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: cardColumn.height + 2 * Theme.paddingLarge
                radius: Theme.paddingMedium
                color: delegate.highlighted
                       ? Theme.rgba(Theme.highlightBackgroundColor, 0.25)
                       : Theme.rgba(Theme.highlightBackgroundColor, 0.08)

                // тонкий цветной акцент слева
                Rectangle {
                    width: Theme.paddingSmall / 2
                    height: parent.height - Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    x: 0
                    radius: width
                    color: Theme.highlightColor
                }

                Column {
                    id: cardColumn
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: parent.right
                        leftMargin: Theme.paddingLarge
                        rightMargin: Theme.paddingLarge
                    }
                    spacing: Theme.paddingSmall

                    Label {
                        width: parent.width
                        text: model.text
                        wrapMode: Text.Wrap
                        maximumLineCount: 3
                        truncationMode: TruncationMode.Elide
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeMedium
                    }

                    Row {
                        spacing: Theme.paddingSmall
                        Image {
                            anchors.verticalCenter: parent.verticalCenter
                            source: "image://theme/icon-s-time"
                        }
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: Qt.formatDateTime(model.createdAt, "dd.MM.yyyy hh:mm")
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                        }
                    }
                }
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: remorseDelete(function() {
                        notesModel.removeNote(index)
                    })
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
