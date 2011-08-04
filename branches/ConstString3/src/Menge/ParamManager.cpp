#	include "ParamManager.h"

#	include "Loadable.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "LogEngine.h"

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
						param.resize(16);

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value, param[0] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value1, param[1] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value2, param[2] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value3, param[3] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value4, param[4] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value5, param[5] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value6, param[6] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value7, param[7] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value8, param[8] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value9, param[9] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value10, param[10] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value11, param[11] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value12, param[12] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value13, param[13] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value14, param[14] );
							BIN_CASE_ATTRIBUTE( Protocol::Param_Value15, param[15] );
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
	bool ParamManager::registerParam( const ConstString& _category, const ConstString& _group, const String& _file )
	{
		TMapParams::iterator it_found = m_params.find( _group );

		if( it_found != m_params.end() )
		{
			MENGE_LOG_ERROR( "ParamManager: alredy have param '%s'"
				, _group.c_str() 
				);

			return false;
		}

		Param param;
		param.category = _category;
		param.file = _file;

		m_params.insert( std::make_pair(_group, param) );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParamManager::getParam( const ConstString& _group, TVectorParams & _params )
	{
		TMapParams::iterator it_found = m_params.find( _group );

		if( it_found == m_params.end() )
		{
			MENGE_LOG_ERROR( "ParamManager: alredy have param '%s'"
				, _group.c_str() 
				);

			return false;
		}

		LoadableParamManager loadable(_params);

		bool exist = false;
		if( LoaderEngine::get()
			->load( it_found->second.category, it_found->second.file, &loadable, exist ) == false )
		{
			MENGE_LOG_ERROR( "ParamManager: Invalid parse param %s:%s"
				, it_found->second.category.c_str()
				, it_found->second.file.c_str()
				);

			return false;
		}

		return true;
	}
}