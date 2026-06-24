import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    function convert(text, index) {
        var value = parseFloat(text)
        if (isNaN(value) || text === "") return "—"

        var result
        if (index === 0) {
            result = value * 9 / 5 + 32
        } else if (index === 1) {
            result = (value - 32) * 5 / 9
        } else {
            result = value + 273.15
        }

        return result.toFixed(2)
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("О приложении")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Очистить")
                onClicked: inputField.text = ""
            }
        }

        PageHeader {
            id: header
            title: qsTr("Конвертер величин")
        }

        Row {
            anchors {
                top: header.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
            }

            // Левая колонка — ввод и выбор направления.
            // Прибита к верху, чтобы при открытии клавиатуры поле не уезжало.
            Column {
                width: parent.width / 2
                spacing: Theme.paddingLarge
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingLarge

                TextField {
                    id: inputField
                    width: parent.width
                    placeholderText: qsTr("Введите число")
                    label: qsTr("Значение для конвертации")
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                }

                ComboBox {
                    id: conversionBox
                    width: parent.width
                    label: qsTr("Направление конвертации")

                    menu: ContextMenu {
                        MenuItem { text: qsTr("°C → °F") }
                        MenuItem { text: qsTr("°F → °C") }
                        MenuItem { text: qsTr("°C → K") }
                    }
                }
            }

            // Правая колонка — крупный результат
            Item {
                width: parent.width / 2
                height: parent.height

                Label {
                    anchors.centerIn: parent
                    width: parent.width - 2 * Theme.paddingLarge
                    font.pixelSize: Theme.fontSizeHuge
                    color: Theme.highlightColor
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: page.convert(inputField.text, conversionBox.currentIndex)
                }
            }
        }
    }
}
