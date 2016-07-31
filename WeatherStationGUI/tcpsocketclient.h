#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QString>
#include <QtNetwork>
#include <QtDebug>

class TCPsocketClient: public QObject
{
Q_OBJECT
public:
    explicit TCPsocketClient(QObject *parent = 0);
    ~TCPsocketClient();
    Q_INVOKABLE bool createConnection(const QString &addr, const quint16 port);
    Q_INVOKABLE bool readData();
    Q_INVOKABLE bool readMaxAndMinValues();
    Q_INVOKABLE float getTemperature();
    Q_INVOKABLE float getHumidity();
    Q_INVOKABLE float getMinTemperature();
    Q_INVOKABLE float getMaxTemperature();
    Q_INVOKABLE float getMinHumidity();
    Q_INVOKABLE float getMaxHumidity();

private:
    QTcpSocket clientSocket;
    bool sendByte(const char *commandByte);
    quint32 parseData(quint8 *rawData, bool tempOrhum);
    quint32 parseMaxMinData(quint8 *rawData, qint32 chooseParse);
    void setTemperature(float temperature);
    void setHumidity(float humidity);
    void setMinTemperature(float minTemperature);
    void setMaxTemperature(float maxTemperature);
    void setMinHumidity(float minHumidity);
    void setMaxHumidity(float maxHumidity);

    float m_temperature;
    float m_humidity;
    float m_minTemperature;
    float m_maxTemperature;
    float m_minHumidity;
    float m_maxHumidity;
};

#endif // TCPSOCKETCLIENT_H
