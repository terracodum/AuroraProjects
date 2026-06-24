import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: dialog
    allowedOrientations: Orientation.All

    // нельзя принять пустой/пробельный текст
    canAccept: textArea.text.trim().length > 0

    // заметка добавляется только при принятии диалога
    onAccepted: notesModel.addNote(textArea.text)

    Column {
        width: parent.width

        DialogHeader {
            title: qsTr("New note")
            acceptText: qsTr("Save")
        }

        TextArea {
            id: textArea
            width: parent.width
            label: qsTr("Note")
            placeholderText: qsTr("Enter note text")
            focus: true
        }
    }
}
