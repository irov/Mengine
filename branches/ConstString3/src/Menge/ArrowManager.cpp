#	include "ArrowManager.h"
#	include "Arrow.h"

#	include "XmlEngine.h"
#	include "ScriptEngine.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ArrowManager::ArrowManager()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ArrowManager::~ArrowManager()
	{
		for( TMapArrows::iterator
			it = m_arrows.begin(),
			it_end = m_arrows.end();
		it != it_end;
		++it)
		{
			it->second->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ArrowManager::registerArrow( const ConstString & _name, const ArrowDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_name, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * ArrowManager::getArrow( const ConstString & _name )
	{
		TMapArrows::iterator it_find = m_arrows.find( _name );

		if( it_find == m_arrows.end() )
		{
			Arrow * arrow = this->createArrow_( _name );

			it_find = m_arrows.insert( std::make_pair( _name, arrow ) ).first;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * ArrowManager::createArrow_( const ConstString & _name )
	{
		TMapDescriptionArrows::iterator it_find = m_descriptions.find( _name );

		if( it_find == m_descriptions.end() )
		{
			MENGE_LOG_ERROR( "Error: Arrow '%s' declaration not found"
				, _name.c_str() 
				);

			return 0;
		}

		const ArrowDesc & desc = it_find->second;

		Arrow * arrow = ScriptEngine::get()
			->createNodeT<Arrow>( _name, Consts::get()->c_Arrow, Consts::get()->c_Arrow );

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "Can't create arrow '%s'"
				, _name.c_str() 
				); 

			return 0;
		}

		arrow->setName( _name );

		String xml_path = desc.path;
		xml_path += "/";
		xml_path += _name.str();
		xml_path += "/Arrow.xml";

		if( XmlEngine::get()
			->parseXmlFileM( desc.pak, xml_path, arrow, &Arrow::loader ) == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid loader xml '%s' for arrow '%s'"
				, xml_path.c_str()
				, _name.c_str() 
				);
		}

		return arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void ArrowManager::removeArrow( const ConstString& _name )
	{
		TMapArrows::iterator it_find = m_arrows.find( _name );

		if( it_find == m_arrows.end() )
		{
			MENGE_LOG_ERROR( "Can't find arrow '%s' to remove"
				, _name.c_str() 
				); 

			return;
		}

		it_find->second->destroy();

		m_arrows.erase( it_find );
	}
}