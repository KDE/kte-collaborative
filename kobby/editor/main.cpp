#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include <kobby/editor/mainwindow.h>

#include <libqinfinity/init.h>

int main( int argc, char **argv )
{
    int ret;
    KAboutData aboutData( "kobby", "kobby",
        ki18n( "kobby" ), "1.0",
        ki18n( "Collaborative text editing environment." ),
        KAboutData::License_GPL,
        ki18n( "Copyright (c) 2008 Gregory Haynes" ) );
    
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app;

    QInfinity::init();
    
    Kobby::MainWindow *mainWindow = new Kobby::MainWindow();
    mainWindow->show();
    
    ret = app.exec();
    QInfinity::deinit();
    return ret;
}
