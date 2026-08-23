pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary

Item {
    id: root
    property string pageTitle: qsTr("Flashcard")
    required property EntryModel entryModel
    property int limit: 0
    property var cards: []
    property int  idx: 0
    property bool revealed: false
    readonly property var card: (cards.length > 0 && idx < cards.length) ? cards[idx] : null

    Component.onCompleted: root.reload()
    StackView.onActivated: root.forceActiveFocus()

    function reload() {
        root.cards = root.entryModel.flashcards(root.limit)
        root.shuffle()
        root.forceActiveFocus()
    }

    function shuffle() {
        let a = root.cards.slice()
        for (let i = a.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1))
            const t = a[i]; a[i] = a[j]; a[j] = t
        }
        root.cards = a
        root.idx = 0
        root.revealed = false
    }
    function next() {
        if (root.cards.length === 0) return
        root.idx = (root.idx + 1) % root.cards.length
        root.revealed = false
    }
    function prev() {
        if (root.cards.length === 0) return
        root.idx = (root.idx - 1 + root.cards.length) % root.cards.length
        root.revealed = false
    }
    function flip() { root.revealed = !root.revealed }

    focus: true
    Keys.onSpacePressed:  root.flip()
    Keys.onReturnPressed: root.flip()
    Keys.onRightPressed:  root.next()
    Keys.onLeftPressed:   root.prev()

    Label {
        anchors.centerIn: parent
        width: parent.width - 60
        visible: root.cards.length === 0
        text: qsTr("Chưa có từ yêu thích nào.\nBấm ★ ở trang chi tiết để thêm.")
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        color: "#707078"
        font.pixelSize: 14
    }

    ColumnLayout {
        anchors.fill: parent
        visible: root.cards.length > 0
        spacing: 12

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("%1 / %2").arg(root.idx + 1).arg(root.cards.length)
            color: "#707078"
            font.pixelSize: 12
        }

        Rectangle {
            id: progressTrack
            Layout.fillWidth: true
            Layout.preferredHeight: 3
            radius: 2
            color: "#22222a"

            Rectangle {
                height: parent.height
                radius: 2
                color: "#3a5a8c"
                width: root.cards.length > 0
                       ? progressTrack.width * (root.idx + 1) / root.cards.length
                       : 0
                Behavior on width { NumberAnimation { duration: 140 } }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 10
            color: "#1c1c22"
            border.color: root.revealed ? "#3a5a8c" : "#2c2c34"
            border.width: 1
            Behavior on border.color { ColorAnimation { duration: 150 } }

            TapHandler {
                gesturePolicy: TapHandler.ReleaseWithinBounds
                onTapped: root.flip()
            }

            ScrollView {
                anchors.fill: parent
                anchors.margins: 24
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    width: parent.width
                    spacing: 10

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: root.revealed ? 0 : 60
                        text: root.card ? root.card.word : ""
                        font.pixelSize: root.revealed ? 40 : 56
                        font.bold: true
                        color: "white"
                        Behavior on font.pixelSize { NumberAnimation { duration: 120 } }
                    }
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        visible: !root.revealed
                        text: qsTr("bấm hoặc nhấn Space để lật")
                        color: "#4c4c54"
                        font.pixelSize: 12
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        visible: root.revealed
                        spacing: 8

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: root.card
                                ? `${root.card.reading} · ${root.card.romaji}   [${root.card.partOfSpeech}]`
                                : ""
                            color: "#a0a0a8"
                            font.pixelSize: 14
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: "#2c2c34"
                        }
                        Label {
                            Layout.fillWidth: true
                            text: root.card ? root.card.meaning : ""
                            color: "#e0e0e6"
                            font.pixelSize: 20
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Label {
                            Layout.fillWidth: true
                            visible: root.card && root.card.english.length > 0
                            text: root.card ? root.card.english : ""
                            color: "#707078"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Label {
                            Layout.topMargin: 10
                            visible: root.card && root.card.notes.length > 0
                            text: qsTr("Ghi chú của bạn")
                            color: "#707078"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Repeater {
                            model: root.card ? root.card.notes : []

                            Rectangle {
                                id: noteBox
                                required property var modelData

                                Layout.fillWidth: true
                                color: "#16161a"
                                radius: 6
                                implicitHeight: noteCol.implicitHeight + 20

                                ColumnLayout {
                                    id: noteCol
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 10
                                    spacing: 3

                                    Label {
                                        Layout.fillWidth: true
                                        visible: noteBox.modelData.japanese.length > 0
                                        text: noteBox.modelData.japanese
                                        color: "#e0e0e6"
                                        font.pixelSize: 15
                                        wrapMode: Text.WordWrap
                                    }
                                    Label {
                                        Layout.fillWidth: true
                                        visible: noteBox.modelData.translation.length > 0
                                        text: noteBox.modelData.translation
                                        color: "#a0a0a8"
                                        font.pixelSize: 13
                                        wrapMode: Text.WordWrap
                                    }
                                    Label {
                                        Layout.fillWidth: true
                                        visible: noteBox.modelData.note.length > 0
                                        text: "— " + noteBox.modelData.note
                                        color: "#c8a44a"
                                        font.pixelSize: 12
                                        wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: qsTr("← Trước")
                focusPolicy: Qt.NoFocus
                onClicked: { root.prev(); root.forceActiveFocus() }
            }
            Button {
                Layout.fillWidth: true
                text: root.revealed ? qsTr("Úp lại") : qsTr("Lật thẻ")
                focusPolicy: Qt.NoFocus
                onClicked: { root.flip(); root.forceActiveFocus() }
            }
            Button {
                text: qsTr("Sau →")
                focusPolicy: Qt.NoFocus
                onClicked: { root.next(); root.forceActiveFocus() }
            }
            Button {
                text: qsTr("↻ Xáo")
                focusPolicy: Qt.NoFocus
                onClicked: { root.shuffle(); root.forceActiveFocus() }
            }
        }
    }
}
