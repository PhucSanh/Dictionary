pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary

Item {
    id: root
    property string pageTitle: root.started
                               ? qsTr("Flashcard · %1").arg(root.deckName)
                               : qsTr("Tạo bộ thẻ")
    required property EntryModel entryModel
    property int limit: 0
    property int categoryId: 0
    property int selectedId: -1
    property bool started: false
    property string deckName: ""
    property var cards: []
    property int  idx: 0
    property bool revealed: false
    readonly property var card: (cards.length > 0 && idx < cards.length) ? cards[idx] : null

    Component.onCompleted: {
        if (root.categoryId > 0)
            root.selectedId = root.categoryId
    }
    StackView.onActivated: root.forceActiveFocus()

    function categoryOptions() {
        return [{ categoryId: 0,
                  name: qsTr("Tất cả từ yêu thích"),
                  entryCount: root.entryModel.favoriteCount(0) }]
               .concat(root.entryModel.categories)
    }

    function countOf(id) {
        if (id < 0)
            return 0
        if (id === 0)
            return root.entryModel.favoriteCount(0)
        const list = root.entryModel.categories
        for (let i = 0; i < list.length; i++)
            if (list[i].categoryId === id)
                return list[i].entryCount
        return 0
    }

    function nameOf(id) {
        if (id === 0)
            return qsTr("Tất cả")
        const list = root.entryModel.categories
        for (let i = 0; i < list.length; i++)
            if (list[i].categoryId === id)
                return list[i].name
        return qsTr("Tất cả")
    }

    function start() {
        if (root.selectedId < 0)
            return
        root.categoryId = root.selectedId
        root.deckName   = root.nameOf(root.selectedId)
        root.cards      = root.entryModel.flashcards(root.limit, root.categoryId)
        if (root.cards.length === 0)
            return
        root.started = true
        root.shuffle()
        root.forceActiveFocus()
    }

    function backToSetup() {
        root.started  = false
        root.cards    = []
        root.idx      = 0
        root.revealed = false
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
    Keys.onSpacePressed:  if (root.started) root.flip()
    Keys.onReturnPressed: root.started ? root.flip() : root.start()
    Keys.onRightPressed:  if (root.started) root.next()
    Keys.onLeftPressed:   if (root.started) root.prev()

    ColumnLayout {
        anchors.fill: parent
        visible: !root.started
        spacing: 12

        Label {
            Layout.fillWidth: true
            Layout.topMargin: 4
            text: qsTr("Chọn loại từ vựng để tạo bộ thẻ")
            font.pixelSize: 18
            font.bold: true
            color: "white"
        }
        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("Bộ thẻ chỉ gồm các từ yêu thích thuộc loại bạn chọn.")
            font.pixelSize: 13
            color: "#707078"
        }

        ScrollView {
            id: optionScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth

            Column {
                id: optionColumn
                width: optionScroll.availableWidth
                spacing: 6

                Repeater {
                    model: root.categoryOptions()

                    Rectangle {
                        id: option
                        required property var modelData

                        readonly property int  optionId: option.modelData.categoryId
                        readonly property int  optionCount: option.modelData.entryCount
                        readonly property bool empty: option.optionCount === 0
                        readonly property bool selected: root.selectedId === option.optionId

                        width: optionColumn.width
                        height: 52
                        radius: 8
                        color: option.selected ? "#2f4468" : "#1c1c22"
                        border.color: option.selected ? "#3a5a8c" : "#2c2c34"
                        border.width: 1
                        opacity: option.empty ? 0.45 : 1.0

                        Behavior on color { ColorAnimation { duration: 120 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 14
                            anchors.rightMargin: 14
                            spacing: 10

                            Rectangle {
                                Layout.preferredWidth: 18
                                Layout.preferredHeight: 18
                                radius: 9
                                color: "transparent"
                                border.color: option.selected ? "#5a7aac" : "#4c4c54"
                                border.width: 2

                                Rectangle {
                                    anchors.centerIn: parent
                                    visible: option.selected
                                    width: 9
                                    height: 9
                                    radius: 4.5
                                    color: "#7a9ada"
                                }
                            }
                            Label {
                                Layout.fillWidth: true
                                text: option.modelData.name
                                font.pixelSize: 16
                                color: option.selected ? "white" : "#c8c8d0"
                                elide: Text.ElideRight
                            }
                            Label {
                                text: option.empty
                                      ? qsTr("chưa có từ")
                                      : qsTr("%1 từ").arg(option.optionCount)
                                font.pixelSize: 12
                                color: option.selected ? "#c8d8f0" : "#707078"
                            }
                        }

                        TapHandler {
                            enabled: !option.empty
                            onTapped: root.selectedId = option.optionId
                        }
                    }
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: root.entryModel.favoriteCount(0) === 0
            wrapMode: Text.WordWrap
            text: qsTr("Bạn chưa có từ yêu thích nào.\nBấm ★ ở trang chi tiết, chọn loại rồi lưu.")
            horizontalAlignment: Text.AlignHCenter
            color: "#707078"
            font.pixelSize: 14
        }

        Button {
            id: startButton
            Layout.fillWidth: true
            Layout.preferredHeight: 46
            enabled: root.selectedId >= 0 && root.countOf(root.selectedId) > 0
            focusPolicy: Qt.NoFocus
            text: root.selectedId < 0
                  ? qsTr("Chọn một loại để bắt đầu")
                  : qsTr("Bắt đầu học · %1 thẻ").arg(root.countOf(root.selectedId))
            contentItem: Label {
                text: startButton.text
                font.pixelSize: 15
                font.bold: true
                color: startButton.enabled ? "white" : "#5c5c64"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 6
                color: startButton.enabled
                       ? (startButton.hovered ? "#4a6a9c" : "#3a5a8c")
                       : "#22222a"
            }
            onClicked: root.start()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        visible: root.started
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: root.deckName
                font.pixelSize: 14
                font.bold: true
                color: "#f0c040"
                padding: 4
                background: Rectangle { color: "#2a2418"; radius: 3 }
            }
            Label {
                text: qsTr("%1 / %2").arg(root.idx + 1).arg(root.cards.length)
                color: "#707078"
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true }
            ToolButton {
                id: changeDeckButton
                Layout.preferredHeight: 28
                focusPolicy: Qt.NoFocus
                background: Rectangle {
                    radius: 4
                    color: changeDeckButton.hovered ? "#2c2c34" : "transparent"
                }
                contentItem: Label {
                    text: qsTr("↺ Đổi loại")
                    font.pixelSize: 13
                    color: "#e0e0e6"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.backToSetup()
            }
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
                            visible: root.card && root.card.categories.length > 0
                            text: root.card ? root.card.categories : ""
                            color: "#f0c040"
                            font.pixelSize: 12
                            padding: 4
                            background: Rectangle { color: "#2a2418"; radius: 3 }
                        }
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
