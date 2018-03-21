#pragma once

#include "Factory/Factorable.h"

#include "stdex/intrusive_ptr_base.h"

#include <stddef.h>
#include <stdint.h>

//typedef stdex::intrusive_ptr<Type> TypePtr;

namespace Mengine
{
	class FactorablePtr
        : public Factorable
		, public stdex::intrusive_ptr_base<FactorablePtr>
	{
	public:
		FactorablePtr();
		~FactorablePtr() override;

	private:
		void destroy() override;

	public:
        uint32_t getReference() const;

	public:
		inline static void intrusive_ptr_destroy( FactorablePtr * _ptr );

#	ifdef STDEX_INTRUSIVE_PTR_DEBUG
	public:
		inline static bool intrusive_ptr_check_ref( FactorablePtr * _ptr );
#	endif
		
#   ifdef MENGINE_FACTORABLE_DEBUG
	protected:
        void _checkDestroy() override;
#   endif
	};
	//////////////////////////////////////////////////////////////////////////
	inline void FactorablePtr::intrusive_ptr_destroy( FactorablePtr * _ptr )
	{
		_ptr->destroy();
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
