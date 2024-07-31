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
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderNSString::setNSString( NSString * _value )
    {
        m_value = [_value mutableCopy];
        
        const char * data = [m_value UTF8String];
        size_t size = [m_value length];
        int64_t hash = Helper::makeHash( data, size );

        this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
    }
    //////////////////////////////////////////////////////////////////////////
}
