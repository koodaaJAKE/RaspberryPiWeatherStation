import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import TCPsocketClient 1.0

Window {

    width: 1200
    height: 700

    Image {
        source: "raspberrypi.png"
        anchors.centerIn: parent
    }

    //Global flag to avoid multiple connections
    property bool socketConnected: false

    property string ipAddress: "10.42.0.2"
    property int portNumber: 51000

    TCPsocketClient {
        id: clientSocket
    }

    Rectangle {

        Button {
            id: connectButton
            text: "Connect"
            anchors {
                top: parent.top
                topMargin: 100
                left: parent.right
                leftMargin: 20
            }

            style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 300
                        implicitHeight: 100
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
                anchors.fill: connectButton
                onClicked: {
                    if(!socketConnected) {
                        //clientSocket.createConnection("10.42.0.2", 51000);
                        clientSocket.createConnection(ipAddress, portNumber);
                    }

                    //Set the flag to allow one connection
                    socketConnected = true;
                }
            }
        }

        Button {
            id: disconnectButton
            text: "Disconnect"
            anchors {
                top: parent.top
                topMargin: 225
                left: parent.right
                leftMargin: 20
            }

            style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 300
                        implicitHeight: 100
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
                anchors.fill: disconnectButton
                onClicked: {
                    Qt.quit();
                }
            }
        }

        Button {
            id: dataButton
            text: "Read temperature and humidity"
            anchors {
                top: parent.top
                topMargin: 350
                left: parent.right
                leftMargin: 20
            }

            style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 300
                        implicitHeight: 100
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
                anchors.fill: dataButton
                onClicked: {
                    if(clientSocket.readData())
                        console.log("Data read ok!");
                    else
                        console.log("Reading data failed!");
                }
            }
        }

        Button {
            id: maxminDataButton
            text: "Read MAX and MIN values"
            anchors {
                top: parent.top
                topMargin: 475
                left: parent.right
                leftMargin: 20
            }

            style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 300
                        implicitHeight: 100
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
                anchors.fill: maxminDataButton
                onClicked: {
                    if(clientSocket.readMaxAndMinValues())
                        console.log("Max and min data read ok!");
                    else
                        console.log("Reading max and min data failed!")
                }
            }
        }

        Text {
            anchors {
                top: parent.top
                topMargin: 43
                left: parent.left
                leftMargin: 960
            }

            text: "Current temperature"
            font.family: "Helvetica"
            font.pointSize: 16
            color: "black"
        }

        TextField {
            id: temperatureField
            anchors {
                top: parent.top
                topMargin: 70
                left: parent.left
                leftMargin: 930
            }

            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 8
                    implicitWidth: 250
                    implicitHeight: 60
                    border.color: "#333"
                    border.width: 1
                }
            }
        }

        Text {
            anchors {
                top: parent.top
                topMargin: 148
                left: parent.left
                leftMargin: 975
            }

            text: "Current humidity"
            font.family: "Helvetica"
            font.pointSize: 16
            color: "black"
        }

        TextField {
            id: humidityField
            anchors {
                top: parent.top
                topMargin: 175
                left: parent.left
                leftMargin: 930
            }

            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 8
                    implicitWidth: 250
                    implicitHeight: 60
                    border.color: "#333"
                    border.width: 1
                }
            }
        }

        Text {
            anchors {
                top: parent.top
                topMargin: 253
                left: parent.left
                leftMargin: 980
            }

            text: "Min temperature"
            font.family: "Helvetica"
            font.pointSize: 16
            color: "black"
        }

        TextField {
            id: minTemperatureField
            anchors {
                top: parent.top
                topMargin: 280
                left: parent.left
                leftMargin: 930
            }

            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 8
                    implicitWidth: 250
                    implicitHeight: 60
                    border.color: "#333"
                    border.width: 1
                }
            }
        }

        Text {
            anchors {
                top: parent.top
                topMargin: 358
                left: parent.left
                leftMargin: 980
            }

            text: "Max temperature"
            font.family: "Helvetica"
            font.pointSize: 16
            color: "black"
        }

        TextField {
            id: maxTemperatureField
            anchors {
                top: parent.top
                topMargin: 385
                left: parent.left
                leftMargin: 930
            }

            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 8
                    implicitWidth: 250
                    implicitHeight: 60
                    border.color: "#333"
                    border.width: 1
                }
            }
        }

        Text {
            anchors {
                top: parent.top
                topMargin: 463
                left: parent.left
                leftMargin: 1000
            }

            text: "Min humidity"
            font.family: "Helvetica"
            font.pointSize: 16
            color: "black"
        }


        TextField {
            id: minHumidityField
            anchors {
                top: parent.top
                topMargin: 490
                left: parent.left
                leftMargin: 930
            }

            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 8
                    implicitWidth: 250
                    implicitHeight: 60
                    border.color: "#333"
                    border.width: 1
                }
            }
        }

        Text {
            anchors {
                top: parent.top
                topMargin: 568
                left: parent.left
                leftMargin: 1000
            }

            text: "Max humidity"
            font.family: "Helvetica"
            font.pointSize: 16
            color: "black"
        }

        TextField {
            id: maxHumidityField
            anchors {
                top: parent.top
                topMargin: 595
                left: parent.left
                leftMargin: 930
            }

            style: TextFieldStyle {
                textColor: "black"
                background: Rectangle {
                    radius: 8
                    implicitWidth: 250
                    implicitHeight: 60
                    border.color: "#333"
                    border.width: 1
                }
            }
        }


    }
}
