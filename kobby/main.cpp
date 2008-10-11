#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include <kobby/editor/mainwindow.h>

int main( int argc, char **argv )
{
    KAboutData aboutData( "kobby", "kobby",
        ki18n( "Kobby" ), "1.0",
        ki18n( "Collaborative text editing environment." ),
        KAboutData::License_GPL,
        ki18n( "Copyright (c) 2008 Gregory Haynes" ) );
    
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app;
    
    Kobby::MainWindow *mainWindow = new Kobby::MainWindow();
    mainWindow->show();
    
    return app.exec();
}
