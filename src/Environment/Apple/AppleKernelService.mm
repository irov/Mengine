#include "AppleKernelService.h"

#include "Interface/StringizeServiceInterface.h"

#include "Config/StdString.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleKernelService, Mengine::AppleKernelService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleKernelService::AppleKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleKernelService::~AppleKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleKernelService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleKernelService::_finalizeService()
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
    void AppleKernelService::stringize( NSString * _value, ConstString * const _cstr )
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

        m_holdersNSString.push_back( holder );
    }
    //////////////////////////////////////////////////////////////////////////    
}
