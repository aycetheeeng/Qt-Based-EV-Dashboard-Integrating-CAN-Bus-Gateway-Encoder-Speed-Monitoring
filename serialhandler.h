#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

class SerialHandler : public QObject {
    Q_OBJECT

    // --- QML Erişimi İçin Tanımlamalar ---
    // speedValue senin orijinal hız okuman
    Q_PROPERTY(int speedValue READ speedValue NOTIFY speedValueChanged)

    // Diğer tüm veriler (Değer halleri)
    Q_PROPERTY(float kwValue READ kwValue NOTIFY dataChanged)
    Q_PROPERTY(int bataryaValue READ bataryaValue NOTIFY dataChanged)
    Q_PROPERTY(int menzilValue READ menzilValue NOTIFY dataChanged)
    Q_PROPERTY(float tuketimValue READ tuketimValue NOTIFY dataChanged)
    Q_PROPERTY(float tripValue READ tripValue NOTIFY dataChanged)

    // UI (.ui.qml) dosyalarında hata almamak için FORMATLANMIŞ metin halleri
    Q_PROPERTY(QString kwDisplay READ kwDisplay NOTIFY dataChanged)
    Q_PROPERTY(QString menzilDisplay READ menzilDisplay NOTIFY dataChanged)
    Q_PROPERTY(QString tuketimDisplay READ tuketimDisplay NOTIFY dataChanged)

public:
    explicit SerialHandler(QObject *parent = nullptr);

    // --- READ Fonksiyonları (Getter'lar) ---
    // Hataların sebebi buradaki isimlerin eksik olmasıydı.
    int speedValue() const { return m_speed; }
    float kwValue() const { return m_kw; }
    int bataryaValue() const { return m_batarya; }
    int menzilValue() const { return m_menzil; }
    float tuketimValue() const { return m_tuketim; }
    float tripValue() const { return m_toplamKwh; }

    // UI dosyası için hazır string fonksiyonları
    QString kwDisplay() const { return QString::number(m_kw, 'f', 1) + " kW"; }
    QString menzilDisplay() const { return QString::number(m_menzil) + " km"; }
    QString tuketimDisplay() const {
        return QString::number(m_tuketim, 'f', 1) + " kWh"; }

private slots:
    void readData(); // Seri porttan veri geldikçe çalışır

signals:
    void speedValueChanged(); // Sadece hız değişince tetiklenir
    void dataChanged();       // Tüm paket güncellenince tetiklenir

private:
    QSerialPort *m_serial;
    QByteArray m_buffer;

    // Arka plan değişkenleri (İlk değerlerini sıfır veriyoruz ki çöp değer gelmesin)
    int m_speed = 0;
    float m_kw = 0.0f;
    float m_tuketim = 0.0f;
    int m_batarya = 0;
    int m_menzil = 0;
    float m_toplamKwh = 0.0f;
};

#endif // SERIALHANDLER_H
