#	include "ResourcePak.h"

#	include "ScriptEngine.h"
#	include "TextManager.h"
#	include "ResourceManager.h"

#	include "SceneManager.h"
#	include "ArrowManager.h"
#	include "EntityManager.h"

#	include "LoaderEngine.h"
#	include "Metacode.h"

#	include "Consts.h"
#	include "LogEngine.h"
#	include "Utils/Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourcePak::ResourcePak( const ConstString & _name, const ConstString & _type, const ConstString & _locale, const String & _platform, const WString & _filename, const WString & _path, bool _preload, const WString & _baseDir )
		: m_name(_name)
		, m_type(_type)
		, m_locale(_locale)
		, m_platform(_platform)
		, m_filename(_filename)
		, m_path(_path)
		, m_preload(_preload)
		, m_baseDir(_baseDir)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::preload() const
	{
		return m_preload;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getLocale() const
	{
		return m_locale;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourcePak::getPlatfrom() const
	{
		return m_platform;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourcePak::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::load()
	{		
		WString fullPakPath = m_baseDir;
		fullPakPath += m_path;
		
		if(  m_type == Consts::get()->c_zip )
		{
			//fullPakPath += L".zip";
		}
		else
		{
			fullPakPath += MENGE_FOLDER_DELIM;
		}		

		if( FileEngine::get()
			->mountFileSystem( m_name, fullPakPath, m_type, false ) == false )
		{
			MENGE_LOG_ERROR( "ResourcePak::load failed to mount pak '%S': '%S'"
				, m_name.c_str()
				, fullPakPath.c_str()
				);

			return false;
		}

		Metacode::Meta_Pak pak;

		bool exist = false;
		if( LoaderEngine::get()
			->load( m_name, m_filename, &pak, exist ) == false )
		{
			MENGE_LOG_ERROR( "ResourcePak::load Invalid resource file '%S %s' '%S'"
				, m_path.c_str()
				, m_name.c_str()
				, m_filename.c_str()
				);

			return false;
		}

		const Metacode::Meta_Pak::TVectorMeta_Scripts & includes_scripts = pak.get_IncludesScripts();

		for( Metacode::Meta_Pak::TVectorMeta_Scripts::const_iterator
			it = includes_scripts.begin(),
			it_end = includes_scripts.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Scripts & scripts = *it;

			scripts.method_Path( this, &ResourcePak::addScriptPath_ );
		}

		const Metacode::Meta_Pak::TVectorMeta_Resources & includes_resources = pak.get_IncludesResources();

		for( Metacode::Meta_Pak::TVectorMeta_Resources::const_iterator
			it = includes_resources.begin(),
			it_end = includes_resources.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Resources & meta_resources = *it;

			const Metacode::Meta_Pak::Meta_Resources::TVectorMeta_Resource & includes_resource = meta_resources.get_IncludesResource();

			for( Metacode::Meta_Pak::Meta_Resources::TVectorMeta_Resource::const_iterator
				it = includes_resource.begin(),
				it_end = includes_resource.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_Pak::Meta_Resources::Meta_Resource & meta_resource = *it;

				meta_resource.method_Path( this, &ResourcePak::addResource_ );
			}
		}

		const Metacode::Meta_Pak::TVectorMeta_Texts & includes_tests = pak.get_IncludesTexts();

		for( Metacode::Meta_Pak::TVectorMeta_Texts::const_iterator
			it = includes_tests.begin(),
			it_end = includes_tests.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Texts & meta_texts = *it;

			const Metacode::Meta_Pak::Meta_Texts::TVectorMeta_Text & includes_text = meta_texts.get_IncludesText();

			for( Metacode::Meta_Pak::Meta_Texts::TVectorMeta_Text::const_iterator
				it = includes_text.begin(),
				it_end = includes_text.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_Pak::Meta_Texts::Meta_Text & meta_text = *it;

                meta_text.method_Path( this, &ResourcePak::addText_ );
			}
		}

		TVectorWString listModulePath;

		WString scriptPakPath = m_baseDir + m_path;
		
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
	void ResourcePak::addResource_( const WString & _path )
	{
		ResourceDesc desc;
		desc.pakName = m_name;
		desc.pakPath = m_path;
		desc.pakType = m_type;
		desc.path = _path;

		m_resourcesDesc.push_back( desc );

		//ResourceManager::get()
		//	->loadResource( m_desc.name, _name, path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addText_( const WString & _path )
	{
		ResourceDesc desc;
		desc.pakName = m_name;
		desc.pakPath = m_path;
		desc.pakType = m_type;
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