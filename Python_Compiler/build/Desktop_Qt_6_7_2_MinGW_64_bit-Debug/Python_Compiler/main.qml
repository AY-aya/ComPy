import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    visible: true
    width: 1300
    height: 700
    title: "<ComPy"

    FileDialog {
        id: fileDialog
        title: "Select a File"
        nameFilters: ["Python files (*.py)", "All files (*)"]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            let localPath = selectedFile
            console.log("Local file path:", localPath)
            controller.loadFile(localPath)
        }

        onRejected: {
            console.log("File selection canceled")
        }
    }

    property bool lexer_run: true
    property bool parser_run: false
    property bool page1: true

    RowLayout {
        id: top_bar
        height: 80
        anchors {
            top: parent.top
            left: parent.left
            right: logo.left
        }

        Button {
            text: "Open Python File"
            onClicked: fileDialog.open()
            width: 100
            height: 50
            font.pointSize: 12
        }

        Button {
            text: "Run Lexer"
            onClicked: {
                controller.runLexer()
                root.lexer_run = false
            }
            width: 100
            height: 50
            font.pointSize: 12
        }

        Button {
            text: "Run Parser"
            enabled: !root.lexer_run
            onClicked: {
                controller.runParser()
                root.parser_run = true
                page1 = false
            }
            width: 100
            height: 50
            font.pointSize: 12
        }

        Button {
            text: "Reset"
            enabled: !root.lexer_run
            onClicked: {
                page1 = true
                controller.clearCode()
                //controller.runLexer()
            }
            width: 100
            height: 50
            font.pointSize: 12
        }

        Button {
            id: page2
            text: "⬅️"
            enabled: !root.lexer_run && !page1
            onClicked: {
                page1 = true
            }
            width: 100
            height: 50
            font.pointSize: 12
        }

        Button {
            text: "➡️"
            enabled: page1
            onClicked: {
                page1 = false
            }
            width: 100
            height: 50
            font.pointSize: 12
        }
    }
    Image {
        id: logo
        source: "assets/logo.png"
        fillMode: Image.PreserveAspectFit
        smooth: true
        width: 200
        height: 200
        anchors {
            verticalCenter: top_bar.verticalCenter
            right: parent.right
            left: top_bar.right
        }
    }

    Rectangle {
        id: lexer_view
        visible: root.page1
        anchors {
            top: top_bar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        RowLayout {
            anchors.fill: parent
            ColumnLayout {
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                spacing: 10

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 2 - 20

                    TextArea {
                        id: codeDisplay
                        text: controller.code
                        readOnly: true
                        wrapMode: Text.WrapAnywhere

                        font.family: "Courier New"
                        font.pointSize: 12
                        color: "white"

                        background: Rectangle {
                            color: "black"
                            radius: 4
                        }
                    }
                }

                GroupBox {
                    title: "Tokens"
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 2 - 20

                    ListView {
                        anchors.fill: parent
                        model: controller.tokens

                        delegate: Text {
                            text: modelData
                            wrapMode: Text.Wrap

                            font.family: "Courier New"
                            font.pointSize: 11
                            color: "black"
                        }
                        clip: true
                    }
                }
            }

            ColumnLayout {
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                spacing: 10

                GroupBox {
                    title: "Errors"
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 2 - 20

                    ListView {
                        anchors.fill: parent
                        model: controller.errors

                        delegate: Item {
                            width: ListView.view.width // ensures full width for wrapping
                            height: 60
                            Text {
                                text: modelData
                                wrapMode: Text.Wrap
                                width: parent.width // match width for proper wrapping

                                font.family: "Courier New"
                                font.pointSize: 11
                                color: "black"
                            }
                        }

                        clip: true
                    }
                }
                GroupBox {
                    title: "Symbol Table"
                    Layout.preferredWidth: parent.width
                    Layout.preferredHeight: parent.height / 2 - 10

                    ListModel {
                        id: symbolModel
                    }

                    Connections {
                        target: controller
                        function onSymbolTableChanged() {
                            symbolModel.clear()
                            for (var i = 0; i < controller.symbolTable.length; ++i) {
                                let entry = controller.symbolTable[i]
                                let parts = entry.split(",")
                                symbolModel.append({
                                                       "ID": parts[0].trim(),
                                                       "name": parts[1].trim(),
                                                       "data_type": parts[2].trim(
                                                           ),
                                                       "value": parts[3].trim()
                                                   })
                            }
                        }
                    }

                    function updateSymbolModel() {
                        symbolModel.clear()
                        for (var i = 0; i < controller.symbolTable.length; ++i) {
                            let entry = controller.symbolTable[i]
                            let parts = entry.split(",")
                            symbolModel.append({
                                                   "ID": parts[0].trim(),
                                                   "name": parts[1].trim(),
                                                   "data_type": parts[2].trim(),
                                                   "value": parts[3].trim()
                                               })
                        }
                    }

                    ColumnLayout {
                        anchors.fill: parent

                        spacing: 2

                        // === Header Row ===
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 0

                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "ID"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Name"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Data Type"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Value"
                                    font.bold: true
                                }
                            }
                        }

                        // === Table Body ===
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Column {
                                width: parent.width

                                Repeater {
                                    model: symbolModel

                                    Row {
                                        spacing: 0

                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.ID
                                            }
                                        }

                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.name
                                            }
                                        }
                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.data_type
                                            }
                                        }
                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.value
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: parser_view
        visible: root.parser_run && !root.page1
        anchors {
            top: top_bar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        RowLayout {
            anchors.fill: parent
            ColumnLayout {
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                spacing: 10

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    TextArea {
                        id: parsetree
                        text: controller.parseTreeJson
                        readOnly: true
                        wrapMode: Text.WrapAnywhere

                        font.family: "Courier New"
                        font.pointSize: 12
                        color: "#000435"
                    }
                }
            }

            ColumnLayout {
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                spacing: 10

                GroupBox {
                    title: "Errors"
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 2 - 20

                    ListView {
                        anchors.fill: parent
                        model: controller.parserErrors

                        delegate: Item {
                            width: ListView.view.width // ensures full width for wrapping
                            height: 60
                            Text {
                                text: modelData
                                wrapMode: Text.Wrap
                                width: parent.width // match width for proper wrapping

                                font.family: "Courier New"
                                font.pointSize: 11
                                color: "black"
                            }
                        }

                        clip: true
                    }
                }
                GroupBox {
                    title: "Symbol Table"
                    Layout.preferredWidth: parent.width
                    Layout.preferredHeight: parent.height / 2 - 10

                    ListModel {
                        id: parsersymbolModel
                    }

                    Connections {
                        target: controller
                        function onParserSymbolTableChanged() {
                            parsersymbolModel.clear()
                            for (var i = 0; i < controller.parserSymbolTable.length; ++i) {
                                let entry = controller.parserSymbolTable[i]
                                let parts = entry.split(",")
                                parsersymbolModel.append({
                                                             "ID": parts[0].trim(
                                                                 ),
                                                             "name": parts[1].trim(
                                                                 ),
                                                             "data_type": parts[2].trim(
                                                                 ),
                                                             "value": parts[3].trim(
                                                                 ),
                                                             "role": parts[4].trim(
                                                                 ),
                                                             "scope": parts[5].trim()
                                                         })
                            }
                        }
                    }

                    function updateSymbolModel() {
                        parsersymbolModel.clear()
                        for (var i = 0; i < controller.parserSymbolTable.length; ++i) {
                            let entry = controller.parserSymbolTable[i]
                            let parts = entry.split(",")
                            parsersymbolModel.append({
                                                         "ID": parts[0].trim(),
                                                         "name": parts[1].trim(
                                                             ),
                                                         "data_type": parts[2].trim(
                                                             ),
                                                         "value": parts[3].trim(
                                                             ),
                                                         "role": parts[4].trim(
                                                             ),
                                                         "scope": parts[5].trim(
                                                             )
                                                     })
                        }
                    }

                    ColumnLayout {
                        anchors.fill: parent

                        spacing: 2

                        // === Header Row ===
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 0

                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "ID"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Name"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Data Type"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Value"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 100
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Role"
                                    font.bold: true
                                }
                            }
                            Rectangle {
                                color: "#dddddd"
                                border.color: "#888"
                                width: 150
                                height: 30
                                Text {
                                    anchors.centerIn: parent
                                    text: "Scope"
                                    font.bold: true
                                }
                            }
                        }

                        // === Table Body ===
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Column {
                                width: parent.width

                                Repeater {
                                    model: parsersymbolModel

                                    Row {
                                        spacing: 0

                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.ID
                                            }
                                        }

                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.name
                                            }
                                        }
                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.data_type
                                            }
                                        }
                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.value
                                            }
                                        }
                                        Rectangle {
                                            width: 100
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.role
                                            }
                                        }
                                        Rectangle {
                                            width: 150
                                            height: 30
                                            border.color: "#aaa"
                                            color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.scope
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
