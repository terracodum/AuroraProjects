// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import "../utils/Utils.js" as Utils
import ru.auroraos.AntivirusDemo 1.0

Page {
    property bool isThreated: false

    readonly property string normalFileMessage: qsTr("The file is not a threat.")
    readonly property string threatedFileMessage: qsTr("The file is a threat.")

    function messageScanResult(state) {
        return state ? threatedFileMessage : normalFileMessage;
    }

    Timer {
        id: timer

        repeat: false
        interval: 2000

        onTriggered: {
            loader.sourceComponent = labelResult;
            client.publish(messageScanResult(isThreated));
        }
    }

    AVClient {
        id: client

        onScanFileResult: function (filePath, isThreatFile) {
            scanFile.text = filePath;
            isThreated = isThreatFile;
            timer.start();
        }
    }

    Item {
        id: mainItem

        anchors.fill: parent

        anchors {
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        Column {
            id: mainColumn

            width: parent.width

            spacing: Theme.paddingLarge

            PageHeader {
                id: header

                title: qsTr("Scan")
            }

            Label {
                id: scanFile

                width: parent.width
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                color: Theme.primaryColor

                text: Utils.getFileName(client.filePath)
            }

            Loader {
                id: loader

                width: parent.width
                height: 100

                sourceComponent: circleProgress
            }

            Component {
                id: circleProgress

                ProgressCircle {
                    id: progressCircle

                    Timer {
                        interval: 32
                        repeat: true
                        onTriggered: progressCircle.value = (progressCircle.value + 0.005) % 1.0
                        running: Qt.application.active
                    }
                }
            }

            Component {
                id: labelResult

                Label {
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: Theme.horizontalPageMargin
                        rightMargin: Theme.horizontalPageMargin
                    }

                    horizontalAlignment: Text.AlignHCenter

                    textFormat: Text.StyledText
                    wrapMode: Text.Wrap
                    color: isThreated ? "red" : "green"

                    text: messageScanResult(isThreated)
                }
            }

            Button {
                id: btnLog

                anchors.horizontalCenter: parent.horizontalCenter

                text: qsTr("Log")

                onClicked: {
                    pageStack.push("LogViewPage.qml");
                }
            }
        }
    }
}
