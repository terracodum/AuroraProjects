import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    objectName: "defaultCover"

    CoverTemplate {
        objectName: "applicationCover"
        primaryText: qsTr("Конвертер")
        secondaryText: qsTr("величин")
        icon {
            source: Qt.resolvedUrl("../icons/Converter.svg")
            sourceSize { width: icon.width; height: icon.height }
        }
    }
}
