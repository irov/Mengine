#	include "TextManager.h"

#	include "LoaderEngine.h"
#   include "Metacode.h"

#	include "LogEngine.h"

#	include "Application.h"

#	include "Utils/Core/String.h"

#	include "ServiceProvider.h"
#	include "Interface/UnicodeInterface.h"

//#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::~TextManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::loadResource( const ResourceDesc & _desc )
	{
		bool exist = false;

        Metacode::Meta_Texts meta_texts;
        
        if( LoaderEngine::get()
        	->load( _desc.pakName, _desc.path, &meta_texts, exist ) == false )
        {
        	MENGE_LOG_ERROR( "TextManager::loadResource Problems parsing Text pack %s:%S"
        		, _desc.name.c_str()
        		, _desc.path.c_str()
        		);

        	return false;
        }

        const Metacode::Meta_Texts::TVectorMeta_Text & includes_text = meta_texts.get_IncludesText();

        for( Metacode::Meta_Texts::TVectorMeta_Text::const_iterator
            it = includes_text.begin(),
            it_end = includes_text.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_Texts::Meta_Text & meta_text = *it;

            TextEntry textEntry;
                        
            const ConstString & key = meta_text.get_Key();

            meta_text.swap_Value( textEntry.text );

            if( meta_text.get_Font( textEntry.font ) == false )
            {
                textEntry.font = m_currentFont;
            }

            if( meta_text.get_CharOffset( textEntry.charOffset ) == false )
            {
                textEntry.charOffset = 0.f;
            }

            if( meta_text.get_CharOffset( textEntry.lineOffset ) == false )
            {
                textEntry.lineOffset = 0.f;
            }
            
            this->addTextEntry( key, textEntry );
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const TextEntry & _entry )
	{
		TMapTextEntry::iterator it_find = m_textMap.find( _key );
		if( it_find != m_textMap.end() )
		{
			MENGE_LOG_INFO( "TextManager::addTextEntry: duplicate key found %s"
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
			MENGE_LOG_ERROR( "TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			static TextEntry emptyEntry;
			emptyEntry.charOffset = 0.0f;
			emptyEntry.lineOffset = 0.0f;

			return emptyEntry;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key ) const
	{
		TMapTextEntry::const_iterator it_find = m_textMap.find( _key );
		return it_find != m_textMap.end();
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
