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
        if( m_value != nil )
		{
			[m_value release];
			m_value = nil;
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderNSString::setNSString( NSString * _value )
    {
        if( m_value != nil )
		{
			[m_value release];
			m_value = nil;
		}

        m_value = _value;

        if( m_value != nil )
        {
            [m_value retain];

            const char * data = [m_value UTF8String];
            uint32_t size = MENGINE_STRLEN( data );
            int64_t hash = Helper::makeHash( _str, _size );

            this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
        }
        else
        {
            this->setup( "", 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}