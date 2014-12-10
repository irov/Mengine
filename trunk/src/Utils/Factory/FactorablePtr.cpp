#	include "FactorablePtr.h"

#	include "Core/Exception.h"

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
#   ifdef MENGINE_FACTORABLE_DEBUG
		if( m_reference != 0 )
		{
			MENGINE_THROW_EXCEPTION("m_reference %d != 0"
				, m_reference
				);
		}
#	endif
    }
	//////////////////////////////////////////////////////////////////////////
	void FactorablePtr::destroy()
	{
		Factorable::destroy();
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactorablePtr::getReference() const
    {
        return m_reference;
    }  
    //////////////////////////////////////////////////////////////////////////
#   ifdef MENGINE_FACTORABLE_DEBUG
    void FactorablePtr::_checkDestroy()
    {
        if( m_reference != 0 )
        {
            MENGINE_THROW_EXCEPTION("m_reference %d != 0"
				, m_reference
				);
        }
    }
#   endif
}
