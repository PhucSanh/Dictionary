pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary
Item {
    id: root

    property int    entryId: 0
    property string word: ""
    property string reading: ""
    property string romaji: ""
    property string partOfSpeech: ""
    property string meaning: ""
    property string english: ""
    property string level: ""
    property bool isFav: false
    property var notes: []
    property var conjugations: []
    property string readingHira: ""
    property string favCategories: ""
    property string pageTitle: qsTr("Chi tiết")
    required property EntryModel entryModel
    function reloadNotes() {
        root.notes = entryModel.notesFor(root.entryId)
    }
    function reloadFavorite() {
        root.isFav = entryModel.isFavorite(root.entryId)
        root.favCategories = root.isFav ? entryModel.categoryNamesFor(root.entryId) : ""
    }
    Component.onCompleted: {
        root.reloadFavorite()
        root.reloadNotes()
        root.conjugations = entryModel.conjugationsFor(root.word, root.readingHira, root.partOfSpeech)
    }
    ScrollView {

            anchors.fill: parent
            contentWidth: availableWidth

            ColumnLayout {
                width: root.width
                spacing: 10
                Label {
                    Layout.topMargin: 16
                    Layout.fillWidth: true
                    visible: !root.isFav
                    wrapMode: Text.WordWrap
                    text: qsTr("Bấm ★ để chọn loại (N2, N3, Tiếng Nhật IT...) và lưu từ này vào yêu thích")
                    color: "#707078"
                    font.pixelSize: 13
                }
                RowLayout {
                    Layout.topMargin: 8
                    spacing: 8
                    Label {
                        text: root.word
                        font.pixelSize: 40
                        font.bold: true
                        color: "white"
                    }
                    Button {
                        text: qsTr("Sao chép từ")
                        onClicked: entryModel.copyToClipboard(root.word)
                    }
                    Button {
                        text: qsTr("Sao chép tất cả")
                        onClicked: entryModel.copyToClipboard(
                            `${root.word}\n${root.reading} · ${root.romaji}\n${root.meaning}`)
                    }
                    Item { Layout.fillWidth: true }


                    ToolButton {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            background: Rectangle {
                                radius: 4
                                color: parent.hovered ? "#2c2c34" : "transparent"
                                Behavior on color { ColorAnimation { duration: 120 } }
                            }
                            contentItem: Label {
                                text: root.isFav ? "★" : "☆"
                                font.pixelSize: 26
                                color: root.isFav ? "#f0c040" : "#707078"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onClicked: {
                                if (root.isFav) {
                                    entryModel.removeFavorite(root.entryId)
                                    root.reloadFavorite()
                                } else {
                                    categoryDialog.openFor(root.entryId, [])
                                }
                            }
                        }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: root.isFav
                    spacing: 8

                    Label {
                        text: qsTr("Loại")
                        font.pixelSize: 13
                        font.bold: true
                        color: "#707078"
                    }
                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: root.favCategories.length > 0
                              ? root.favCategories
                              : qsTr("Chưa phân loại")
                        font.pixelSize: 13
                        color: "#f0c040"
                        padding: 4
                        background: Rectangle { color: "#2a2418"; radius: 3 }
                    }
                    Button {
                        text: qsTr("Sửa loại")
                        onClicked: categoryDialog.openFor(
                            root.entryId, entryModel.categoryIdsFor(root.entryId))
                    }
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: {
                        const r = root.reading.length > 0
                            ? `${root.reading} · ${root.romaji}`
                            : root.romaji
                        return `${r}   [${root.partOfSpeech}]`
                    }
                    font.pixelSize: 15
                    color: "#a0a0a8"
                }

                Label {
                    visible: root.level.length > 0
                    text: root.level
                    font.pixelSize: 11
                    color: "white"
                    padding: 4
                    background: Rectangle { color: "#2c2c34"; radius: 3 }
                }

                Label {
                    Layout.topMargin: 12
                    text: qsTr("Nghĩa tiếng Việt")
                    font.pixelSize: 13
                    font.bold: true
                    color: "#707078"
                }

                Repeater {
                    model: root.meaning.split(";").map(s => s.trim()).filter(s => s.length > 0)

                    Label {
                        required property int index
                        required property string modelData

                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: `${index + 1}. ${modelData}`
                        font.pixelSize: 16
                        color: "#e0e0e6"
                    }
                }

                Label {
                    Layout.topMargin: 12
                    visible: root.english.length > 0
                    text: qsTr("Nghĩa tiếng Anh")
                    font.pixelSize: 13
                    font.bold: true
                    color: "#707078"
                }

                Repeater {
                    model: root.english.split(";").map(s => s.trim()).filter(s => s.length > 0)

                    Label {
                        required property int index
                        required property string modelData

                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: `${index + 1}. ${modelData}`
                        font.pixelSize: 16
                        color: "#c8c8d0"
                    }
                }

                RowLayout {
                    Layout.topMargin: 16
                    Layout.fillWidth: true
                    visible: root.conjugations.length > 0

                    Label {
                        text: qsTr("Chia động từ")
                        font.pixelSize: 13
                        font.bold: true
                        color: "#707078"
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: qsTr("Sao chép")
                        onClicked: {
                            let s = ""
                            for (let i = 0; i < root.conjugations.length; i++)
                                s += root.conjugations[i].name + "\t"
                                   + root.conjugations[i].text + "\n"
                            entryModel.copyToClipboard(s)
                        }
                    }
                }
                Repeater {
                    model: root.conjugations

                    RowLayout {
                        required property var modelData
                        Layout.fillWidth: true
                        spacing: 12

                        Label {
                            Layout.preferredWidth: 120
                            text: parent.modelData.name
                            font.pixelSize: 13
                            color: "#8a8a94"
                        }
                        Label {
                            Layout.fillWidth: true
                            text: parent.modelData.text
                            font.pixelSize: 16
                            color: "#e0e0e6"

                            wrapMode: TextEdit.Wrap
                        }
                    }
                }
                RowLayout {
                    Layout.topMargin: 16
                    Layout.fillWidth: true
                    visible: root.isFav

                    Label {
                        text: qsTr("Ghi chú của tôi")
                        font.pixelSize: 13
                        font.bold: true
                        color: "#707078"
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: qsTr("+ Thêm")
                        onClicked: noteDialog.openNew()
                    }
                }


                Repeater {
                    model: root.isFav ? root.notes : []

                    Rectangle {
                        id: card
                        required property var modelData

                        Layout.fillWidth: true
                        implicitHeight: cardCol.implicitHeight + 16
                        radius: 6
                        color: "#1c1c22"

                        ColumnLayout {
                            id: cardCol
                            x: 10
                            y: 10
                            width: card.width - 20
                            spacing: 4

                            Label {
                                Layout.fillWidth: true
                                visible: card.modelData.japanese.length > 0
                                wrapMode: Text.WordWrap
                                text: card.modelData.japanese
                                font.pixelSize: 16
                                color: "#e0e0e6"
                            }
                            Label {
                                Layout.fillWidth: true
                                visible: card.modelData.translation.length > 0
                                wrapMode: Text.WordWrap
                                text: card.modelData.translation
                                font.pixelSize: 14
                                color: "#a0a0a8"
                            }
                            Label {
                                Layout.fillWidth: true
                                visible: card.modelData.note.length > 0
                                wrapMode: Text.WordWrap
                                text: "※ " + card.modelData.note
                                font.pixelSize: 13
                                color: "#8a8a94"
                                font.italic: true
                            }

                            RowLayout {
                                Layout.topMargin: 4
                                Layout.fillWidth: true
                                spacing: 6

                                Item { Layout.fillWidth: true }

                                Button {
                                    text: qsTr("Sửa")
                                    implicitWidth: 64
                                    implicitHeight: 28
                                    flat: true
                                    contentItem: Label {
                                        text: parent.text
                                        font.pixelSize: 12
                                        color: "#a0a0a8"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    background: Rectangle {
                                        color: parent.hovered ? "#2c2c34" : "transparent"
                                        radius: 4
                                    }
                                    onClicked: noteDialog.openEdit(card.modelData)
                                }

                                Button {
                                    text: qsTr("Xoá")
                                    implicitWidth: 64
                                    implicitHeight: 28
                                    flat: true
                                    contentItem: Label {
                                        text: parent.text
                                        font.pixelSize: 12
                                        color: "#c07070"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    background: Rectangle {
                                        color: parent.hovered ? "#3a2626" : "transparent"
                                        radius: 4
                                    }
                                    onClicked: {
                                        entryModel.deleteNote(card.modelData.noteId)
                                        root.reloadNotes()
                                    }
                                }
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }

    CategoryDialog {
        id: categoryDialog
        entryModel: root.entryModel
        onConfirmed: (ids) => {
            if (root.isFav)
                root.entryModel.setFavoriteCategories(root.entryId, ids)
            else
                root.entryModel.addFavorite(root.entryId, ids)
            root.reloadFavorite()
        }
    }

    Dialog{
        id: noteDialog
        width: Math.min(root.width-60,560);
        padding: 28
        property int editingId: 0
        function openNew() {
                editingId = 0
                jpField.text = ""
                trField.text = ""
                noteField.text = ""
                open()
            }

            function openEdit(m) {
                editingId = m.noteId
                jpField.text = m.japanese
                trField.text = m.translation
                noteField.text = m.note
                open()
            }

            title: editingId === 0 ? qsTr("Thêm ghi chú") : qsTr("Sửa ghi chú")
            modal: true
            anchors.centerIn: Overlay.overlay


            background: Rectangle {
                  color: "#1c1c22"
                  radius: 8
                  border.color: "#2c2c34"
                  border.width: 1
              }
            header: Label {
                    text: noteDialog.title
                    font.pixelSize: 20
                    font.bold: true
                    color: "white"
                    padding: 20
                    bottomPadding: 8
                }
            Overlay.modal: Rectangle {
                  color: "#a0000000"
              }

            ColumnLayout {
                    anchors.fill: parent
                    spacing: 15

                    TextField {
                        id: jpField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Câu tiếng Nhật (tuỳ chọn)")
                        color: "white"
                        Keys.onReturnPressed: noteDialog.accept()
                            Keys.onEnterPressed:  noteDialog.accept()
                        Layout.preferredHeight: 48
                            font.pixelSize: 15
                        placeholderTextColor: "#707078"
                        background: Rectangle {
                            color: "#26262e"
                            radius: 4
                            border.color: parent.activeFocus ? "#3a5a8c" : "#2c2c34"
                            border.width: 1
                        }
                    }
                    TextField {
                        id: trField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Bản dịch (tuỳ chọn)")
                        color: "white"
                        Keys.onReturnPressed: noteDialog.accept()
                            Keys.onEnterPressed:  noteDialog.accept()
                        Layout.preferredHeight: 48
                            font.pixelSize: 15
                        placeholderTextColor: "#707078"
                        background: Rectangle {
                            color: "#26262e"
                            radius: 4
                            border.color: parent.activeFocus ? "#3a5a8c" : "#2c2c34"
                            border.width: 1
                        }
                    }
                    TextField {
                        id: noteField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Ghi chú về từ")
                        color: "white"
                        Keys.onReturnPressed: noteDialog.accept()
                            Keys.onEnterPressed:  noteDialog.accept()
                        Layout.preferredHeight: 48
                            font.pixelSize: 15
                        placeholderTextColor: "#707078"
                        background: Rectangle {
                            color: "#26262e"
                            radius: 4
                            border.color: parent.activeFocus ? "#3a5a8c" : "#2c2c34"
                            border.width: 1
                        }
                    }
                }
            footer: DialogButtonBox {
                  alignment: Qt.AlignRight
                  background: Rectangle { color: "transparent" }
                  padding: 20
                  topPadding: 8

                  Button {
                      text: qsTr("Huỷ")
                      flat: true
                      DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                      contentItem: Label {
                          text: parent.text
                          color: "#a0a0a8"
                          font.pixelSize: 15

                          horizontalAlignment: Text.AlignHCenter
                      }
                      background: Rectangle {
                          color: parent.hovered ? "#26262e" : "transparent"
                          radius: 4
                      }
                  }
                  Button {
                      text: qsTr("Lưu")
                      DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                      font.pixelSize: 10

                      contentItem: Label {
                          text: parent.text
                          color: "white"
                          font.pixelSize: 15
                          horizontalAlignment: Text.AlignHCenter
                      }
                      background: Rectangle {
                          color: parent.hovered ? "#4a6a9c" : "#3a5a8c"
                          radius: 4
                      }
                  }
              }
            onAccepted: {
                    if (jpField.text.length === 0 && noteField.text.length === 0)
                        return
                    if (editingId === 0)
                        entryModel.addNote(root.entryId, jpField.text, trField.text, noteField.text)
                    else
                        entryModel.updateNote(editingId, root.entryId,
                                              jpField.text, trField.text, noteField.text)
                    root.reloadNotes()
                }
    }

}
