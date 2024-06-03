#include <QCoreApplication>
#include "servermanager.h"
#include <memory>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "RU");
    QCoreApplication app(argc, argv);

    int port = 7001;

    std::shared_ptr<ServerManager> serverManager = std::make_shared<ServerManager>(port);

    return app.exec();
}
