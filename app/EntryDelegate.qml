import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
Item {
    id: root
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
    signal activated(int row)
    width: ListView.view ? ListView.view.width : 0
    height: contentColumn.implicitHeight + 16
    Rectangle {
        anchors.fill: parent
        radius: 6
        color: root.ListView.isCurrentItem
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
                text: root.word
                font.pixelSize: 22
                font.bold: true
                color: "White"

            }
            Item { Layout.fillWidth: true }
            Label {

                text:  {
                    const text = root.level.length>0? `Level: ${root.level}`:'Level: Unknown';
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
                const r = root.reading.length > 0
                    ? `${root.reading} · ${root.romaji}`
                    : root.romaji
                return `${r}   [${root.part_of_speech}]`
            }
            font.pixelSize: 13
            color: "#a0a0a8"
        }
        Label {
            Layout.fillWidth: true
            text: root.meaning
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
           listView.currentIndex = root.index
           openCurrent();
       }

    }
}
