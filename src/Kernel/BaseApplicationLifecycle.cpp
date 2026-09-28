#include "BaseApplicationLifecycle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseApplicationLifecycle::BaseApplicationLifecycle()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseApplicationLifecycle::~BaseApplicationLifecycle()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationDidBecomeActive()
    {
        this->_onApplicationDidBecomeActive();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationWillEnterForeground()
    {
        this->_onApplicationWillEnterForeground();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationDidEnterBackground()
    {
        this->_onApplicationDidEnterBackground();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationWillResignActive()
    {
        this->_onApplicationWillResignActive();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationWillTerminate()
    {
        this->_onApplicationWillTerminate();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationDidReceiveMemoryWarning()
    {
        this->_onApplicationDidReceiveMemoryWarning();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::onApplicationDidReceiveTrimMemory( int32_t _level )
    {
        this->_onApplicationDidReceiveTrimMemory( _level );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationDidBecomeActive()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationWillEnterForeground()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationDidEnterBackground()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationWillResignActive()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationWillTerminate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationDidReceiveMemoryWarning()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseApplicationLifecycle::_onApplicationDidReceiveTrimMemory( int32_t _level )
    {
        MENGINE_UNUSED( _level );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
