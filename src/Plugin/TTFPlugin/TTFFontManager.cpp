#	pragma once

#	include "TTFFontManager.h"

#	include "Interface/FileSystemInterface.h"

#	include "TTFFont.h"

#	include "Core/MemoryHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TTFFontManager, Menge::TTFFontManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	TTFFontManager::TTFFontManager()
		: m_library(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFFontManager::~TTFFontManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFontManager::_initialize()
	{
		if( FT_Init_FreeType( &m_library ) )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFFontManager::_finalize()
	{
		FT_Done_FreeType( m_library );
		m_library = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	TTFFontInterfacePtr TTFFontManager::createFont( const ConstString & _name, const ConstString& _pakName, const FilePath& _fileName )
	{
		InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openInputFile( _pakName, _fileName, false );

		MemoryInterfacePtr memory = Helper::createMemoryStream( m_serviceProvider, stream );

		TTFFontPtr font = new TTFFont;

		font->initialize( m_library, memory );

		m_fonts.insert( std::make_pair( _name, font ) );

		return font;
	}
	//////////////////////////////////////////////////////////////////////////
	TTFFontInterfacePtr TTFFontManager::getFont( const ConstString & _name ) const
	{
		TMapFonts::const_iterator it_found = m_fonts.find( _name );

		if( it_found == m_fonts.end() )
		{
			return nullptr;
		}

		const TTFFontInterfacePtr & font = it_found->second;

		return font;
	}
}