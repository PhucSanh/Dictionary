import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
ColumnLayout {
    id: root
       property string pageTitle: qsTr("Từ điển Nhật - Việt")

       signal entryActivated(var payload)
       signal flashcardRequested()

       function focusSearch() { searchField.forceActiveFocus() }
       function moveDown()    { listView.incrementCurrentIndex() }
       function moveUp()      { listView.decrementCurrentIndex() }

       function openCurrent() {
           if (listView.currentIndex < 0) return
           const it = listView.currentItem
           if (!it) return
           entryModel.addHistory(it.entryId)
           root.entryActivated({
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
       Label {
           Layout.fillWidth: true
           Layout.topMargin: 4
           visible: entryModel.deinflectedFrom.length > 0
           wrapMode: Text.WordWrap
           text: qsTr("Dạng gốc của %1 → %2")
                   .arg(entryModel.deinflectedFrom)
                   .arg(entryModel.deinflectedTo)
           font.pixelSize: 13
           color: "#c0a060"
           padding: 6
           background: Rectangle {
               color: "#2a2418"
               radius: 4
           }
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

           delegate: EntryDelegate {
               onActivated: (row) => { listView.currentIndex = row; openCurrent() }
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
