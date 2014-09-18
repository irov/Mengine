#	pragma once

#   include "Factory/Factorable.h"

#   include "stdex/intrusive_ptr.h"

#   include <stddef.h>

//typedef stdex::intrusive_ptr<Type> TypePtr;

namespace Menge
{
	class FactorablePtr
        : public Factorable
	{
	public:
		FactorablePtr();
		~FactorablePtr();

	private:
		void destroy() override;

	public:
        size_t getReference() const;

	public:
        inline static void intrusive_ptr_add_ref( FactorablePtr * _ptr );
        inline static void intrusive_ptr_dec_ref( FactorablePtr * _ptr );

#	ifdef STDEX_INTRUSIVE_PTR_DEBUG
	public:
		inline static bool intrusive_ptr_check_ref( FactorablePtr * _ptr );
#	endif
		
#   ifdef MENGINE_FACTORABLE_DEBUG
	protected:
        void _checkDestroy() override;
#   endif

    protected:
        size_t m_reference;
	};
    //////////////////////////////////////////////////////////////////////////
	inline void FactorablePtr::intrusive_ptr_add_ref( FactorablePtr * _ptr )
    {
        ++_ptr->m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void FactorablePtr::intrusive_ptr_dec_ref( FactorablePtr * _ptr )
    {
        if( --_ptr->m_reference == 0 )
        {
            _ptr->destroy();
        }
    }
	//////////////////////////////////////////////////////////////////////////
#	ifdef STDEX_INTRUSIVE_PTR_DEBUG
	inline bool FactorablePtr::intrusive_ptr_check_ref( FactorablePtr * _ptr )
	{
#   ifdef MENGINE_FACTORABLE_DEBUG
		if( _ptr->isDestroyed() == true )
		{
			return false;
		}
#	endif

		if( _ptr->m_reference == 0 )
		{
			return false;
		}

		return true;
	}
#	endif
}
