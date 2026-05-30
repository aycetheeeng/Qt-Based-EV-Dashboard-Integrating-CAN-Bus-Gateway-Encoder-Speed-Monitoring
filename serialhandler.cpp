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
            // --- MERKEZİ VİTES KONTROLÜ (TxData103[6] ve [7] birleşimi) ---
            int rxData6_vites = static_cast<quint8>(packet[8]);
            int rxData7_vites = static_cast<quint8>(packet[9]);
            int prnd_adc = (rxData6_vites << 8) | rxData7_vites; // 12-bit ADC verisi hazır

            // ADC değerine göre hangi vitesin aktif olacağını hesapLYORUZ
            bool target_p = (prnd_adc < 4096 && prnd_adc > 4050);
            bool target_r = (prnd_adc < 3250 && prnd_adc > 3180);
            bool target_n = (prnd_adc < 2520 && prnd_adc > 2430);
            bool target_d = (prnd_adc < 1650 && prnd_adc > 1570);

            // Eğer durum değiştiyse QML'i tetikle (Gereksiz arayüz kasmasını önler)
            if (m_p_signal != target_p) { m_p_signal = target_p; emit p_signalChanged(); }
            if (m_r_signal != target_r) { m_r_signal = target_r; emit r_signalChanged(); }
            if (m_n_signal != target_n) { m_n_signal = target_n; emit n_signalChanged(); }
            if (m_d_signal != target_d) { m_d_signal = target_d; emit d_signalChanged(); }


            // --- LAMBA / FAR KONTROLÜ (RxData103[2] -> rxData2) ---
            if (rxData2 == 1) {
                qDebug() << ">>> [DURUM] FARLAR AÇIK (1)";
                if (!m_lambaAcik) { m_lambaAcik = true; emit lambaAcikChanged(); }
            } else {
                qDebug() << ">>> [DURUM] FARLAR KAPALI (0)";
                if (m_lambaAcik) { m_lambaAcik = false; emit lambaAcikChanged(); }
            }

            // --- SÜRÜŞ MODU KONTROLÜ (RxData103[1] / rxData1) ---
            bool targetSport   = (rxData1 == 1);
            bool targetEco     = (rxData1 == 2);
            bool targetComfort = (rxData1 == 3);

            // Sport Mod Kontrolü
            if (m_aktifSport != targetSport) {
                m_aktifSport = targetSport;
                qDebug() << ">>> [DURUM] SPORT MOD:" << (m_aktifSport ? "AKTİF" : "PASİF");
                emit aktifSportChanged();
            }

            // Eco Mod Kontrolü
            if (m_aktifEco != targetEco) {
                m_aktifEco = targetEco;
                qDebug() << ">>> [DURUM] ECO MOD:" << (m_aktifEco ? "AKTİF" : "PASİF");
                emit aktifEcoChanged();
            }

            // Comfort Mod Kontrolü
            if (m_aktifComfort != targetComfort) {
                m_aktifComfort = targetComfort;
                qDebug() << ">>> [DURUM] COMFORT MOD:" << (m_aktifComfort ? "AKTİF" : "PASİF");
                emit aktifComfortChanged();
            }

            // --- SİNYALLERİN KONTROLLERİ ---

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

            // =========================================================
            // TAM BURAYA EKLENDİ: GÜVENLİ FLAŞÖR (TIMER) MANTIĞI
            // =========================================================
            // Çökmeyi önlemek adına timer havada yoksa tam şu an güvenle yaratılıyor
            if (!m_flasherTimer)
            {
                m_flasherTimer = new QTimer(this);
                m_flasherTimer->setInterval(350); // 350ms yanma - 350ms sönme ritmi
                connect(m_flasherTimer, &QTimer::timeout, this, [this]() {
                    m_globalBlink = !m_globalBlink;
                    emit globalBlinkChanged();
                });
            }

            if (m_leftSignal || m_rightSignal)
            {
                // Herhangi bir sinyal aktifse ve timer duruyorsa başlat
                if (!m_flasherTimer->isActive()) {
                    m_globalBlink = true;
                    emit globalBlinkChanged();
                    m_flasherTimer->start();
                }
            }
            else
            {
                // İki sinyal de kapatıldıysa timer'ı durdur ve durumu sıfırla
                if (m_flasherTimer->isActive()) {
                    m_flasherTimer->stop();
                    m_globalBlink = false;
                    emit globalBlinkChanged();
                }
            }
            // =========================================================

        } // <-- if (msgType == 3) bloğunun kapanışı
    } // <-- while (m_buffer.size() >= 11) döngüsünün kapanışı
}

/*
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
            // --- MERKEZİ VİTES KONTROLÜ (TxData103[6] ve [7] birleşimi) ---
            int rxData6 = static_cast<quint8>(packet[8]);
            int rxData7 = static_cast<quint8>(packet[9]);
            int prnd_adc = (rxData6 << 8) | rxData7; // 12-bit ADC verisi hazır

            // ADC değerine göre hangi vitesin aktif olacağını hesaplıyoruz
            bool target_p = (prnd_adc < 4096 && prnd_adc > 4050);
            bool target_r = (prnd_adc < 3250 && prnd_adc > 3180);
            bool target_n = (prnd_adc < 2520 && prnd_adc > 2430);
            bool target_d = (prnd_adc < 1650 && prnd_adc > 1570);

            // Eğer durum değiştiyse QML'i tetikle (Gereksiz arayüz kasmasını önler)
            if (m_p_signal != target_p) { m_p_signal = target_p; emit p_signalChanged(); }
            if (m_r_signal != target_r) { m_r_signal = target_r; emit r_signalChanged(); }
            if (m_n_signal != target_n) { m_n_signal = target_n; emit n_signalChanged(); }
            if (m_d_signal != target_d) { m_d_signal = target_d; emit d_signalChanged(); }


            // --- LAMBA / FAR KONTROLÜ (RxData103[2] -> rxData2) ---
            if (rxData2 == 1) {
                qDebug() << ">>> [DURUM] FARLAR AÇIK (1)";
                if (!m_lambaAcik) { m_lambaAcik = true; emit lambaAcikChanged(); }
            } else {
                qDebug() << ">>> [DURUM] FARLAR KAPALI (0)";
                if (m_lambaAcik) { m_lambaAcik = false; emit lambaAcikChanged(); }
            }

            // --- SÜRÜŞ MODU KONTROLÜ (RxData103[1] / rxData1) ---
            bool targetSport   = (rxData1 == 1);
            bool targetEco     = (rxData1 == 2);
            bool targetComfort = (rxData1 == 3);

            // Sport Mod Kontrolü
            if (m_aktifSport != targetSport) {
                m_aktifSport = targetSport;
                qDebug() << ">>> [DURUM] SPORT MOD:" << (m_aktifSport ? "AKTİF" : "PASİF");
                emit aktifSportChanged();
            }

            // Eco Mod Kontrolü
            if (m_aktifEco != targetEco) {
                m_aktifEco = targetEco;
                qDebug() << ">>> [DURUM] ECO MOD:" << (m_aktifEco ? "AKTİF" : "PASİF");
                emit aktifEcoChanged();
            }

            // Comfort Mod Kontrolü
            if (m_aktifComfort != targetComfort) {
                m_aktifComfort = targetComfort;
                qDebug() << ">>> [DURUM] COMFORT MOD:" << (m_aktifComfort ? "AKTİF" : "PASİF");
                emit aktifComfortChanged();
            }

            // SİNYALLERİN KONTROLLERİ BURADA

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
*/
