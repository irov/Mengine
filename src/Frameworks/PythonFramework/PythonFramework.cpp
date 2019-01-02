#include "PythonFramework.h"

#include "PythonWrapper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Framework, Mengine::PythonFramework );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::PythonFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::~PythonFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFramework::_initializeService()
    {
        if( PythonWrapper::constsWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::helperWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::nodeWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::mathWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::entityWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::engineWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::soundWrap() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::onFrameworkInitialize()
    {
        if( PythonWrapper::gameWrap() == false )
        {
            return;
        }
    }
}
