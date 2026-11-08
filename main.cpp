#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "serialhandler.h"
#include "autogen/environment.h" // Otomatik yollar için şart 

// 13.06.2026

int main(int argc, char *argv[])
{
    // Qt Ortamını Hazırla 
    set_qt_environment();

    QApplication app(argc, argv);

    // Seri Port Nesnesini Oluştur
    SerialHandler serialHandler;

    QQmlApplicationEngine engine;

    // QML'e Veriyi Tanıt 
    engine.rootContext()->setContextProperty("serialHandler", &serialHandler);

    // Dosya Yolunu Otomatik Al
    // (mainQmlFile, environment.h içinde tanımlıdır, o yüzden UI açılır)
    const QUrl url(mainQmlFile);

    // Bağlantı başarılı mı
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    // Import Yollarını Ekle (Resimler ve fontlar için)
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");

    // Arayüzü Yükle
    engine.load(url);

    // Eğer yüklenemezse uygulamayı kapat
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
