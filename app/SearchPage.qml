pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary
ColumnLayout {
    id: root
       property string pageTitle: qsTr("Từ điển Nhật - Việt")
       required property EntryModel entryModel

       signal entryActivated(var payload)
       signal flashcardRequested(int categoryId)

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
                       entryModel.showFavorites(entryModel.categoryFilter)
               }
           }

           ToolButton {
               id: flashcardButton
               Layout.preferredHeight: 28
               ToolTip.visible: flashcardButton.hovered
               ToolTip.text: qsTr("Học các từ đã đánh dấu ★ theo từng loại (Ctrl+L)")
               background: Rectangle {
                   radius: 4
                   color: flashcardButton.hovered ? "#2c2c34" : "transparent"
                   Behavior on color { ColorAnimation { duration: 120 } }
               }
               contentItem: Label {
                   text: qsTr("Flashcard ▸")
                   font.pixelSize: 13
                   color: "#e0e0e6"
                   horizontalAlignment: Text.AlignHCenter
                   verticalAlignment: Text.AlignVCenter
               }
               onClicked: root.flashcardRequested(entryModel.categoryFilter)
           }
       }

       RowLayout {
           Layout.fillWidth: true
           Layout.topMargin: 2
           spacing: 8
           visible: searchField.text.length === 0
                    && entryModel.mode === EntryModel.ModeFavorites

           Flickable {
               id: filterStrip
               Layout.fillWidth: true
               Layout.preferredHeight: 34
               contentWidth: filterRow.width
               clip: true
               flickableDirection: Flickable.HorizontalFlick
               boundsBehavior: Flickable.StopAtBounds

               Row {
                   id: filterRow
                   spacing: 6

                   Repeater {
                       model: [{ categoryId: 0, name: qsTr("Tất cả"), entryCount: 0 }]
                              .concat(root.entryModel.categories)

                       Rectangle {
                           id: filterChip
                           required property var modelData

                           readonly property bool selected:
                               root.entryModel.categoryFilter === filterChip.modelData.categoryId

                           width: chipLabel.implicitWidth + 24
                           height: 30
                           radius: 15
                           color: filterChip.selected ? "#3a5a8c" : "#26262e"
                           border.color: filterChip.selected ? "#5a7aac" : "#2c2c34"
                           border.width: 1

                           Behavior on color { ColorAnimation { duration: 120 } }

                           Label {
                               id: chipLabel
                               anchors.centerIn: parent
                               text: filterChip.modelData.entryCount > 0
                                     ? `${filterChip.modelData.name} (${filterChip.modelData.entryCount})`
                                     : filterChip.modelData.name
                               font.pixelSize: 13
                               color: filterChip.selected ? "white" : "#a0a0a8"
                           }

                           TapHandler {
                               onTapped: {
                                   listView.currentIndex = 0
                                   root.entryModel.showFavorites(filterChip.modelData.categoryId)
                               }
                           }
                       }
                   }
               }
           }

           ToolButton {
               id: manageCategoriesButton
               Layout.preferredWidth: 34
               Layout.preferredHeight: 30
               ToolTip.visible: manageCategoriesButton.hovered
               ToolTip.text: qsTr("Quản lý loại: thêm, đổi tên, xoá")
               background: Rectangle {
                   radius: 15
                   color: manageCategoriesButton.hovered ? "#3c3c46" : "#26262e"
                   border.color: "#2c2c34"
                   border.width: 1
               }
               contentItem: Label {
                   text: "⚙"
                   font.pixelSize: 15
                   color: "#a0a0a8"
                   horizontalAlignment: Text.AlignHCenter
                   verticalAlignment: Text.AlignVCenter
               }
               onClicked: manageDialog.openManage()
           }
       }

       CategoryDialog {
           id: manageDialog
           entryModel: root.entryModel
           onClosed: {
               listView.currentIndex = 0
               root.entryModel.showFavorites(root.entryModel.categoryFilter)
           }
       }

       StackView.onActivated: {
           if (searchField.text.length > 0)
               return

           if (entryModel.mode === EntryModel.ModeFavorites) {
               entryModel.showFavorites(entryModel.categoryFilter)
               listView.currentIndex = 0
           } else {
               entryModel.showHistory()
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
                  text: {
                      if (searchField.text.length > 0)
                          return qsTr("Không tìm thấy.\nThử gõ romaji, hoặc gõ nghĩa tiếng Việt")

                      if (entryModel.mode === EntryModel.ModeFavorites)
                          return entryModel.categoryFilter === 0
                              ? qsTr("Chưa có từ yêu thích nào.\nBấm ★ ở trang chi tiết, chọn loại rồi lưu.")
                              : qsTr("Chưa có từ nào trong loại này.\nBấm ★ ở trang chi tiết rồi chọn loại này.")

                      if (entryModel.mode === EntryModel.ModeHistory)
                          return qsTr("Chưa có lịch sử tra cứu.\nNhập kanji, kana, romaji hoặc nghĩa tiếng Việt")

                      return qsTr("Nhập kanji, kana, romaji hoặc nghĩa tiếng Việt")
                  }
                  color: "white"
                  font.pixelSize: 14
           }
           function maybeLoadMore() {
               if (listView.atYEnd && entryModel.hasMore && entryModel.count > 0)
                   Qt.callLater(entryModel.loadMore)
           }

           onAtYEndChanged: listView.maybeLoadMore()
           onCountChanged:  listView.maybeLoadMore()
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
