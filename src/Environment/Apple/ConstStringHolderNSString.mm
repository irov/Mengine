#include "ConstStringHolderNSString.h"

#include "Kernel/HashHelper.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderNSString::ConstStringHolderNSString()
        : m_value( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderNSString::~ConstStringHolderNSString()
    {
        m_value = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderNSString::setNSString( const NSString * _value )
    {
        if( _value != nil )
        {
            const char * data = [m_value UTF8String];
            size_t size = MENGINE_STRLEN( data );
            int64_t hash = Helper::makeHash( data, size );

            this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
        }
        else
        {
            this->setup( "", 0, 0 );
        }
        
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
}
