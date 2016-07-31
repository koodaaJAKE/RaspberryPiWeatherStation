import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    id: mainWindow
    width: 1200
    height: 700

    property var win;

    Image {
        source: "raspi.jpg"
        anchors.fill: parent
    }

    TextField {
        placeholderText: qsTr("Enter username")
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 170
        }

        style: TextFieldStyle {
            textColor: "black"
            background: Rectangle {
                radius: 2
                implicitWidth: 250
                implicitHeight: 50
                border.color: "#333"
                border.width: 1
            }
        }
    }

    TextField {
        placeholderText: qsTr("Enter password")
        echoMode: TextInput.Password
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 240
        }

        style: TextFieldStyle {
            textColor: "black"
            background: Rectangle {
                radius: 2
                implicitWidth: 250
                implicitHeight: 50
                border.color: "#333"
                border.width: 1
            }
        }

    }

    Button {
        id: exitButton
        text: "Exit"
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 390
        }

        style: ButtonStyle {
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    border.width: control.activeFocus ? 2 : 1
                    border.color: "#888"
                    radius: 10
                    gradient: Gradient {
                        GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                        GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                    }
                }
            }

        MouseArea {
            anchors.fill: exitButton
            onClicked: {
                Qt.quit();
            }
        }
    }

    Button {
        id: loginButton
        text: "LogIn"
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 390
        }
        style: ButtonStyle {
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    border.width: control.activeFocus ? 2 : 1
                    border.color: "#888"
                    radius: 10
                    gradient: Gradient {
                        GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                        GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                    }
                }
            }

        MouseArea {
            anchors.fill: loginButton
            onClicked: {
                var component = Qt.createComponent("TCPsocketWindow.qml");
                win = component.createObject(mainWindow);
                win.show();
            }
        }
    }
}
