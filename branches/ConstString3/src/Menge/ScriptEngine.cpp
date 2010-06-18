#	include "ScriptEngine.h"

#	include "ScriptWrapper.h"

#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassWrapper.h"

#	include "XmlEngine.h"
#	include "Logger/Logger.h"

#	include "Factory/FactoryIdentity.h"

#	include "Entity.h"
#	include "Scene.h"
#	include "Arrow.h"
#	include "Game.h"


#	include "osdefs.h"

#	include <iostream>
#	include <stdarg.h>

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		static T * extractNodeT( PyObject * _obj, const std::string & _type, FactoryIdentity * _factoryIdentity )
		{
			T * node = pybind::extract_nt<T *>( _obj );

			if( node == 0 )
			{
				return 0;
			}

			node->setFactoryIdentity( _factoryIdentity );

			node->setType( _type );
			node->setEmbedding( _obj );

			return node;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine( FactoryIdentity * _factoryIdentity )
		: m_global(0)
		, m_factoryIdentity(_factoryIdentity)
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
	ScriptLogger::ScriptLogger()
		: m_softspace(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const String& _msg )
	{
		//MENGE_LOG_INFO( _msg.c_str() );
		Logger::hostage()->logMessage( _msg, LM_LOG );
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
		Logger::hostage()->logMessage( _msg, LM_ERROR );
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
	bool ScriptEngine::registerEntityType( const ConstString& _packName, const ConstString& _path, const ConstString& _type )
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

		MENGE_LOG_INFO("register entity type '%s'"
			, _type.c_str() );

		TMapEntitiesType::iterator it_found = m_mapEntitiesType.find( _type );

		if( it_found != m_mapEntitiesType.end() )
		{
			return false;
		}

		PyObject * py_module = this->importModule( _type );

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

		String xml_path = _path.str();
		xml_path += "/";
		xml_path += _type;
		xml_path += ".xml";

		FileInputInterface* file = FileEngine::hostage()
								->openFileInput( _packName, xml_path );
		 
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
		
		FileEngine::hostage()
			->closeFileInput( file );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const ConstString& _name )
	{
		MENGE_LOG_INFO( "init module '%s'"
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
	PyObject * ScriptEngine::importModule( const ConstString& _file )
	{
		MENGE_LOG_INFO( "import module '%s'"
			, _file.c_str()
			);

		TMapModule::iterator it_find = m_mapModule.find( _file );

		if( it_find != m_mapModule.end() )
		{
			return it_find->second;
		}

		PyObject * module = 0;

		try
		{
			module = pybind::module_import( _file.c_str() );
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		if( module == 0 )
		{			
			MENGE_LOG_INFO( "invalid import module '%s'"
				, _file.c_str() 
				);

			return 0;
		}

		m_mapModule.insert( std::make_pair( _file, module ) );

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

		Entity * entity = Helper::extractNodeT<Entity>( py_entity, "Entity", m_factoryIdentity );

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
	Entity * ScriptEngine::createEntityFromXml( const ConstString& _type, const ConstString& _xml )
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

		if( XmlEngine::hostage()
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

		Arrow * arrow = Helper::extractNodeT<Arrow>( result, "Arrow", m_factoryIdentity );

		return arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * ScriptEngine::createScene( const ConstString& _type )
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

		Scene * scene = Helper::extractNodeT<Scene>( result, "Scene", m_factoryIdentity );

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const ConstString & _name )
	{
		return pybind::has_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getModuleFunction( PyObject * _module, const ConstString & _name )
	{
		return pybind::get_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( const ConstString& _module, const ConstString & _name, const char * _params, ... )
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
	void ScriptEngine::callModuleFunction( PyObject * _module, const ConstString & _name, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		pybind::call_method_va( _module, _name.c_str(), _params, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askModuleFunction( PyObject * _module, const ConstString & _name, const char * _params, ... )
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
	bool ScriptEngine::hasMethod( Node * _node, const ConstString & _name )
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
	void ScriptEngine::callMethod( Node * _node, const ConstString & _name, const char * _params, ...  )
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
	void ScriptEngine::writeError( const std::string & _message )
	{
		pybind::error_message( _message.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::proxy( PyObject * _module, const ConstString & _name, void * _impl )
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
