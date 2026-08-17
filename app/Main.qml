pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary
ApplicationWindow {
    id: window
    minimumWidth: 420
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
    Timer{
        id: searchTimer
        interval: 250
        onTriggered: entryModel.search(searchField.text)
    }

    header: ToolBar {
        background: Rectangle { color: "#1c1c22" }
        contentItem: RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12

            Label {
                text: qsTr("Từ điển Nhật - Việt")
                font.pixelSize: 18
                font.bold: true
                color: "white"
            }
            Item { Layout.fillWidth: true }
            Label {
                visible: entryModel.totalCount>0
                text: qsTr("%1 kết quả / %2 tổng").arg(entryModel.count).arg(entryModel.totalCount)
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
    BusyIndicator {
        anchors.centerIn: parent
        running: searchTimer.running
        visible: running
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Tìm theo từ, romaji, kana, nghĩa...")
            onTextChanged: searchTimer.restart();
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

            delegate: Item {
                id: delegateRoot
                required property int index
                required property string word
                required property string reading
                required property string romaji
                required property string part_of_speech
                required property string meaning
                required property string level
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