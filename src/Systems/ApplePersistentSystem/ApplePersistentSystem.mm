#include "ApplePersistentSystem.h"

#include "Environment/Apple/AppleUserDefaults.h"

#include "Kernel/StringCopy.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PersistentSystem, Mengine::ApplePersistentSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ApplePersistentSystem::ApplePersistentSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ApplePersistentSystem::~ApplePersistentSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePersistentSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ApplePersistentSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ApplePersistentSystem::getPersistentArguments( Char * const _value, size_t _capacity ) const
    {
        NSString * arguments = Helper::AppleGetUserDefaultsString( @("mengine.persistent.arguments"), nil );

        if( arguments == nil )
        {
            return false;
        }
        
        const Char * arguments_value = [arguments UTF8String];
        
        Helper::stringCopy( _value, arguments_value, _capacity );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ApplePersistentSystem::setPersistentArguments( const Char * _value )
    {
        Helper::AppleSetUserDefaultsString( @("mengine.persistent.arguments"), @(_value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ApplePersistentSystem::removePersistentArguments()
    {
        Helper::AppleRemoveUserDefaults( @("mengine.persistent.arguments") );
    }
    //////////////////////////////////////////////////////////////////////////
}
