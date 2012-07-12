#	include "ResourcePak.h"

#	include "ScriptEngine.h"
#	include "TextManager.h"
#	include "ResourceManager.h"

#	include "SceneManager.h"
#	include "ArrowManager.h"
#	include "EntityManager.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "Consts.h"
#	include "LogEngine.h"
#	include "Utils/Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourcePak::ResourcePak( const ResourcePakDesc & _desc, const WString & _baseDir )
		: m_desc(_desc)
		, m_baseDir(_baseDir)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::preload() const
	{
		return m_desc.preload;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getName() const
	{
		return m_desc.name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getLocale() const
	{
		return m_desc.locale;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourcePak::getPlatfrom() const
	{
		return m_desc.platform;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourcePak::getPath() const
	{
		return m_desc.path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::load()
	{		
		WString fullPakPath = m_baseDir;
		fullPakPath += m_desc.path;
		
		if(  m_desc.type == Consts::get()->c_zip )
		{
			//fullPakPath += L".zip";
		}
		else
		{
			fullPakPath += MENGE_FOLDER_DELIM;
		}		

		if( FileEngine::get()
			->mountFileSystem( m_desc.name, fullPakPath, m_desc.type, false ) == false )
		{
			MENGE_LOG_ERROR( "ResourcePak::load failed to mount pak '%S': '%S'"
				, m_desc.name.c_str()
				, fullPakPath.c_str()
				);

			return false;
		}

		bool exist = false;
		if( LoaderEngine::get()
			->load( m_desc.name, m_desc.filename, this, exist ) == false )
		{
			MENGE_LOG_ERROR( "ResourcePak::load Invalid resource file '%S %s' '%S'"
				, m_desc.path.c_str()
				, m_desc.name.c_str()
				, m_desc.filename.c_str()
				);

			return false;
		}

		TVectorWString listModulePath;

		WString scriptPakPath = m_baseDir + m_desc.path;
		
		//if(  m_desc.type == Consts::get()->c_zip )
		//{
			//scriptPakPath += L".zip";
		//}
		
		for( TVectorWString::iterator
			it = m_pathScripts.begin(),
			it_end = m_pathScripts.end();
		it != it_end;
		++it )
		{
			WString path = scriptPakPath + MENGE_FOLDER_DELIM + *it;
			//Utils::collapsePath( scriptPakPath + "\\" + *it, path );
			listModulePath.push_back( path );
		}

		ScriptEngine::get()
			->addModulePath( listModulePath );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::apply()
	{
		for( TResourceDescs::iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
			ResourceManager::get()
				->loadResource( *it );
		}

		for( TTextDescs::iterator
			it = m_textsDesc.begin(),
			it_end = m_textsDesc.end();
		it != it_end;
		++it )
		{
			TextManager::get()
				->loadResource( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD( Protocol::Pak, this, &ResourcePak::loaderPak_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderPak_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Resources )
			{
				WString path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Resources_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderResources_, path );
			}

			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Scripts_Path, &ResourcePak::addScriptPath_ );

			BIN_CASE_NODE( Protocol::Texts )
			{
				WString path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Texts_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderTexts_, path );
			}			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::_loaded()
	{
		//Empty
	}	
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderResources_( BinParser * _parser, const WString & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Resource_Name, &ResourcePak::addResource_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderTexts_( BinParser * _parser, const WString & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Text )
			{
				ConstString Name;
				WString File;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Text_Name, Name);
					BIN_CASE_ATTRIBUTE( Protocol::Text_File, File);
				}
				
				this->addText_( Name, _path, File );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const ConstString & _name, const WString & _path )
	{
		ResourceDesc desc;
		desc.name = _name;
		desc.pakName = m_desc.name;
		desc.pakPath = m_desc.path;
		desc.pakType = m_desc.type;
		desc.path = _path;

		m_resourcesDesc.push_back( desc );

		//ResourceManager::get()
		//	->loadResource( m_desc.name, _name, path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addText_( const ConstString & _name, const WString & _path, const WString & _file )
	{
		ResourceDesc desc;
		desc.name = _name;
		desc.pakName = m_desc.name;
		desc.pakPath = m_desc.path;
		desc.pakType = m_desc.type;
		desc.path = _path;

		m_textsDesc.push_back( desc );

		//TextManager::get()
		//	->addResourceFile( _name, m_desc.name, filename );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScriptPath_( const WString & _path )
	{
		m_pathScripts.push_back( _path );
	}
}