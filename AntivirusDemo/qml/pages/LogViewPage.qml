// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import ru.auroraos.AntivirusDemo 1.0

Page {
    function updateLogView() {
        logArea.text = loggerModel.logData;
    }

    SilicaFlickable {
        id: flick

        anchors.fill: parent
        contentHeight: mainColumn.height

        LoggerModel {
            id: loggerModel

            Component.onCompleted: {
                updateLogView();
            }

            onLogDataChanged: {
                updateLogView();
            }
        }

        Column {
            id: mainColumn

            width: parent.width

            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Log")
            }

            TextArea {
                id: logArea

                width: parent.width

                readOnly: true

                VerticalScrollDecorator {
                }
            }
        }
    }
}
