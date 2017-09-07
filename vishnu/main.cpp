#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSystemSemaphore>

#include "MainWindow.h"
#include "utils/Auxiliars.hpp"
#include "Definitions.hpp"

#include <fstream>

int main( int argc, char *argv[] )
{
  QApplication app( argc, argv );
  QApplication::setApplicationVersion( "1.0" );
  QApplication::setApplicationName( "Launcher" );
  QApplication::setOrganizationName( "GMRV - URJC" );

  const QString sharedMemorykey = QApplication::applicationName( ) +
    QString( "-Key" );
  const QString semaphoreKey = QApplication::applicationName( ) +
    QString( "-SemKey" );

  QSystemSemaphore semaphore( semaphoreKey, 1 );
  semaphore.acquire();

  #ifdef __linux__
  // On unix, shared memory is not freed upon crash
  QSharedMemory unixSharedMemory( sharedMemorykey );
  //bool detach = false;
  if ( unixSharedMemory.attach( ) == false )
  {
    /*detach = */unixSharedMemory.detach( );
  }
  unixSharedMemory.detach( );
  #endif

  QSharedMemory sharedMemory( sharedMemorykey );
  bool running = sharedMemory.attach( );
  if ( !running )
  {
    sharedMemory.create( 1 );
  }

  semaphore.release( );

  if( running )
  {
    QMessageBox::warning( NULL, "Warning!", "Another instance of " +
      QApplication::applicationName( ) + " is already running!" );
    app.exit( );
    return 1;
  }

  std::map<std::string, std::string> args = Auxiliars::splitArgs( argc, argv );

   //ZeqSession
  auto it = args.find( "-z" );
  if ( it == args.end( ) )
  {
    args["-z"] = DEFAULT_ZEQ_SESSION;
  }

  //XML filename
  it = args.find( "-f" );
  if ( it != args.end( ) )
  {
    std::ifstream fileExists( it->second );
    if ( !fileExists )
    {
      args.erase( it );
      std::cerr << "Info: Filename '" << it->second << "'' not found!" <<
        std::endl;
    }
  }

  vishnu::MainWindow window( args );
  QResource::registerResource( "resources.rcc" );
  window.setWindowTitle( QApplication::applicationName( ) );
  window.show( );
  return app.exec( );
}
