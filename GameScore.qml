import QtQuick 2.0
import QtQuick.Controls 2.15

Label {
   property int firstWins: 0
   property int secondWins: 0
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("First Player's Wins: %1 | Second Player's Wins: %2").arg(firstWins).arg(secondWins)
        font {
            pointSize: height
            bold: true
        }
    }
}
