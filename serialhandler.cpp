#include "serialhandler.h"

SerialHandler::SerialHandler(QObject *parent) : QObject(parent), m_speed(0) {
    m_serial = new QSerialPort(this);

    // AYARLAR: STM32 CubeMX ile birebir aynı olmalı!
    m_serial->setPortName("COM8"); // Seninkine göre düzelt (Örn: COM5)
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);

    if (m_serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Seri Port başarıyla açıldı!";
    } else {
        qDebug() << "Seri Port AÇILAMADI! Hatayı kontrol et.";
    }

    // Veri geldiğinde readData fonksiyonunu çalıştır
    connect(m_serial, &QSerialPort::readyRead, this, &SerialHandler::readData);
}

void SerialHandler::readData() {
    // Porttaki tüm veriyi oku
    QByteArray data = m_serial->readAll();

    if (!data.isEmpty()) {
        // En son gelen byte'ı hız olarak kabul et (1 byte'lık veri gönderiyorsan)
        m_speed = static_cast<unsigned char>(data.at(data.size() - 1));

        // TERMINALE YAZDIRMA: "Seri Port başarıyla açıldı!" yazısının altına düşer
        // Her veri geldiğinde terminalde canlı olarak göreceksin
        qDebug() << "G431'den Gelen Canlı Hız Verisi [tam_sayi_kalibre_hiz]:" << m_speed;

        // QML'deki göstergeyi güncellemesi için sinyal fırlat
        emit speedValueChanged();
    }
}
