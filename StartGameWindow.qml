import QtQuick 2.0
import QtQuick.Controls 2.15

Popup {
    id: _startGameWindow
    width: 300
    height: 120
    anchors.centerIn: parent

    Column {
       anchors.fill: parent
       spacing: 20

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Which color?")
            font {
                pointSize: height
                bold: true
            }
        }

        Row {
            spacing: 30

            Button {
                height: 48
                width: 128
                text: qsTr("White")

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.model.firstColorChange();
                        _startGameWindow.close();
                    }
                }
            }

            Button {
                height: 48
                width: 128
                text: qsTr("Black")

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.model.secondColorChange();
                        _startGameWindow.close();
                    }
                }
            }
        }
    }
}
