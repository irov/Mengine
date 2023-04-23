#include "AppleEnvironmentService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleEnvironmentService, Mengine::AppleEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleEnvironmentService::AppleEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleEnvironmentService::~AppleEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleEnvironmentService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleEnvironmentService::_finalizeService()
    {
        for( IntrusiveListConstStringHolderNSString::iterator
            it = m_holdersNSString.begin();
            it != m_holdersNSString.end(); )
        {
            IntrusiveListConstStringHolderNSString::iterator it_erase = it;

            ConstStringHolderNSString * holder = *it;
            ++it;

            m_holdersNSString.erase( it_erase );

            m_poolNSString.destroyT( holder );
        }

        m_holdersNSString.clear();
        m_poolNSString.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleEnvironmentService::stringize( NSString * _value, ConstString * const _cstr ) const
    {
        if( [_value length] == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        ConstStringHolderNSString * holder = m_poolNSString.createT();

        holder->setNSString( _value );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            m_poolNSString.destroyT( holder );

            return;
        }

        m_holdersPythonString.push_back( holder );
    }
    //////////////////////////////////////////////////////////////////////////    
}