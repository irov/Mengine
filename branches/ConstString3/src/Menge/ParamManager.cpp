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
	bool ParamManager::registerParam( const ConstString& _name, const ResourceDesc & _desc )
	{
		TMapParams::iterator it_found = m_params.find( _name );

		if( it_found != m_params.end() )
		{
			MENGE_LOG_ERROR( "ParamManager: alredy have param '%s'"
				, _name.c_str() 
				);

			return false;
		}

		m_params.insert( std::make_pair(_name, _desc) );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParamManager::getParam( const ConstString& _name, TVectorParams & _params )
	{
		TMapParams::iterator it_found = m_params.find( _name );

		if( it_found == m_params.end() )
		{
			MENGE_LOG_ERROR( "ParamManager: alredy have param '%s'"
				, _name.c_str() 
				);

			return false;
		}

		const ResourceDesc & desc = it_found->second;

		String xml_path = desc.path;
		xml_path += "/";
		xml_path += Helper::to_str(_name);

		LoadableParamManager loadable(_params);

		bool exist = false;
		if( LoaderEngine::get()
			->load( desc.pak, xml_path, &loadable, exist ) == false )
		{
			MENGE_LOG_ERROR( "ParamManager: Invalid parse param %s:%s"
				, desc.pak.c_str()
				, xml_path.c_str()
				);

			return false;
		}

		return true;
	}
}