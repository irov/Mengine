#pragma once

#include "Interface/ConverterInterface.h"

#include "Core/ServiceBase.h"

#include "Core/ConstString.h"

#include "Config/Map.h"

namespace Mengine
{
	class ConverterFactoryInterface;
	class ConverterInterface;


	class ConverterEngine
		: public ServiceBase<ConverterServiceInterface>
	{
	public:
		ConverterEngine();
		~ConverterEngine() override;

	public:
		bool registerConverter( const ConstString & _type, const ConverterFactoryInterfacePtr & _interface ) override;
		bool unregisterConverter( const ConstString & _type ) override;

		ConverterInterfacePtr createConverter( const ConstString & _type ) override; 

    public:
        bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _category, const FilePath & _in, FilePath & _out ) override;

	protected:
		typedef Map<ConstString, ConverterFactoryInterfacePtr> TMapConverterSystem;
		TMapConverterSystem m_mapConverterSystem;
	};
}
