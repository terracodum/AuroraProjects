// SPDX-FileCopyrightText: 2026 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0
import "../utils/Utils.js" as Utils
import ru.auroraos.AntivirusDemo 1.0

Page {
    id: page

    SilicaFlickable {
        id: flick

        anchors.fill: parent

        anchors {
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        Column {
            id: content
            width: parent.width

            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Antivirus Demo")
            }

            Label {
                id: labelFile

                width: parent.width

                anchors.horizontalCenter: parent.horizontalCenter

                text: Utils.getFileName("")
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                linkColor: Theme.primaryColor
            }

            Button {
                id: scanBtn

                anchors.horizontalCenter: parent.horizontalCenter

                text: qsTr("Select")

                onClicked: {
                    pageStack.push(filePicker);
                }
            }

            Item {
                height: Theme.paddingLarge
            }

            ButtonLayout {
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: btnScan

                    enabled: false

                    text: qsTr("Scan")

                    onClicked: {
                        pageStack.push("ScanPage.qml");
                        client.scanStarted(client.filePath);
                    }
                }

                Button {
                    id: btnLog

                    text: qsTr("Log")

                    onClicked: {
                        pageStack.push("LogViewPage.qml");
                    }
                }
            }
        }

        AVClient {
            id: client

            onFilePathChanged: {
                labelFile.text = Utils.getFileName(client.filePath);
                btnScan.enabled = true;
            }
        }

        Component {
            id: filePicker

            FilePickerPage {
                title: qsTr("Select file")

                showSystemFiles: false

                anchors.topMargin: SafeZoneRect.insets.top

                onSelectedContentChanged: {
                    client.filePath = selectedContent;
                }
            }
        }
    }

    Rectangle {
        width: Theme.iconSizeMedium + Theme.paddingMedium
        height: Theme.iconSizeMedium + Theme.paddingMedium
        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: Theme.paddingMedium
        }
        color: "transparent"

        IconButton {
            id: aboutButton
            anchors.fill: parent
            objectName: "aboutButton"
            icon {
                source: "image://theme/icon-m-about"
                sourceSize {
                    width: Theme.iconSizeMedium
                    height: Theme.iconSizeMedium
                }
            }
            onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
        }
    }
}
