#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

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
    Q_PROPERTY(QString menzilDisplay READ menzilDisplay NOTIFY dataChanged)
    Q_PROPERTY(QString tuketimDisplay READ tuketimDisplay NOTIFY dataChanged)

    // =========================================================
    // SİNYAL PROPERTYLERİ
    // =========================================================

    Q_PROPERTY(bool leftSignal READ leftSignal NOTIFY leftSignalChanged)
    Q_PROPERTY(bool rightSignal READ rightSignal NOTIFY rightSignalChanged)

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

    QString menzilDisplay() const
    {
        return QString::number(m_menzil) + " km";
    }

    QString tuketimDisplay() const
    {
        return QString::number(m_tuketim, 'f', 1) + " kWh";
    }

    // SİNYAL GETTER
    bool leftSignal() const { return m_leftSignal; }
    bool rightSignal() const { return m_rightSignal; }

private slots:
    void readData();

signals:

    // NORMAL VERİ SİNYALLERİ
    void speedValueChanged();
    void dataChanged();

    // SİNYAL EVENTLERİ
    void leftSignalChanged();
    void rightSignalChanged();

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
};



#endif // SERIALHANDLER_H
