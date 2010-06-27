#	include "ScriptEngine.h"

#	include "ScriptWrapper.h"

#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassWrapper.h"

#	include "XmlEngine.h"
#	include "Logger/Logger.h"

#	include "Entity.h"
#	include "Scene.h"
#	include "Arrow.h"
#	include "Game.h"

#	include "Consts.h"

#	include "osdefs.h"

#	include <iostream>
#	include <stdarg.h>

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		static T * extractNodeT( PyObject * _obj, const ConstString & _type )
		{
			T * node = pybind::extract_nt<T *>( _obj );

			if( node == 0 )
			{
				return 0;
			}

			node->setType( _type );
			node->setEmbedding( _obj );

			return node;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_global(0)
	{
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

		for( TMapEntitiesType::iterator
			it = m_mapEntitiesType.begin(),
			it_end = m_mapEntitiesType.end();
		it != it_end;
		++it )
		{
			pybind::decref( it->second );
		}

		pybind::finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptLogger::ScriptLogger()
		: m_softspace(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const String& _msg )
	{
		//MENGE_LOG_INFO( _msg.c_str() );
		Logger::get()->logMessage( _msg, LM_LOG );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::setSoftspace( int _softspace )
	{
		m_softspace = _softspace;
	}
	//////////////////////////////////////////////////////////////////////////
	int ScriptLogger::getSoftspace() const
	{
		return m_softspace;
	}
	//////////////////////////////////////////////////////////////////////////
	void ErrorScriptLogger::write( const String& _msg )
	{
		Logger::get()->logMessage( _msg, LM_ERROR );
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
	void ScriptEngine::initialize()
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
			.def_property("softspace", &ScriptLogger::getSoftspace, &ScriptLogger::setSoftspace )
			;

		pybind::class_<ErrorScriptLogger>("ErrorScriptLogger", true, py_menge )
			.def("write", &ErrorScriptLogger::write )
			.def_property("softspace", &ErrorScriptLogger::getSoftspace, &ErrorScriptLogger::setSoftspace )
			;

		PyObject * pyLogger = pybind::ptr(&m_loger);
		PyObject * pyErrorLogger = pybind::ptr(&m_errorLogger);

		pybind::setStdErrorHandle( pyErrorLogger );
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
	bool ScriptEngine::isEntityType( const ConstString& _type )
	{
		TMapEntitiesType::iterator it_find = 
			m_mapEntitiesType.find( _type );

		return it_find != m_mapEntitiesType.end();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getEntityPyType( const ConstString& _type )
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
	Blobject * ScriptEngine::getEntityXML( const ConstString& _type )
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
	bool ScriptEngine::registerEntityType( const ConstString& _packName, const String& _path, const ConstString& _type )
	{
		MENGE_LOG_INFO("register entity type '%s'"
			, _type.c_str() 
			);

		TMapEntitiesType::iterator it_found = m_mapEntitiesType.find( _type );

		if( it_found != m_mapEntitiesType.end() )
		{
			return false;
		}

		PyObject * py_module = this->importModule( _type, Consts::get()->c_builtin_empty );

		if( py_module == 0 )
		{
			MENGE_LOG_INFO("registerEntityType: failed importModule %s"
				, _type.c_str()
				);

			return false;
		}

		PyObject * py_entityType = 0;

		try
		{
			py_entityType = pybind::get_attr( py_module, _type.c_str() );

			if( py_entityType == 0 || pybind::check_type( py_entityType ) == false )
			{
				MENGE_LOG_INFO("registerEntityType: failed get from module %s attr %s"
					, _path.c_str()
					, _type.c_str()
					);

				return false;
			}

			MENGE_LOG_INFO("successful");
		}
		catch (...)
		{
			handleException();
			return false;
		}

		m_mapEntitiesType.insert( std::make_pair( _type, py_entityType ) );

		String xml_path = _path;
		xml_path += "/";
		xml_path += _type.str();
		xml_path += ".xml";

		FileInputInterface* file = FileEngine::get()
			->openInputFile( _packName, xml_path );
		 
		if( file == 0 )
		{
			MENGE_LOG_INFO("registerEntityType: failed open xml file %s"
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
		
		file->close();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const char * _name )
	{
		MENGE_LOG_INFO( "init module '%s'"
			, _name );

		try
		{
			PyObject * module = pybind::module_init( _name );
			return module;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;

	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importModule( const ConstString& _name, const ConstString& _type )
	{
		TMapModule::iterator it_find = m_mapModule.find( _name );

		if( it_find != m_mapModule.end() )
		{
			return it_find->second;
		}

		MENGE_LOG_INFO( "import module '%s'"
			, _name.c_str()
			);

		PyObject * module = 0;

		try
		{
			String module_path = _name.str();
			
			if( _type != Consts::get()->c_builtin_empty )
			{
				module_path += ".";
				module_path += _type.str();
			}
			
			module = pybind::module_import( module_path.c_str() );
		}
		catch( ... )
		{
			ScriptEngine::handleException();
		}

		if( module == 0 )
		{			
			MENGE_LOG_INFO( "invalid import module '%s'"
				, _name.c_str()
				);

			return 0;
		}

		m_mapModule.insert( std::make_pair( _name, module ) );

		return module;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setCurrentModule( PyObject * _module )
	{
		pybind::set_currentmodule( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity_( const ConstString& _type )
	{
		PyObject * py_entityType = this->getEntityPyType( _type );

		if( py_entityType == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		PyObject * py_entity = pybind::ask( py_entityType, "()" );

		if( py_entity == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity '%s' (invalid constructor)"
				, _type.c_str()
				);

			return 0;
		}

		Entity * entity = Helper::extractNodeT<Entity>( py_entity, Consts::get()->c_Entity );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity '%s' (invalid cast)"
				, _type.c_str() 
				);

			return 0;
		}

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const ConstString& _type )
	{
		Blobject * entityXml = 
			this->getEntityXML( _type );

		if( entityXml == 0 )
		{
			MENGE_LOG_ERROR( "Entity type '%s' not registered", _type.c_str() );
			return 0;
		}
	
		Entity * entity = this->createEntityFromXml_( _type, &entityXml->front(), entityXml->size() );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntityFromXml( const ConstString& _type, const String& _xml )
	{
		Entity * entity = this->createEntityFromXml_( _type, _xml.c_str(), _xml.size() );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "Can't create entity '%s' from xml [%s]"
				, _type.c_str() 
				, _xml.c_str()
				);

			return 0;
		}

		return entity;
	}	
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntityFromXml_( const ConstString& _type, const void * _buffer, std::size_t _size )
	{
		if( _size == 0 )
		{
			MENGE_LOG_ERROR( "Can't parse template entity '%s': xml is empty"
				, _type.c_str()
				);

			return 0;
		}

		Entity * entity = createEntity_( _type );

		if( entity == 0 )
		{
			return 0;
		}

		if( XmlEngine::get()
			->parseXmlBufferM( _buffer, _size, entity, &Entity::loader ) == false )
		{
			entity->destroy();

			return false;
		}

		entity->callMethod( ("onLoader"), "()" );

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////		
	Arrow * ScriptEngine::createArrow( const ConstString& _type )
	{
		PyObject * module = ScriptEngine::get()
			->importModule( _type, Consts::get()->c_Arrow );

		if( module == 0 )
		{
			return 0;
		}

		PyObject * result = pybind::ask_method( module, "Arrow", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Arrow * arrow = Helper::extractNodeT<Arrow>( result, Consts::get()->c_Arrow );

		return arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * ScriptEngine::createScene( const ConstString& _type )
	{
		PyObject * module = ScriptEngine::get()
			->importModule( _type, Consts::get()->c_Scene );

		if( module == 0 )
		{
			return 0;
		}

		PyObject * result = pybind::ask_method( module, "Scene", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Scene * scene = Helper::extractNodeT<Scene>( result, Consts::get()->c_Scene );

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const char * _name )
	{
		return pybind::has_attr( _module, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getModuleFunction( PyObject * _module, const char * _name )
	{
		return pybind::get_attr( _module, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( const ConstString& _module, const char * _name, const char * _params, ... )
	{
		TMapModule::iterator it_find = m_mapModule.find( _module );

		if( it_find == m_mapModule.end() )
		{
			return;
		}

		va_list valist;
		va_start(valist, _params);

		pybind::call_method_va( it_find->second, _name, _params, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( PyObject * _module, const char * _name, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		pybind::call_method_va( _module, _name, _params, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askModuleFunction( PyObject * _module, const char * _name, const char * _params, ... )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::ask_method_va( _module, _name, _params, valist );

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
	bool ScriptEngine::hasMethod( Node * _node, const char * _name )
	{
		PyObject * embedding = _node->getEmbedding();

		if( embedding == 0 )
		{
			return false;
		}

		pybind::decref( embedding );

		int res = pybind::has_attr( embedding, _name );

		return res == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callMethod( Node * _node, const char * _name, const char * _params, ...  )
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

			pybind::call_method_va( embedding, _name, _params, valist );

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
		try
		{
			return pybind::extract<bool>( _result );
		}
		catch( const pybind::pybind_exception & )
		{
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::writeError( const std::string & _message )
	{
		pybind::error_message( _message.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::proxy( PyObject * _module, const char * _name, void * _impl )
	{
		PyObject * result = pybind::ask_method( _module, _name, "()" );

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
		const ConstString& type = _node->getType();
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
