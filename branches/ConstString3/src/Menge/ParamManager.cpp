#	include "ParamManager.h"

#	include "Loadable.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ParamManager::ParamManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParamManager::~ParamManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class LoadableParamManager
			: public Loadable
		{
		public:
			LoadableParamManager( TMapParams::iterator _param )
				: m_param(_param)
			{
			}

		protected:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE_PARSE_METHOD( Protocol::Params, this, &LoadableParamManager::loaderParams_ );
				}
			}

		private:
			void loaderParams_( BinParser * _parser )
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE( Protocol::Param )
					{
						String name;
						String value;

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE( Protocol::Param_Key, name );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value, value );
						}

						if( name.empty() || value.empty() )
						{
							continue;
						}

						m_param->second.insert( std::make_pair(name, value) );
					}
				}
			}

		protected:
			TMapParams::iterator m_param;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParamManager::loadParam( const ConstString& _category, const ConstString& _group, const String& _file )
	{
		TMapParams::iterator it_found = m_params.find( _group );

		if( it_found != m_params.end() )
		{
			MENGE_LOG_ERROR( "ParamManager: alredy have param '%s'"
				, _group.c_str() 
				);

			return false;
		}

		it_found = m_params.insert( std::make_pair(_group, TMapParam()) ).first;

		LoadableParamManager loadable(it_found);

		bool exist = false;
		if( LoaderEngine::get()
			->load( _category, _file, &loadable, exist ) == false )
		{
			MENGE_LOG_ERROR( "ParamManager: Invalid parse resource '%s'"
				, _file.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TMapParams & ParamManager::getParams() const
	{
		return m_params;
	}
}