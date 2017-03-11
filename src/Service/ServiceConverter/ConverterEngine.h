#	pragma once

#	include "Interface/ConverterInterface.h"

#   include "Core/ServiceBase.h"

#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

namespace Menge
{
	class ConverterFactoryInterface;
	class ConverterInterface;


	class ConverterEngine
		: public ServiceBase<ConverterServiceInterface>
	{
	public:
		ConverterEngine();
		~ConverterEngine();

	public:
		bool registerConverter( const ConstString & _type, const ConverterFactoryInterfacePtr & _interface ) override;
		bool unregisterConverter( const ConstString & _type ) override;

		ConverterInterfacePtr createConverter( const ConstString & _type ) override; 

    public:
        bool convert( const ConstString & _converter, const ConstString & _category, const ConstString & _in, ConstString & _out ) override;
			
	protected:
		typedef stdex::map<ConstString, ConverterFactoryInterfacePtr> TMapConverterSystem;
		TMapConverterSystem m_mapConverterSystem;
	};
}
