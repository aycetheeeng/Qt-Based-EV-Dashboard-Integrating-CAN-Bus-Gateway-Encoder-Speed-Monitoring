#include "serialhandler.h"
#include <QDebug>

SerialHandler::SerialHandler(QObject *parent)
    : QObject(parent),
    m_speed(0),
    m_leftSignal(false),
    m_rightSignal(false)
{
    m_serial = new QSerialPort(this);

    // STM32 ile aynı ayarlar
    m_serial->setPortName("COM8"); // Kendi portuna göre değiştir
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);

    if (m_serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Seri Port başarıyla açıldı!";
    } else {
        qDebug() << "Seri Port AÇILAMADI!";
    }

    connect(m_serial, &QSerialPort::readyRead,
            this, &SerialHandler::readData);
}


void SerialHandler::readData()
{
    m_buffer.append(m_serial->readAll());

    while (m_buffer.size() >= 11)
    {
        // 1. FRAME BAŞLANGIÇ KONTROLÜ
        if (static_cast<quint8>(m_buffer[0]) != 0xAA)
        {
            m_buffer.remove(0, 1);
            continue;
        }

        // 2. FRAME BİTİŞ KONTROLÜ
        if (static_cast<quint8>(m_buffer[10]) != 0x55)
        {
            m_buffer.remove(0, 1);
            continue;
        }

        // Tam paketi al ve buffer'dan temizle
        QByteArray packet = m_buffer.left(11);
        m_buffer.remove(0, 11);

        // 3. MESAJ TİPİNİ BELİRLE (1 = 101, 2 = 102, 3 = 103)
        uint8_t msgType = static_cast<uint8_t>(packet[1]);

        // 4. VERİLERİ DOĞRU İNDEKSLE EŞLEŞTİR (Kayma tamamen düzeltildi)
        int rxData0 = static_cast<int>(packet[2]);
        int rxData1 = static_cast<int>(packet[3]);
        int rxData2 = static_cast<int>(packet[4]);
        int rxData3 = static_cast<int>(packet[5]); // Sol Sinyal
        int rxData4 = static_cast<int>(packet[6]); // Sağ Sinyal
        int rxData5 = static_cast<int>(packet[7]);
        int rxData6 = static_cast<int>(packet[8]);
        int rxData7 = static_cast<int>(packet[9]);

        // --- TERMİNALDE KESİNTİSİZ GÖSTERİM BÖLÜMÜ ---
        qDebug() << "------------------------------------------";
        qDebug() << "GELEN PAKET KAYNAĞI: RxData" << (100 + msgType);
        qDebug() << "Data[0]:" << rxData0 << "Data[1]:" << rxData1 << "Data[2]:" << rxData2;
        qDebug() << "Data[3]:" << rxData3 << "Data[4]:" << rxData4 << "Data[5]:" << rxData5;
        qDebug() << "Data[6]:" << rxData6 << "Data[7]:" << rxData7;

        // 5. SADECE RxData103 GELİRSE SİNYAL MANTIĞINI ÇALIŞTIR
        if (msgType == 3)
        {
            // RxData103[3] - SOL SİNYAL KONTROLÜ
            if (rxData3 == 1) {
                qDebug() << ">>> [DURUM] SOL SINYAL AKTİF (1)";
                if (!m_leftSignal) { m_leftSignal = true; emit leftSignalChanged(); }
            } else {
                qDebug() << ">>> [DURUM] SOL SINYAL KAPALI (0)";
                if (m_leftSignal) { m_leftSignal = false; emit leftSignalChanged(); }
            }

            // RxData103[4] - SAĞ SİNYAL KONTROLÜ
            if (rxData4 == 1) {
                qDebug() << ">>> [DURUM] SAĞ SİNYAL AKTİF (1)";
                if (!m_rightSignal) { m_rightSignal = true; emit rightSignalChanged(); }
            } else {
                qDebug() << ">>> [DURUM] SAĞ SİNYAL KAPALI (0)";
                if (m_rightSignal) { m_rightSignal = false; emit rightSignalChanged(); }
            }
        }
    }
}
