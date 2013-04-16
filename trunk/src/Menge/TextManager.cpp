#	include "TextManager.h"

#	include "Core/String.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#   include <stdio.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TextService, Menge::TextServiceInterface, Menge::TextManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextManager()
        : m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::~TextManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void TextManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * TextManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const TextEntry & _entry )
	{
		TMapTextEntry::iterator it_find = m_textMap.find( _key );
		if( it_find != m_textMap.end() )
		{
			LOGGER_INFO(m_serviceProvider)( "TextManager::addTextEntry: duplicate key found %s"
				, _key.c_str() 
				);

			it_find->second = _entry;
		}
		else
		{
			m_textMap.insert( std::make_pair( _key, _entry ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const TextEntry & TextManager::getTextEntry( const ConstString& _key ) const
	{
		TMapTextEntry::const_iterator it_find = m_textMap.find( _key );
		
		if( it_find == m_textMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			static TextEntry emptyEntry;
			emptyEntry.charOffset = 0.f;
			emptyEntry.lineOffset = 0.f;

			return emptyEntry;
		}

        const TextEntry & entry = it_find->second;
        
        return entry;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key, const TextEntry ** _entry ) const
	{
		TMapTextEntry::const_iterator it_find = m_textMap.find( _key );

        if( it_find == m_textMap.end() )
        {
            return false;
        }

        if( _entry != NULL )
        {
            *_entry = &it_find->second;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::setDefaultResourceFontName( const ConstString & _fontName )
	{
		m_defaultResourceFontName = _fontName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextManager::getDefaultResourceFontName() const
	{
		return m_defaultResourceFontName;
	}
}
