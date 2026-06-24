import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0

Page {
    id: page
    objectName: "mainPage"
    allowedOrientations: Orientation.All

    property string sourcePath: ""
    readonly property bool hasResult: imageProcessor.resultSource !== "" && !imageProcessor.busy

    Component {
        id: imagePickerComponent
        ImagePickerPage {
            onSelectedContentPropertiesChanged: {
                page.sourcePath = selectedContentProperties.filePath
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingLarge

        VerticalScrollDecorator {}

        PullDownMenu {
            MenuItem {
                text: qsTr("About Application")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader { title: qsTr("Processor") }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Выбрать изображение")
                onClicked: pageStack.push(imagePickerComponent)
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Оригинал")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: page.sourcePath !== ""
            }

            Image {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                fillMode: Image.PreserveAspectFit
                source: page.sourcePath
                visible: page.sourcePath !== ""
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Обработать")
                enabled: page.sourcePath !== "" && !imageProcessor.busy
                onClicked: imageProcessor.process(page.sourcePath)
            }

            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                size: BusyIndicatorSize.Large
                running: imageProcessor.busy
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Результат")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: page.hasResult
            }

            Image {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                fillMode: Image.PreserveAspectFit
                cache: false                       // иначе второй результат не обновится
                source: imageProcessor.resultSource
                visible: page.hasResult
            }

            DetailItem {
                label: qsTr("Средняя яркость")
                value: imageProcessor.meanBrightness.toFixed(1) + " / 255"
                visible: page.hasResult
            }

            DetailItem {
                label: qsTr("Время обработки")
                value: imageProcessor.elapsedMs + " " + qsTr("мс")
                visible: page.hasResult
            }
        }
    }
}
