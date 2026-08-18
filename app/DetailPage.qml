import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
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
    ScrollView {

            anchors.fill: parent
            contentWidth: availableWidth

            ColumnLayout {
                width: root.width
                spacing: 10

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

                Item { Layout.fillHeight: true }
            }
        }
}
