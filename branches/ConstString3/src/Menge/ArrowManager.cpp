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
	}
	//////////////////////////////////////////////////////////////////////////
	void ArrowManager::registerArrow( const ConstString & _name, const ResourceDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_name, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * ArrowManager::createArrow( const ConstString & _name, const ConstString & _prototype )
	{
		Arrow * arrow = this->createArrow_( _name, _prototype );

		return arrow;
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

		const ResourceDesc & desc = it_find->second;

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

		return arrow;
	}
}