pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Dictionary

Dialog {
    id: control

    required property EntryModel entryModel
    property int  entryId: 0
    property var  selectedIds: []
    property var  categoryList: []
    property bool editing: false
    property bool manageOnly: false

    signal confirmed(var ids)

    function openFor(id, preselected) {
        control.entryId     = id
        control.manageOnly  = false
        control.editing     = preselected !== undefined && preselected !== null
                              && preselected.length > 0
        control.selectedIds = control.editing ? preselected.slice() : []
        control.reload()
        newField.text = ""
        control.open()
    }

    function openManage() {
        control.entryId     = 0
        control.manageOnly  = true
        control.editing     = false
        control.selectedIds = []
        control.reload()
        newField.text = ""
        control.open()
    }

    function reload() {
        control.categoryList = control.entryModel.categories
    }

    function isChecked(id) {
        return control.selectedIds.indexOf(id) !== -1
    }

    function toggle(id, on) {
        let ids = control.selectedIds.slice()
        const at = ids.indexOf(id)
        if (on && at === -1)
            ids.push(id)
        else if (!on && at !== -1)
            ids.splice(at, 1)
        control.selectedIds = ids
    }

    function createCategory() {
        const name = newField.text.trim()
        if (name.length === 0)
            return
        const id = control.entryModel.addCategory(name)
        newField.text = ""
        control.reload()
        if (id > 0)
            control.toggle(id, true)
    }

    function renameCategory(id, name) {
        const trimmed = name.trim()
        if (trimmed.length === 0)
            return
        control.entryModel.renameCategory(id, trimmed)
        control.reload()
    }

    function removeCategory(id) {
        control.entryModel.deleteCategory(id)
        control.toggle(id, false)
        control.reload()
    }

    title: control.manageOnly
           ? qsTr("Quản lý loại từ vựng")
           : (control.editing ? qsTr("Sửa loại của từ") : qsTr("Từ này thuộc loại nào?"))
    modal: true
    width: Math.min(parent ? parent.width - 60 : 540, 540)
    padding: 24
    anchors.centerIn: Overlay.overlay

    background: Rectangle {
        color: "#1c1c22"
        radius: 8
        border.color: "#2c2c34"
        border.width: 1
    }
    Overlay.modal: Rectangle {
        color: "#a0000000"
    }
    header: ColumnLayout {
        spacing: 2
        Label {
            text: control.title
            font.pixelSize: 20
            font.bold: true
            color: "white"
            leftPadding: 20
            topPadding: 20
        }
        Label {
            text: control.manageOnly
                  ? qsTr("Thêm loại mới · bấm ✎ để đổi tên · 🗑 để xoá loại")
                  : qsTr("Chọn ít nhất một loại · bấm ✎ để đổi tên · 🗑 để xoá loại")
            font.pixelSize: 12
            color: "#707078"
            leftPadding: 20
            bottomPadding: 8
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        ScrollView {
            id: listScroll
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(rowColumn.implicitHeight, 300)
            visible: control.categoryList.length > 0
            clip: true
            contentWidth: availableWidth

            Column {
                id: rowColumn
                width: listScroll.availableWidth
                spacing: 4

                Repeater {
                    model: control.categoryList

                    Rectangle {
                        id: row
                        required property var modelData

                        readonly property int  categoryId: row.modelData.categoryId
                        readonly property bool checked: control.isChecked(row.categoryId)
                        property string rowMode: "view"

                        width: rowColumn.width
                        height: 44
                        radius: 6
                        color: row.checked && row.rowMode === "view" ? "#2f4468" : "#22222a"
                        border.color: row.checked && row.rowMode === "view" ? "#3a5a8c" : "#2c2c34"
                        border.width: 1

                        Behavior on color { ColorAnimation { duration: 120 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 8
                            spacing: 6
                            visible: row.rowMode === "view"

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 8

                                    Rectangle {
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                        visible: !control.manageOnly
                                        radius: 4
                                        color: row.checked ? "#3a5a8c" : "transparent"
                                        border.color: row.checked ? "#5a7aac" : "#4c4c54"
                                        border.width: 1

                                        Label {
                                            anchors.centerIn: parent
                                            visible: row.checked
                                            text: "✓"
                                            font.pixelSize: 13
                                            color: "white"
                                        }
                                    }
                                    Label {
                                        Layout.fillWidth: true
                                        text: row.modelData.name
                                        font.pixelSize: 15
                                        color: row.checked ? "white" : "#c8c8d0"
                                        elide: Text.ElideRight
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    Label {
                                        text: qsTr("%1 từ").arg(row.modelData.entryCount)
                                        font.pixelSize: 11
                                        color: "#707078"
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                }

                                TapHandler {
                                    enabled: !control.manageOnly
                                    onTapped: control.toggle(row.categoryId, !row.checked)
                                }
                            }

                            ToolButton {
                                id: editButton
                                Layout.preferredWidth: 30
                                Layout.preferredHeight: 30
                                ToolTip.visible: editButton.hovered
                                ToolTip.text: qsTr("Đổi tên")
                                background: Rectangle {
                                    radius: 4
                                    color: editButton.hovered ? "#3c3c46" : "transparent"
                                }
                                contentItem: Label {
                                    text: "✎"
                                    font.pixelSize: 15
                                    color: "#a0a0a8"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    nameField.text = row.modelData.name
                                    row.rowMode = "edit"
                                    nameField.forceActiveFocus()
                                    nameField.selectAll()
                                }
                            }
                            ToolButton {
                                id: deleteButton
                                Layout.preferredWidth: 30
                                Layout.preferredHeight: 30
                                ToolTip.visible: deleteButton.hovered
                                ToolTip.text: qsTr("Xoá loại")
                                background: Rectangle {
                                    radius: 4
                                    color: deleteButton.hovered ? "#3a2626" : "transparent"
                                }
                                contentItem: Label {
                                    text: "🗑"
                                    font.pixelSize: 14
                                    color: "#c07070"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: row.rowMode = "confirm"
                            }
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 8
                            spacing: 6
                            visible: row.rowMode === "edit"

                            TextField {
                                id: nameField
                                Layout.fillWidth: true
                                Layout.preferredHeight: 32
                                color: "white"
                                font.pixelSize: 14
                                placeholderText: qsTr("Tên loại")
                                placeholderTextColor: "#707078"
                                background: Rectangle {
                                    color: "#26262e"
                                    radius: 4
                                    border.color: "#3a5a8c"
                                    border.width: 1
                                }
                                Keys.onReturnPressed: {
                                    control.renameCategory(row.categoryId, nameField.text)
                                    row.rowMode = "view"
                                }
                                Keys.onEnterPressed: {
                                    control.renameCategory(row.categoryId, nameField.text)
                                    row.rowMode = "view"
                                }
                                Keys.onEscapePressed: row.rowMode = "view"
                            }
                            ToolButton {
                                id: saveNameButton
                                Layout.preferredWidth: 30
                                Layout.preferredHeight: 30
                                enabled: nameField.text.trim().length > 0
                                background: Rectangle {
                                    radius: 4
                                    color: saveNameButton.hovered ? "#3c3c46" : "transparent"
                                }
                                contentItem: Label {
                                    text: "✓"
                                    font.pixelSize: 15
                                    color: saveNameButton.enabled ? "#7ac07a" : "#4c4c54"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    control.renameCategory(row.categoryId, nameField.text)
                                    row.rowMode = "view"
                                }
                            }
                            ToolButton {
                                id: cancelNameButton
                                Layout.preferredWidth: 30
                                Layout.preferredHeight: 30
                                background: Rectangle {
                                    radius: 4
                                    color: cancelNameButton.hovered ? "#3c3c46" : "transparent"
                                }
                                contentItem: Label {
                                    text: "✕"
                                    font.pixelSize: 14
                                    color: "#a0a0a8"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: row.rowMode = "view"
                            }
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 8
                            spacing: 6
                            visible: row.rowMode === "confirm"

                            Label {
                                Layout.fillWidth: true
                                text: qsTr("Xoá \"%1\"? Các từ sẽ mất nhãn này.").arg(row.modelData.name)
                                font.pixelSize: 13
                                color: "#c8c8d0"
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }
                            Button {
                                id: confirmDeleteButton
                                implicitWidth: 62
                                implicitHeight: 28
                                text: qsTr("Xoá")
                                contentItem: Label {
                                    text: confirmDeleteButton.text
                                    font.pixelSize: 13
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle {
                                    radius: 4
                                    color: confirmDeleteButton.hovered ? "#a04848" : "#8c3a3a"
                                }
                                onClicked: {
                                    row.rowMode = "view"
                                    control.removeCategory(row.categoryId)
                                }
                            }
                            Button {
                                id: cancelDeleteButton
                                implicitWidth: 62
                                implicitHeight: 28
                                text: qsTr("Huỷ")
                                flat: true
                                contentItem: Label {
                                    text: cancelDeleteButton.text
                                    font.pixelSize: 13
                                    color: "#a0a0a8"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle {
                                    radius: 4
                                    color: cancelDeleteButton.hovered ? "#2c2c34" : "transparent"
                                }
                                onClicked: row.rowMode = "view"
                            }
                        }
                    }
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: control.categoryList.length === 0
            wrapMode: Text.WordWrap
            text: qsTr("Chưa có loại nào. Tạo loại đầu tiên ở ô bên dưới.")
            font.pixelSize: 13
            color: "#707078"
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: newField
                Layout.fillWidth: true
                Layout.preferredHeight: 42
                placeholderText: qsTr("Tạo loại mới, ví dụ: Kanji N2, IT Web...")
                color: "white"
                font.pixelSize: 14
                placeholderTextColor: "#707078"
                Keys.onReturnPressed: control.createCategory()
                Keys.onEnterPressed:  control.createCategory()
                background: Rectangle {
                    color: "#26262e"
                    radius: 4
                    border.color: newField.activeFocus ? "#3a5a8c" : "#2c2c34"
                    border.width: 1
                }
            }
            Button {
                id: addButton
                text: qsTr("Thêm loại")
                enabled: newField.text.trim().length > 0
                implicitHeight: 42
                contentItem: Label {
                    text: addButton.text
                    color: addButton.enabled ? "white" : "#5c5c64"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 4
                    color: addButton.enabled
                           ? (addButton.hovered ? "#3c3c46" : "#2c2c34")
                           : "#22222a"
                }
                onClicked: control.createCategory()
            }
        }
    }

    footer: DialogButtonBox {
        alignment: Qt.AlignRight
        background: Rectangle { color: "transparent" }
        padding: 20
        topPadding: 8

        Button {
            id: rejectButton
            text: qsTr("Huỷ")
            flat: true
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            contentItem: Label {
                text: rejectButton.text
                color: "#a0a0a8"
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
            }
            background: Rectangle {
                radius: 4
                color: rejectButton.hovered ? "#26262e" : "transparent"
            }
        }
        Button {
            id: acceptButton
            text: control.manageOnly
                  ? qsTr("Xong")
                  : (control.editing ? qsTr("Cập nhật") : qsTr("Lưu vào yêu thích"))
            enabled: control.manageOnly || control.selectedIds.length > 0
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            contentItem: Label {
                text: acceptButton.text
                color: acceptButton.enabled ? "white" : "#5c5c64"
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
            }
            background: Rectangle {
                radius: 4
                color: acceptButton.enabled
                       ? (acceptButton.hovered ? "#4a6a9c" : "#3a5a8c")
                       : "#22222a"
            }
        }
    }

    onAccepted: {
        if (control.manageOnly || control.selectedIds.length === 0)
            return
        control.confirmed(control.selectedIds)
    }
}
