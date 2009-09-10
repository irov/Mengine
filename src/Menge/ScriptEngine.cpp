#	include "ScriptEngine.h"

#	include "ScriptWrapper.h"

#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassWrapper.h"

#	include "XmlEngine.h"
#	include "LogEngine.h"

#	include "Entity.h"
#	include "Scene.h"
#	include "Arrow.h"
#	include "Game.h"


#	include "osdefs.h"

#	include <iostream>

#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_global(0)
	{
		Holder<ScriptEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
		for( TMapModule::iterator
			it = m_mapModule.begin(),
			it_end = m_mapModule.end();
		it != it_end;
		++it )
		{
			pybind::decref( it->second );
		}

		pybind::finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::genEvent( const std::string &_name )
	{
		try
		{
			return pybind::get_attr( m_global, _name.c_str() );
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const String& _msg )
	{
		//MENGE_LOG( _msg.c_str() );
		Holder<LogEngine>::hostage()->logMessage( _msg, LM_ERROR );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::exec( const String& _command )
	{
		pybind::exec( _command.c_str(), m_global, m_global );
		if( PyErr_Occurred() )
		{
			PyErr_Clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::init()
	{
//#	ifndef _DEBUG
		++Py_OptimizeFlag;
//#	endif
		++Py_NoSiteFlag;
		//Py_IgnoreEnvironmentFlag++;
		pybind::initialize();

		PyObject * main = initModule( "__main__" );
		m_global = pybind::module_dict( main );

		PyObject * py_menge = initModule( "Menge" );

		pybind::set_currentmodule( py_menge );

		//ScriptModuleDeclaration::init( py_menge );

		ScriptWrapper::nodeWrap();
		ScriptWrapper::helperWrap();
		ScriptWrapper::soundWrap();
		//ScriptWrapper::actorWrap();
		ScriptWrapper::entityWrap();

		pybind::class_<ScriptLogger>("ScriptLogger", true, py_menge )
			.def("write", &ScriptLogger::write )
			;

		PyObject * pyLogger = pybind::ptr(&m_loger);

		pybind::setStdErrorHandle( pyLogger );
		pybind::setStdOutHandle( pyLogger );

	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::incref( PyObject * _object )
	{
		pybind::incref( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::decref( PyObject * _object )
	{
		pybind::decref( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setModulePath( const TListModulePath & _listModulePath )
	{
		String path_packet;

		m_modulePaths = _listModulePath;

		for( TListModulePath::const_iterator
			it = m_modulePaths.begin(),
			it_end = m_modulePaths.end();
		it != it_end;
		++it)
		{
			path_packet += (*it);
			path_packet += DELIM;
		}

		pybind::set_syspath( path_packet.c_str() );

		pybind::check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addModulePath( const TListModulePath& _listPath )
	{
		m_modulePaths.insert( m_modulePaths.end(), _listPath.begin(), _listPath.end() );
		String path_packet;

		for( TListModulePath::const_iterator
			it = m_modulePaths.begin(),
			it_end = m_modulePaths.end();
		it != it_end;
		++it)
		{
			path_packet += (*it);
			path_packet += DELIM;
		}

		pybind::set_syspath( path_packet.c_str() );

		pybind::check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::isEntityType( const String& _type )
	{
		TMapEntitiesType::iterator it_find = 
			m_mapEntitiesType.find( _type );

		return it_find != m_mapEntitiesType.end();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getEntityModule( const String& _type )
	{
		TMapEntitiesType::iterator it_find = 
			m_mapEntitiesType.find( _type );

		if( it_find == m_mapEntitiesType.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	Blobject * ScriptEngine::getEntityXML( const String& _type )
	{
		TMapEntitiesXML::iterator it_find = 
			m_mapEntitiesXML.find( _type );

		if( it_find == m_mapEntitiesXML.end() )
		{
			return 0;
		}

		return &it_find->second;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::registerEntityType( const String& _packName, const String& _path, const String& _type )
	{
		TEntityPackMap::iterator it_find = m_entityPackMap.find( _type );
		if( it_find != m_entityPackMap.end() )
		{
			return false;
		}
		else
		{
			m_entityPackMap.insert( std::make_pair( _type, _packName ) );
		}

		MENGE_LOG("register entity type \"%s\""
			, _type.c_str() );

		TMapEntitiesType::iterator it_found = m_mapEntitiesType.find( _type );

		if( it_found != m_mapEntitiesType.end() )
		{
			return false;
		}

		PyObject * module = this->importModule( _type );

		if( module == 0 )
		{
			MENGE_LOG("registerEntityType: failed importModule %s"
				, _type.c_str()
				);

			return false;
		}

		try
		{
			PyObject * result = pybind::get_attr( module, _type.c_str() );

			if( pybind::check_type( result ) == false )
			{
				MENGE_LOG("registerEntityType: failed get from module %s attr %s"
					, _type.c_str()
					, _type.c_str()
					);

				return false;
			}

			pybind::decref( result );

			MENGE_LOG("successful");
		}
		catch (...)
		{
			handleException();
			return false;
		}

		m_mapEntitiesType.insert( std::make_pair( _type, module ) );

		String xml_path = _path;
		xml_path += "/";
		xml_path += _type;
		xml_path += ".xml";

		FileInput* file = FileEngine::hostage()
								->openFileInput( _packName, xml_path );
		 
		if( file == 0 )
		{
			MENGE_LOG("registerEntityType: failed open xml file %s"
				, xml_path.c_str()
				);

			return false;
		}

		TMapEntitiesXML::iterator it_insert =
			m_mapEntitiesXML.insert( std::make_pair( _type, Blobject() ) ).first;

		std::streamsize size = file->size();
		Blobject & blob = it_insert->second;
		
		blob.resize( size );
		file->read( &blob[0], size );
		
		FileEngine::hostage()
			->closeFileInput( file );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const String& _name )
	{
		MENGE_LOG( "init module \"%s\""
			, _name.c_str() );

		try
		{
			PyObject * module = pybind::module_init( _name.c_str() );
			return module;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;

	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importModule( const String& _file )
	{
		MENGE_LOG( "import module \"%s\""
			, _file.c_str() );

		TMapModule::iterator it_find = m_mapModule.find( _file );

		if( it_find != m_mapModule.end() )
		{
			return it_find->second;
		}

		try
		{
			PyObject * module = pybind::module_import( _file.c_str() );

			if( module == 0 )
			{				
				return 0;
			}

			m_mapModule.insert( std::make_pair( _file, module ) );

			return module;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setCurrentModule( PyObject * _module )
	{
		pybind::set_currentmodule( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity_( const String& _type )
	{
		PyObject * module = this->getEntityModule( _type );

		if( module == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity \"%s\""
				, _type.c_str() );
		}

		PyObject * result = pybind::ask_method( module, _type.c_str(), "()" );

		if( result == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity \"%s\" (invalid constructor)"
				, _type.c_str() );
			return 0;
		}

		Entity * entity = pybind::extract_nt<Entity*>( result );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity \"%s\" (invalid cast)"
				, _type.c_str() );
			return 0;
		}

		entity->setName( _type );
		entity->setType( "Entity" );
		entity->setEmbedding( result );

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const String& _type )
	{
		Entity * entity = createEntity_( _type );

		Blobject * entityXml = 
			this->getEntityXML( _type );

		if( entityXml )
		{
			if( Holder<XmlEngine>::hostage()
				->parseXmlBufferM( *entityXml, entity, &Entity::loader ) )
			{
				//entity->registerEvent( "LOADER", "onLoader" );
				//entity->callEvent("LOADER", "()");
				entity->callMethod( ("onLoader"), "()" );
			}
		}

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntityWithXml( const String& _type, const String& _xml )
	{
		Entity * entity = createEntity_( _type );

		if( Holder<XmlEngine>::hostage()
			->parseXmlBufferM( _xml, entity, &Entity::loader ) )
		{
			entity->callMethod( ("onLoader"), "()" );
		}

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////		
	Arrow * ScriptEngine::createArrow( const String& _type )
	{
		PyObject * module = Holder<ScriptEngine>::hostage()
			->importModule( _type );

		if( module == 0 )
		{
			return 0;
		}

		PyObject * result = pybind::ask_method( module, "Arrow", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Arrow * arrow = pybind::extract_nt<Arrow*>( result );

		arrow->setType( "Arrow" );
		arrow->setEmbedding( result );

		return arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * ScriptEngine::createScene( const String& _type )
	{
		PyObject * module = Holder<ScriptEngine>::hostage()
			->importModule( _type );

		if( module == 0 )
		{
			return 0;
		}

		PyObject * result = pybind::ask_method( module, "Scene", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Scene * scene = pybind::extract_nt<Scene*>( result );

		scene->setEmbedding( result );
		scene->setType( "Scene" );

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const String & _name )
	{
		return pybind::has_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getModuleFunction( PyObject * _module, const String & _name )
	{
		return pybind::get_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( const String& _module, const String & _name, const char * _params, ... )
	{
		TMapModule::iterator it_find = m_mapModule.find( _module );

		if( it_find == m_mapModule.end() )
		{
			return;
		}

		va_list valist;
		va_start(valist, _params);

		pybind::call_method_va( it_find->second, _name.c_str(), _params, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( PyObject * _module, const String & _name, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		pybind::call_method_va( _module, _name.c_str(), _params, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askModuleFunction( PyObject * _module, const String & _name, const char * _params, ... )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::ask_method_va( _module, _name.c_str(), _params, valist );

		va_end( valist );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( PyObject * _object, const char * _params, va_list _valist )
	{
		pybind::call_va( _object, _params, _valist );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askFunction( PyObject * _object, const char * _params, va_list _valist )
	{
		PyObject * result = pybind::ask_va( _object, _params, _valist );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askFunction( PyObject * _object, const char * _params, ... )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = askFunction( _object, _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( PyObject * _object, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		pybind::call_va( _object, _params, valist );

		va_end( valist ); 
	}	
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasMethod( Node * _node, const String & _name )
	{
		PyObject * embedding = _node->getEmbedding();

		if( embedding == 0 )
		{
			return false;
		}

		pybind::decref( embedding );

		int res = pybind::has_attr( embedding, _name.c_str() );

		return res == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callMethod( Node * _node, const String & _name, const char * _params, ...  )
	{
		PyObject * embedding = _node->getEmbedding();

		if( embedding == 0 )
		{
			return;
		}

		pybind::decref( embedding );

		try
		{
			va_list valist;
			va_start(valist, _params);

			pybind::call_method_va( embedding, _name.c_str(), _params, valist );

			va_end( valist ); 
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::parseBool( PyObject * _result )
	{
		return pybind::extract_nt<bool>( _result );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::proxy( PyObject * _module, const String & _name, void * _impl )
	{
		PyObject * result = pybind::ask_method( _module, _name.c_str(), "()" );

		if( result == 0 )
		{
			pybind::check_error();
			return 0;
		}

		pybind::class_core::wrap_holder( result, _impl );

		return result;
	}	
	//////////////////////////////////////////////////////////////////////////|
	PyObject * ScriptEngine::wrap( Node * _node )
	{
		const String& type = _node->getType();
		PyObject * pyNode = ScriptClassWrapperFactory::wrap( type, _node );
		return pyNode;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}
	//////////////////////////////////////////////////////////////////////////
}
