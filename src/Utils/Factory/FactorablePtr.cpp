#	include "FactorablePtr.h"

#   ifdef _DEBUG
#   include <exception>
#   endif

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    FactorablePtr::FactorablePtr()
        : m_reference(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePtr::~FactorablePtr()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t FactorablePtr::getReference() const
    {
        return m_reference;
    }  
    //////////////////////////////////////////////////////////////////////////
#   ifdef _DEBUG
    void FactorablePtr::_checkDestroy()
    {
        if( m_reference != 0 )
        {
            throw std::exception();
        }
    }
#   endif
}
