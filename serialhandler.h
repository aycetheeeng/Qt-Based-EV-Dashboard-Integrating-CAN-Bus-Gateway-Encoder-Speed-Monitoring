#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <QTimer>  // <-- Bu eksik olduğu için hata veriyor

class SerialHandler : public QObject
{
    Q_OBJECT

    // =========================================================
    // QML PROPERTY TANIMLARI
    // =========================================================

    // HIZ
    Q_PROPERTY(int speedValue READ speedValue NOTIFY speedValueChanged)

    // NORMAL VERİLER
    Q_PROPERTY(float kwValue READ kwValue NOTIFY dataChanged)
    Q_PROPERTY(int bataryaValue READ bataryaValue NOTIFY dataChanged)
    Q_PROPERTY(int menzilValue READ menzilValue NOTIFY dataChanged)
    Q_PROPERTY(float tuketimValue READ tuketimValue NOTIFY dataChanged)
    Q_PROPERTY(float tripValue READ tripValue NOTIFY dataChanged)

    // FORMATLANMIŞ STRINGLER
    Q_PROPERTY(QString kwDisplay READ kwDisplay NOTIFY dataChanged)
    Q_PROPERTY(QString kWhDisplay READ kWhDisplay NOTIFY dataChanged)
    Q_PROPERTY(QString menzilDisplay READ menzilDisplay NOTIFY dataChanged)
    Q_PROPERTY(QString tuketimDisplay READ tuketimDisplay NOTIFY dataChanged)

    // =========================================================
    // SİNYAL PROPERTYLERİ
    // =========================================================

    Q_PROPERTY(bool leftSignal READ leftSignal NOTIFY leftSignalChanged)
    Q_PROPERTY(bool rightSignal READ rightSignal NOTIFY rightSignalChanged)

    // SÜRÜŞ MODU PROPERTYLERİ
    Q_PROPERTY(bool aktifSport READ aktifSport NOTIFY aktifSportChanged)
    Q_PROPERTY(bool aktifEco READ aktifEco NOTIFY aktifEcoChanged)
    Q_PROPERTY(bool aktifComfort READ aktifComfort NOTIFY aktifComfortChanged)

    // LAMBA / FAR PROPERTYSİ
    Q_PROPERTY(bool lambaAcik READ lambaAcik NOTIFY lambaAcikChanged)

    // PRND PROPERYTSİ
    //Q_PROPERTY(int prnd READ prnd NOTIFY prndChanged)
    Q_PROPERTY(bool p_signal READ p_signal NOTIFY p_signalChanged)
    Q_PROPERTY(bool r_signal READ r_signal NOTIFY r_signalChanged)
    Q_PROPERTY(bool n_signal READ n_signal NOTIFY n_signalChanged)
    Q_PROPERTY(bool d_signal READ d_signal NOTIFY d_signalChanged)

    // Blink için
    Q_PROPERTY(bool globalBlink READ globalBlink NOTIFY globalBlinkChanged)

    Q_PROPERTY(QString currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(QString currentDate READ currentDate NOTIFY currentDateChanged)

public:
    explicit SerialHandler(QObject *parent = nullptr);

    // =========================================================
    // GETTER FONKSİYONLARI
    // =========================================================

    // NORMAL VERİLER
    int speedValue() const { return m_speed; }
    float kwValue() const { return m_kw; }
    int bataryaValue() const { return m_batarya; }
    int menzilValue() const { return m_menzil; }
    float tuketimValue() const { return m_tuketim; }
    float tripValue() const { return m_toplamKwh; }

    // FORMATLI STRINGLER
    QString kwDisplay() const
    {
        return QString::number(m_kw, 'f', 1) + " kW";
    }

    QString kWhDisplay() const
    {
        return QString::number(m_toplamKwh, 'f', 1) + " kWh";
    }

    QString menzilDisplay() const
    {
        return QString::number(m_menzil) + " km";
    }

    QString tuketimDisplay() const
    {
        return QString::number(m_tuketim, 'f', 1) + " kWh";
    }


    QString currentTime() const { return m_currentTime; }
    QString currentDate() const { return m_currentDate; }

    // SİNYAL GETTER
    bool leftSignal() const { return m_leftSignal; }
    bool rightSignal() const { return m_rightSignal; }

    // SÜRÜŞ MODU GETTERLARI (Eksik olan ve hataya sebep olan kısım burasıydı, eklendi!)
    bool aktifSport() const { return m_aktifSport; }
    bool aktifEco() const { return m_aktifEco; }
    bool aktifComfort() const { return m_aktifComfort; }

    // LAMBA / FAR GETTERI
    bool lambaAcik() const { return m_lambaAcik; }


    // PRND GETTERI
    //bool prnd() const { return m_prnd; }


    bool p_signal() const { return m_p_signal; }
    bool r_signal() const { return m_r_signal; }
    bool n_signal() const { return m_n_signal; }
    bool d_signal() const { return m_d_signal; }

    // Blink
    bool globalBlink() const { return m_globalBlink; }



private slots:
    void readData();

signals:

    // NORMAL VERİ SİNYALLERİ
    void speedValueChanged();
    void dataChanged();

    // SİNYAL EVENTLERİ
    void leftSignalChanged();
    void rightSignalChanged();

    // SÜRÜŞ MODU EVENTLERİ
    void aktifSportChanged();
    void aktifEcoChanged();
    void aktifComfortChanged();

    // LAMBA / FAR EVENTİ
    void lambaAcikChanged();

    // PRND EVENTİ
    void prndChanged();

    // VİTES SİNYAL EVENTLERİ
    void p_signalChanged();
    void r_signalChanged();
    void n_signalChanged();
    void d_signalChanged();

    // BLİNK
    void globalBlinkChanged();

    // ZAMAN
    void currentTimeChanged();
    void currentDateChanged();

private:

    // =========================================================
    // SERIAL
    // =========================================================

    QSerialPort *m_serial;
    QByteArray m_buffer;

    // =========================================================
    // NORMAL VERİLER
    // =========================================================

    int m_speed = 0;
    float m_kw = 0.0f;
    float m_tuketim = 0.0f;
    int m_batarya = 0;
    int m_menzil = 0;
    float m_toplamKwh = 0.0f;

    // =========================================================
    // SİNYAL DURUMLARI
    // =========================================================

    bool m_leftSignal = false;
    bool m_rightSignal = false;

    // =========================================================
    // SÜRÜŞ MODU DURUMLARI
    // =========================================================

    bool m_aktifSport = false;
    bool m_aktifEco = false;
    bool m_aktifComfort = false;

    // =========================================================
    // LAMBA / FAR DURUMU
    // =========================================================

    bool m_lambaAcik = false;

    // =========================================================
    // PRND DURUMU
    // =========================================================

    //bool m_prnd = false;

    bool m_p_signal = false;
    bool m_r_signal = false;
    bool m_n_signal = false;
    bool m_d_signal = false;

    QTimer *m_flasherTimer;
    bool m_globalBlink = false;


    QString m_currentTime;
    QString m_currentDate;
    void updateClock(); // Saati tetikleyecek iç fonksiyon

};


#endif // SERIALHANDLER_H
