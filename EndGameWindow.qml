import QtQuick 2.0
import QtQuick.Controls 2.15

Popup {
    id: _endGameWindow
    width: 300
    height: 120
    anchors.centerIn: parent

    Column {
       anchors.fill: parent
       spacing: 20

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Restart Game?")
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
                text: qsTr("Yes")

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.model.restartGame();
                        _endGameWindow.close();
                        _startGameWindow.open();
                    }
                }
            }

            Button {
                height: 48
                width: 128
                text: qsTr("No")

                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.quit()
                }
            }
        }
    }
}
