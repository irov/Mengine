#	pragma once

#	include "Interface/ConverterInterface.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class ConverterSystemInterface;
	class ConverterInterface;


	class ConverterEngine
		: public Holder<ConverterEngine>
		, public ConverterServiceInterface
	{
	public:
		ConverterEngine();
		~ConverterEngine();

	public:
		virtual void registerConverter( const ConstString& _type, ConverterSystemInterface * _interface ) override;
		virtual void unregisterConverter( const ConstString& _type ) override;

		virtual ConverterInterface * createConverter( const ConstString & _type )override; 
			
	protected:
		typedef std::map<ConstString, ConverterSystemInterface *> TMapConverterSystem;
		TMapConverterSystem m_mapConverterSystem;
	};
}
