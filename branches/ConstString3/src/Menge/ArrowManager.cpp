#	include "ArrowManager.h"
#	include "Arrow.h"

#	include <memory>

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "LogEngine.h"

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
	Arrow * ArrowManager::createArrow( const ConstString & _name, const ConstString & _prototype )
	{
		TMapArrows::iterator it_find = m_arrows.find( _name );

		if( it_find == m_arrows.end() )
		{
			Arrow * arrow = this->createArrow_( _name, _prototype );

			if( arrow == 0 )
			{
				return 0;
			}

			it_find = m_arrows.insert( std::make_pair( _name, arrow ) ).first;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class ArrowLoadable
			: public Loadable
		{
		public:
			ArrowLoadable( Arrow * _arrow )
				: m_arrow(_arrow)
			{

			}

		protected:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE_PARSE( Protocol::Arrow, m_arrow );
				}
			}

		protected:
			void _loaded() override
			{
				m_arrow->loaded();
			}

		protected:
			Arrow * m_arrow;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * ArrowManager::createArrow_( const ConstString & _name, const ConstString & _prototype )
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

		const ConstString & type = Consts::get()->c_Arrow;

		Arrow * arrow = ScriptEngine::get()
			->createEntityT<Arrow>( _name, type, type, _prototype, desc.pak, desc.path );

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "Can't create arrow '%s'"
				, _name.c_str() 
				); 

			return 0;
		}

		arrow->loaded();

		//String xml_path = Helper::to_str(desc.path);
		//xml_path += "/";
		//xml_path += Helper::to_str(_name);
		//xml_path += "/";
		//xml_path += Helper::to_str(_name);

		//ArrowLoadable loadable(arrow);

		//bool exist;
		//if( LoaderEngine::get()
		//	->cache( desc.pak, xml_path, &loadable, exist ) == false )
		//{
		//	if( exist == true )
		//	{
		//		MENGE_LOG_ERROR( "Warning: invalid loader xml '%s' for arrow '%s'(invalid binary)"
		//			, xml_path.c_str()
		//			, _name.c_str() 
		//			);

		//		arrow->destroy();

		//		return 0;
		//	}
		//}

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