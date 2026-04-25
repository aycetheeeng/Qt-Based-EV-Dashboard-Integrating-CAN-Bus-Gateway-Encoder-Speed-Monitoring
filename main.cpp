#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "serialhandler.h"
#include "autogen/environment.h" // Otomatik yollar için şart

int main(int argc, char *argv[])
{
    // 1. Qt Ortamını Hazırla (Bu ekranın gelmesini sağlar)
    set_qt_environment();

    QApplication app(argc, argv);

    // 2. Seri Port Nesnesini Oluştur
    SerialHandler serialHandler;

    QQmlApplicationEngine engine;

    // 3. QML'e Veriyi Tanıt (Daha yükleme yapmadan önce!)
    engine.rootContext()->setContextProperty("serialHandler", &serialHandler);

    // 4. Dosya Yolunu Otomatik Al
    // (mainQmlFile, environment.h içinde tanımlıdır, o yüzden UI açılır)
    const QUrl url(mainQmlFile);

    // 5. Hata Kontrolü (Bağlantı başarılı mı?)
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    // 6. Import Yollarını Ekle (Resimler ve fontlar için)
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");

    // 7. Arayüzü Yükle
    engine.load(url);

    // 8. Eğer yüklenemezse uygulamayı kapat
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
