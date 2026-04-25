#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

class SerialHandler : public QObject {
    Q_OBJECT
    // QML tarafında bu ismi (speedValue) kullanacağız
    Q_PROPERTY(int speedValue READ speedValue NOTIFY speedValueChanged)

public:
    explicit SerialHandler(QObject *parent = nullptr);
    int speedValue() const { return m_speed; }

private slots:
    void readData(); // Veri geldiğinde tetiklenecek

signals:
    void speedValueChanged();

private:
    QSerialPort *m_serial;
    int m_speed;
};

#endif
