// ÖNEMLİ: bu kod henüz daha proje kurulmadan önceki versiyon2 kodu, önceki ilk kod yalnızca sag_sinyal ve sol_siynali okuyan koddu (v1), 
// bu kod hız tüketim batarya ve menzil verilerini gösteren kod (v2), bundan sonraki ise proje stm32f4 ten verilerin farklı idler ile (0x101, 0x102 ve 0x103)
// olarak gönderildiği v3 kodu VERSİYON 3. tüm veriler geliyor şu an ama sinyaller üzerinde çalışıyorum. (29.05.2026)

// bundan sonraki kod v4 olacak yani hepsini okuduğumuzdan emin olucaz.

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
    m_buffer.append(m_serial->readAll());

    while (m_buffer.size() >= 11) {
        // Zarf Kontrolü: 0. byte 0xAA ve 10. byte 0x55 mi?
        if (static_cast<uint8_t>(m_buffer[0]) == 0xAA && static_cast<uint8_t>(m_buffer[10]) == 0x55) {

            QByteArray packet = m_buffer.left(11);
            m_buffer.remove(0, 11);

            uint8_t id = static_cast<uint8_t>(packet[1]); // ID artık 1. indiste

            if (id == 1) { // MESAJ 1 (Hız, kW, Tüketim)
                // HIZ: 2. indiste
                m_speed = static_cast<uint8_t>(packet[2]);

                // kW: 3. ve 4. indisler (Düşük ve Yüksek Byte birleşimi)
                int16_t guc_raw = (static_cast<uint8_t>(packet[4]) << 8) | static_cast<uint8_t>(packet[3]);
                m_kw = guc_raw / 10.0f;

                // --- TÜKETİM: 5. ve 6. indisler ---
                // Eskiden bu 3. ve 4. indisti, zarf ekleyince 2 byte kaydı! 0.0 görme sebebin buydu.
                uint16_t tuk_raw = (static_cast<uint8_t>(packet[6]) << 8) | static_cast<uint8_t>(packet[5]);
                m_tuketim = tuk_raw / 10.0f;

                emit speedValueChanged();
            }
            else if (id == 2) { // MESAJ 2 (Batarya, Menzil, Trip)
                // BATARYA: 2. indiste
                m_batarya = static_cast<uint8_t>(packet[2]);

                // MENZİL: 3. ve 4. indisler
                uint16_t men_raw = (static_cast<uint8_t>(packet[4]) << 8) | static_cast<uint8_t>(packet[3]);
                m_menzil = static_cast<int>(men_raw);

                // TRIP (kWh): 5. ve 6. indisler
                uint16_t trip_raw = (static_cast<uint8_t>(packet[6]) << 8) | static_cast<uint8_t>(packet[5]);
                m_toplamKwh = trip_raw / 100.0f;
            }

            emit dataChanged(); // QML'e "her şey güncellendi" haberi gönder
        }
        else {
            // Zarf bozuksa 1 byte atla ve aramaya devam et
            m_buffer.remove(0, 1);
        }
    }
}
