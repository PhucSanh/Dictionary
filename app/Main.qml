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


    required property EntryModel entryModel

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
                      text: stackView.currentItem && stackView.currentItem.pageTitle
                            ? stackView.currentItem.pageTitle : ""
                      font.pixelSize: 18
                      font.bold: true
                      color: "white"
                  }

            Label {
                visible: stackView.depth === 1 && window.entryModel.totalCount > 0
                text: window.entryModel.mode === EntryModel.ModeSearch
                      ? qsTr("%1 / %2 kết quả").arg(window.entryModel.count).arg(window.entryModel.totalCount)
                      : qsTr("%1 từ").arg(window.entryModel.count)
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
    Shortcut {
        sequence: "Ctrl+L"
        enabled: stackView.depth === 1
        onActivated: stackView.push(flashcardPage, { categoryId: window.entryModel.categoryFilter })
    }

    StackView {
        id: stackView
        anchors.fill: parent
        anchors.margins: 12
        initialItem: searchPage

    }
    Component {
        id: searchPage
        SearchPage {
            entryModel: window.entryModel
            onEntryActivated: (p) => stackView.push(detailPage, p)
            onFlashcardRequested: (categoryId) => stackView.push(flashcardPage, { categoryId: categoryId })
        }
    }
    Component {
        id: detailPage
        DetailPage {
            entryModel: window.entryModel
        }
    }
    Component {
        id: flashcardPage
        FlashcardPage {
            entryModel: window.entryModel
        }
    }

}