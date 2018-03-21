#pragma once

#include "Interface/COMInterface.h"

namespace Mengine
{
	class Comable
	{
	public:
		virtual COMInterface * getCOMInterface()
		{
			return nullptr;
		}

		template<class T>
		T * getCOMInterfaceT()
		{
			COMInterface * com = this->getCOMInterface();
#	ifdef _DEBUG
			if(dynamic_cast<T *>(com) == nullptr)
			{
                throw;
			}
#	endif

			return static_cast<T *>(com);
		}
	};

#	define DECLARE_COMABLE()\
public:\
	COMInterface * getCOMInterface() override{ return this; }\
protected:

}