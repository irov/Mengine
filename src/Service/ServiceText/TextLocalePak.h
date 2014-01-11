# pragma once

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
	class TextLocalePak
			: public FactorablePtr
	{
	public:
		TextLocalePak();
		~TextLocalePak();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _locale, const ConstString & _pakName, const FilePath & _path );
		
	public:
		char * getXmlBuffer() const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_locale;
		char * m_xml_buffer;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextLocalePak> TextLocalePakPtr;
}
