#include "iOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "AndroidProxyLogger.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ProxyLogger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( iOSEnvironmentService, Mengine::iOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::~iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}