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
			LoadableParamManager( TVectorParams & _param )
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
						TVectorString param;
						param.reserve(8);

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value1, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value2, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value3, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value4, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value5, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value6, &param, &TVectorString::push_back );
							BIN_CASE_ATTRIBUTE_METHOD_PTR( Protocol::Param_Value7, &param, &TVectorString::push_back );
						}

						if( param.empty() == true )
						{
							BIN_SKIP();
						}

						m_param.push_back( param );
					}
				}
			}

		protected:
			TVectorParams & m_param;
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

		TVectorParams param;
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

		m_params.insert( std::make_pair(_group, param) );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TMapParams & ParamManager::getParams() const
	{
		return m_params;
	}
}