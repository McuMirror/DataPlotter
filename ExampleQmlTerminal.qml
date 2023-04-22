// These are maximum versions on Windows XP (Qt 5.7.1)
import QtQuick 2.7
import QtQml 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.2

Rectangle {
    // Do not set size or anchors of root object

    property bool darkThemeIsUsed: dataPlotter.darkThemeIsUsed
    Material.theme: darkThemeIsUsed ? Material.Dark : Material.Light
    Material.accent: Material.Blue
    Material.elevation: 1
    color: dataPlotter.tabBackground

    function send(data) {
        // Use this function to send data to serial port
        dataPlotter.transmitToSerial(data);
    }

    function sendToParser(data) {
        // Use this function to send data into manual input parser
        dataPlotter.sendToParser(data);
    }

    Connections {
        target: dataPlotter
        function onReceivedFromSerial(data) {
            // This function handles "$$D.......\0" data received
            rxText.text = data
        }
    }

    // Use $$V(name):(value) to set values of properties
    property int a: 0
    property int b: 0

    property int w: 0
    property int h: 0

    Flickable {
        anchors.fill: parent

        contentHeight: mainColumn.implicitHeight
        ScrollBar.vertical: ScrollBar{}
        Column {
            id: mainColumn
            anchors.fill: parent
            spacing: 10
            anchors.margins: 5

            Rectangle{
                anchors.horizontalCenter: parent.horizontalCenter

                height: title.implicitHeight*2
                width: title.implicitWidth+20

                color: darkThemeIsUsed?"green":"yellow"

                Label {
                    id: title
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: "Welcome to new terminal!"
                }
            }

            Label {
                text: 'This GUI can be created using QML file that can be sent from connected device using $$Q message type. Click <html><a href="null">here</a></html> do open development options. You can save this as template for creating your terminal. Use the load and refresh button to test your file. Data in $$Q message are compressed and bytestuffed - use the export function to do that.'
                onLinkActivated: sendToParser("$$Sqmldev:1;")
                width: parent.width
                font.pointSize: 9
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
            }

            Label {
                text: 'You can create controlls that will send text commands to the device.'
                width: parent.width
                font.pointSize: 9
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Button{
                    text: "Send:"
                    onClicked: send(txText.text)
                }
                TextField {
                    id: txText
                    Layout.fillWidth: true
                    Layout.preferredWidth: 10000
                    text: "Hello World"
                }
            }

            Label {
                text: 'And you can use $$V(property):(value) to set variables inside the QML file. Numeric values are sent as text and converted automatically.'
                width: parent.width
                font.pointSize: 9
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
            }

            Label{
                // Display values of properties like this:
                text: "Variables: a=" + a + "  b=" + b +"  a×b=" + (a*b)
            }

            Label {
                text: 'You can also create your own parser for messages send as $$D(message)\\0'
                width: parent.width
                font.pointSize: 9
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Label{
                    text: "This was received:"
                }
                TextField {
                    id: rxText
                    Layout.fillWidth: true
                    Layout.preferredWidth: 10000
                    text: ""
                    readOnly: true
                }
            }

            Button {
                text: "Need more space?"
                onClicked: popupExample.open()
            }
        }

        Popup{
            id: popupExample
            anchors.centerIn: parent

            onOpened: {
                slider_r.value = randomInteger(0,255);
                slider_g.value = randomInteger(0,255);
                slider_b.value = randomInteger(0,255);
            }

            ColumnLayout {
                Label {
                    text: "You can use pop-up"
                    horizontalAlignment: "AlignHCenter"
                    Layout.fillWidth: true
                }
                Slider {
                    id:slider_r
                    from: 0
                    to: 255
                    Material.accent: Material.Red
                    Layout.fillWidth: true
                }

                Slider {
                    id:slider_g
                    from: 0
                    to: 255
                    Material.accent: Material.Green
                    Layout.fillWidth: true
                }

                Slider {
                    id:slider_b
                    from: 0
                    to: 255
                    Material.accent: Material.Blue
                    Layout.fillWidth: true
                }

                Rectangle {
                    color: Qt.rgba(slider_r.value/255., slider_g.value/255., slider_b.value/255., 1)
                    height: 50
                    Layout.fillWidth: true
                }

                Button {
                    text: "OK"
                    onClicked: popupExample.close()
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
    }

    // Create your own functions (it is JavaScript)
    function randomInteger(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min;
    }
}
