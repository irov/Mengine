#	include "FactorablePtr.h"

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
			STDEX_THROW_EXCEPTION("m_reference != 0");
		}
#	endif
    }
	//////////////////////////////////////////////////////////////////////////
	void FactorablePtr::destroy()
	{
		Factorable::destroy();
	}
    //////////////////////////////////////////////////////////////////////////
    size_t FactorablePtr::getReference() const
    {
        return m_reference;
    }  
    //////////////////////////////////////////////////////////////////////////
#   ifdef MENGINE_FACTORABLE_DEBUG
    void FactorablePtr::_checkDestroy()
    {
        if( m_reference != 0 )
        {
            STDEX_THROW_EXCEPTION("m_reference != 0");
        }
    }
#   endif
}
