#	include "TextLocalePak.h"

#   include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include <stdex/xml_sax_parser.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLocalePak::TextLocalePak()
		: m_serviceProvider(nullptr)
		, m_xml_buffer(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextLocalePak::~TextLocalePak()
	{
		delete [] m_xml_buffer;
		m_xml_buffer = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextLocalePak::initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		m_serviceProvider = _serviceProvider;
		m_locale = _locale;

		InputStreamInterfacePtr xml_text = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pakName, _path );

		if( xml_text == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource invalid open file %s:%s"
				, _pakName.c_str()
				, _path.c_str()
				);

			return false;
		}

		size_t xml_buffer_size = xml_text->size();

		m_xml_buffer = new char[xml_buffer_size + 1];
		m_xml_buffer[xml_buffer_size] = '\0';

		xml_text->read( m_xml_buffer, xml_buffer_size );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	char * TextLocalePak::getXmlBuffer() const
	{
		return m_xml_buffer;
	}
}