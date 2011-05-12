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
			LoadableParamManager( Params & _param )
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
						String key;

						TVectorString values;
						values.reserve(8);

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE( Protocol::Param_Key, key );

							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value1, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value2, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value3, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value4, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value5, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value6, &values, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value7, &values, &TVectorString::push_back );
						}

						if( values.empty() == true )
						{
							continue;
						}

						m_param.mp[key].push_back( values );
					}
				}
			}

		protected:
			Params & m_param;
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

		Params param;
		LoadableParamManager loadable(param);

		bool exist = false;
		if( LoaderEngine::get()
			->load( _category, _file, &loadable, exist ) == false )
		{
			MENGE_LOG_ERROR( "ParamManager: Invalid parse resource '%s'"
				, _file.c_str() 
				);

			return false;
		}

		//m_params.insert( std::make_pair(_group, param) );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TMapParams & ParamManager::getParams() const
	{
		return m_params;
	}
}