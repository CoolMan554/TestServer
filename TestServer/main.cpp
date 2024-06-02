#include <QCoreApplication>
#include "servermanager.h"
#include <memory>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "RU");
    QCoreApplication app(argc, argv);

    std::shared_ptr<ServerManager> serverManager = std::make_shared<ServerManager>(7001);

    return app.exec();
}
