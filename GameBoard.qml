import QtQuick 2.15
import Game 1.0;
import Qt.labs.qmlmodels 1.0
import QtQuick.Controls 2.15

TableView {
    id: root

    model: GameBoardModel {
        onGameEndWindow: _endGameWindow.open();
    }

    implicitHeight: height / root.model.demension
    implicitWidth: width / root.model.demension

    delegate: Item {
        id: _backgroundDelegate
        property int row: Math.floor(index/root.model.demension);
        property int col: index % root.model.demension;
        implicitWidth: root.implicitWidth
        implicitHeight: root.implicitHeight

        Rectangle {
            anchors.fill: _backgroundDelegate
            border.width: 7
            border.color: boardColor
            color: {
                var even = ((row + col) % 2) == 0;
                return even ? "#ffaa88" : "#ff8b47";
            }

            MouseArea {
                anchors.fill: parent
                enabled: mouseEnable
                onClicked: {
                    root.model.indexMoveTo = index;
                    root.model.move();
                }
            }
        }

        Checker {
            id: _Checker
            width: _backgroundDelegate.width - 20
            height: width
            anchors.centerIn: _backgroundDelegate
            radius: root.implicitHeight / 2
            visible: visibl
            color: checkerColor

            Rectangle {
                anchors.centerIn: _Checker
                width: _Checker.width / 4
                height: _Checker.height / 4
                radius: width / 2
                color: "violet"
                visible: king
            }

            MouseArea {
                anchors.fill: _Checker;
                onClicked: {
                    root.model.indexMoveFrom = index;
                    root.model.checkerSelection();
                }
            }
        }
    }

    StartGameWindow {
        id: _startGameWindow
    }

    EndGameWindow {
        id: _endGameWindow
    }

    Component.onCompleted: {
        _startGameWindow.open();
    }
}
