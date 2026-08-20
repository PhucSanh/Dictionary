pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary
ApplicationWindow {
    id: window
    minimumWidth: 890
    minimumHeight: 760


    visible: true
    title: qsTr("Từ điển Nhật - Việt")
    color: "#101014"

    palette {
        windowText: "white"
        text: "white"
        base: "#1c1c22"
    }

    EntryModel{
        id:entryModel
    }

    header: ToolBar {
        background: Rectangle { color: "#1c1c22" }
        contentItem: RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 8
            ToolButton {
                       visible: stackView.depth > 1
                       Layout.preferredWidth: 32
                       Layout.preferredHeight: 32
                       background: Rectangle {
                           radius: 4
                           color: parent.hovered ? "#2c2c34" : "transparent"
                           Behavior on color { ColorAnimation { duration: 120 } }
                       }
                       contentItem: Label {
                           text: "←"
                           font.pixelSize: 20
                           color: "#e0e0e6"
                           horizontalAlignment: Text.AlignHCenter
                           verticalAlignment: Text.AlignVCenter
                       }
                       onClicked: stackView.pop()
                   }
             Label {
                 visible: stackView.depth ===1
                 text: qsTr("Từ điển Nhật - Việt")
                 font.pixelSize: 18
                 font.bold: true
                 color: "white"
             }

            Item { Layout.fillWidth: true }
            Label {
                      visible: stackView.depth > 1
                      text: qsTr("Chi tiết")
                      font.pixelSize: 18
                      font.bold: true
                      color: "white"
                  }

            Label {
                visible: stackView.depth === 1 && entryModel.totalCount > 0
                text: entryModel.mode === EntryModel.ModeSearch
                      ? qsTr("%1 / %2 kết quả").arg(entryModel.count).arg(entryModel.totalCount)
                      : qsTr("%1 từ").arg(entryModel.count)
                font.pixelSize: 13
                color: "#a0a0a8"
            }
        }
    }

    footer: Frame {
            id: appFooter
            padding: 10
            background: Rectangle {
                color: "#18181c"
                Rectangle { width: parent.width; height: 1; color: "#2c2c34" }
            }
            contentItem: RowLayout {
                spacing: 6

                Label {
                    text: qsTr("Từ điển Nhật - Việt")
                    color: "#707078"
                    font.pixelSize: 11
                }
                Label {
                    text: "·"
                    color: "#3c3c44"
                    font.pixelSize: 11
                }
                Label {
                    text: qsTr("Slazenger app")
                    color: "#707078"
                    font.pixelSize: 11
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: qsTr("v0.1")
                    color: "#707078"
                    font.pixelSize: 11
                }
            }
        }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            if (stackView.depth > 1)
                stackView.pop()
        }
    }
    Shortcut {
        sequence: "Backspace"
        enabled: stackView.depth > 1
        onActivated: {
            stackView.pop()
            stackView.currentItem.focusSearch()
        }
    }
    Shortcut {
        sequence: "Down"
        enabled: stackView.depth === 1
        onActivated: stackView.currentItem.moveDown()
    }
    Shortcut {
        sequence: "Up"
        enabled: stackView.depth === 1
        onActivated: stackView.currentItem.moveUp()
    }

    StackView {
        id: stackView
        anchors.fill: parent
        anchors.margins: 12
        initialItem: searchPage
        onDepthChanged: {
           if(depth === 1 && entryModel.mode === EntryModel.ModeFavorites){
               entryModel.showFavorites();
           }
        }
    }
    Component{
        id:searchPage
        ColumnLayout {
            spacing: 8
            function focusSearch() {
                searchField.forceActiveFocus()
            }
            function moveDown() { listView.incrementCurrentIndex() }
            function moveUp()   { listView.decrementCurrentIndex() }
            function openCurrent() {
                if (listView.currentIndex < 0) return
                const it = listView.currentItem
                 if (!it) return
                entryModel.addHistory(it.entryId)
                stackView.push(detailPage, {
                    entryId: it.entryId, word: it.word, reading: it.reading,
                    romaji: it.romaji, partOfSpeech: it.part_of_speech,
                    meaning: it.meaning, english: it.english, level: it.level,
                    readingHira: it.reading_hira
                })
            }

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Tìm theo từ, romaji, kana, nghĩa...")
                onTextChanged: searchTimer.restart();
                Keys.onReturnPressed: openCurrent()
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 4
                spacing: 8
                visible: searchField.text.length === 0

                Label {
                    text: entryModel.mode === EntryModel.ModeFavorites
                        ? qsTr("Từ vựng yêu thích")
                        : qsTr("Lịch sử tra cứu")
                    font.pixelSize: 14
                    font.bold: true
                    color: "#a0a0a8"
                }

                Item { Layout.fillWidth: true }

                ToolButton {
                    Layout.preferredHeight: 28
                    background: Rectangle {
                        radius: 4
                        color: parent.hovered ? "#2c2c34" : "transparent"
                        Behavior on color { ColorAnimation { duration: 120 } }
                    }
                    contentItem: Label {
                        text: entryModel.mode === EntryModel.ModeFavorites
                            ? qsTr("← Lịch sử")
                            : qsTr("★ Yêu thích")
                        font.pixelSize: 13
                        color: "#e0e0e6"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        listView.currentIndex = 0;
                        if (entryModel.mode === EntryModel.ModeFavorites)
                            entryModel.showHistory()
                        else
                            entryModel.showFavorites()
                    }
                }
            }
            Component.onCompleted: entryModel.showHistory()
            Connections {
                target: stackView
                function onDepthChanged() {
                    if (stackView.depth === 1 && entryModel.mode === EntryModel.ModeFavorites) {
                        listView.currentIndex = 0;
                    }
                }
            }
            Timer{
                id: searchTimer
                interval: 250
                onTriggered: {
                    if(searchField.text === ""){
                        entryModel.clear();
                        entryModel.showHistory();
                    }
                   else  entryModel.search(searchField.text)}
            }
            ListView {
                id: listView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: entryModel
                spacing: 6
                currentIndex: -1

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
                BusyIndicator {
                    anchors.centerIn: parent
                    running: searchTimer.running
                    visible: running
                }

                delegate: Item {
                    id: delegateRoot
                    required property int index
                    required property string word
                    required property string reading
                    required property string romaji
                    required property string part_of_speech
                    required property string meaning
                    required property string level
                    required property int entryId
                    required property string english
                    required property string reading_hira
                    width: ListView.view.width
                    height: contentColumn.implicitHeight + 16
                    Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: delegateRoot.ListView.isCurrentItem
                            ? "#3a5a8c"
                            : (hoverArea.containsMouse ? "#26262e" : "transparent")

                        Behavior on color {
                            ColorAnimation { duration: 120 }
                        }
                                    }
                    ColumnLayout{
                        id: contentColumn
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 2
                        RowLayout{
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: delegateRoot.word
                                font.pixelSize: 22
                                font.bold: true
                                color: "White"

                            }
                            Item { Layout.fillWidth: true }
                            Label {

                                text:  {
                                    const text = delegateRoot.level.length>0? `Level: ${delegateRoot.level}`:'Level: Unknown';
                                    return text;
                                }
                                font.pixelSize: 11
                                color: "white"
                                padding: 3
                                background: Rectangle { color: "#2c2c34"; radius: 3 }
                            }

                        }
                        Label {
                            text: {
                                const r = delegateRoot.reading.length > 0
                                    ? `${delegateRoot.reading} · ${delegateRoot.romaji}`
                                    : delegateRoot.romaji
                                return `${r}   [${delegateRoot.part_of_speech}]`
                            }
                            font.pixelSize: 13
                            color: "#a0a0a8"
                        }
                        Label {
                            Layout.fillWidth: true
                            text: delegateRoot.meaning
                            font.pixelSize: 15
                            color: "#e0e0e6"
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }


                    }
                    MouseArea{
                       id: hoverArea
                       anchors.fill: parent
                       hoverEnabled: true
                       onClicked: {
                           listView.currentIndex = delegateRoot.index
                           openCurrent();
                       }

                    }

                }
                Label{
                   anchors.centerIn: parent
                   width: parent.width - 40
                   horizontalAlignment: Text.AlignHCenter
                   wrapMode: Text.WordWrap
                       visible: entryModel.count === 0 && !searchTimer.running
                       text: searchField.text.length === 0
                           ? qsTr("Nhập kanji, kana, romaji hoặc nghĩa tiếng Việt")
                           : qsTr("Không tìm thấy.\nThử gõ romaji, hoặc gõ nghĩa tiếng Việt")
                       color: "white"
                       font.pixelSize: 14
                }
                onAtYEndChanged: {
                    if (atYEnd && entryModel.hasMore &&  entryModel.count > 0)
                        entryModel.loadMore()
                }
                footer: Item {
                    width: ListView.view.width
                    height: entryModel.hasMore ? 40 : 0
                    visible: entryModel.hasMore && entryModel.count > 0
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("Đang tải thêm...")
                        color: "#707078"
                        font.pixelSize: 12
                    }
                }
            }
        }

    }
    Component {
        id: detailPage
        DetailPage { }
    }

}