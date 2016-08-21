#	pragma once

namespace Menge
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
				return nullptr;
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