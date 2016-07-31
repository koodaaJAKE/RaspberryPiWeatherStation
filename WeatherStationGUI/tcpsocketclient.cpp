#include "tcpsocketclient.h"

TCPsocketClient::TCPsocketClient(QObject *parent) : QObject(parent)
{

}

TCPsocketClient::~TCPsocketClient()
{
    clientSocket.disconnectFromHost();
    if(clientSocket.state() == QAbstractSocket::UnconnectedState || clientSocket.waitForDisconnected(2000))
        qDebug() << "Disconnected from the host!";

    clientSocket.close();
    qDebug() << "Socket closed!";
}

bool TCPsocketClient::createConnection(const QString &addr, const quint16 port)
{
    QHostAddress hostAddr(addr);

    clientSocket.connectToHost(hostAddr, port);
    if(clientSocket.waitForConnected(5000)) {
        qDebug("Connected!");
        return true;
    }
    else
        return false;
}

bool TCPsocketClient::sendByte(const char *commandByte)
{
    if(clientSocket.write(commandByte, 1) == 1)
        return true;
    else
        return false;
}

bool TCPsocketClient::readData()
{
    const char commandByte[1] = { 'S' };
    quint8 dataArray[8];

    if(sendByte(commandByte) == true) {
        clientSocket.read((char*)dataArray, 8);

        quint32 tempRawData, humRawData;
        float temperature, humidity;

        /* Parse the data to unsigned int and then to ieee 754 float */
        tempRawData = parseData(dataArray, true);
        char *pul_t = (char *)&tempRawData;
        char *pf_t = (char *)&temperature;
        memcpy(pf_t, pul_t, sizeof(float));
        qDebug("Temperature: %0.2f", temperature);

        humRawData = parseData(dataArray, false);
        char *pul_h = (char *)&humRawData;
        char *pf_h = (char *)&humidity;
        memcpy(pf_h, pul_h, sizeof(float));
        qDebug("Humidity: %0.2f", humidity);

        return true;
    }
    else {
        qDebug("Failed to send data!");
        return false;
    }
}

bool TCPsocketClient::readMaxAndMinValues()
{
    const char commandByte[1] = { 'T' };
    quint8 dataArray[16];

    if(sendByte(commandByte) == true) {
        clientSocket.read((char*)dataArray, 16);

        quint32 minTempRawData, maxTempRawData, minHumRawData, maxHumRawData;
        float minTemperature, maxTemperature, minHumidity, maxHumidity;

        /* Parse the data to unsigned int and then to ieee 754 float */
        minTempRawData = parseData(dataArray, 0);
        char *pul_tMin = (char *)&minTempRawData;
        char *pf_tMin = (char *)&minTemperature;
        memcpy(pf_tMin, pul_tMin, sizeof(float));
        qDebug("Min temperature: %0.2f", minTemperature);

        maxTempRawData = parseData(dataArray, 1);
        char *pul_tMax = (char *)&maxTempRawData;
        char *pf_tMax = (char *)&maxTemperature;
        memcpy(pf_tMax, pul_tMax, sizeof(float));
        qDebug("Max temperature: %0.2f", maxTemperature);

        minHumRawData = parseData(dataArray, 2);
        char *pul_hMin = (char *)&minHumRawData;
        char *pf_hMin = (char *)&minHumidity;
        memcpy(pf_hMin, pul_hMin, sizeof(float));
        qDebug("Min humidity: %0.2f", minHumidity);

        maxHumRawData = parseData(dataArray, 3);
        char *pul_hMax = (char *)&maxHumRawData;
        char *pf_hMax = (char *)&maxHumidity;
        memcpy(pf_hMax, pul_hMax, sizeof(float));
        qDebug("Max humidity: %0.2f", maxHumidity);

        return true;
    }
    else {
        qDebug("Failed to send data!");
        return false;
    }
}

quint32 TCPsocketClient::parseData(quint8 *rawData, bool tempOrHum)
{
    quint32 data;
    if(tempOrHum)
        data = rawData[0] << 24 | rawData[1] << 16 | rawData[2] << 8 | rawData[3];
    else
        data = rawData[4] << 24 | rawData[5] << 16 | rawData[6] << 8 | rawData[7];
    return data;
}

quint32 TCPsocketClient::parseMaxMinData(quint8 *rawData, qint32 chooseParse)
{
    quint32 data;
    switch(chooseParse)
    {
        case 0:
            data = rawData[0] << 24 | rawData[1] << 16 | rawData[2] << 8 | rawData[3];
            break;

        case 1:
            data = rawData[4] << 24 | rawData[5] << 16 | rawData[6] << 8 | rawData[7];
            break;

        case 2:
            data = rawData[8] << 24 | rawData[9] << 16 | rawData[10] << 8 | rawData[11];
            break;

        case 3:
            data = rawData[12] << 24 | rawData[13] << 16 | rawData[14] << 8 | rawData[15];
            break;

        default:
            qDebug("Wrong value!");
            return -1;
    }
    return data;
}

void TCPsocketClient::setTemperature(float temperature)
{
    m_temperature = temperature;
}

float TCPsocketClient::getTemperature()
{
    return m_temperature;
}

void TCPsocketClient::setHumidity(float humidity)
{
    m_humidity = humidity;
}

float TCPsocketClient::getHumidity()
{
    return m_humidity;
}

void TCPsocketClient::setMinTemperature(float minTemperature)
{
    m_minTemperature = minTemperature;
}

float TCPsocketClient::getMinTemperature()
{
    return m_minTemperature;
}

void TCPsocketClient::setMaxTemperature(float maxTemperature)
{
    m_maxTemperature = maxTemperature;
}

float TCPsocketClient::getMaxTemperature()
{
    return m_maxTemperature;
}

void TCPsocketClient::setMinHumidity(float minHumidity)
{
    m_minHumidity = minHumidity;
}

float TCPsocketClient::getMinHumidity()
{
    return m_minHumidity;
}

void TCPsocketClient::setMaxHumidity(float maxHumidity)
{
    m_maxHumidity = maxHumidity;
}

float TCPsocketClient::getMaxHumidity()
{
    return m_maxHumidity;
}
