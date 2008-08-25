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
#	include "Utils.h"


#	include "pybind/pybind.hpp"

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
		Holder<LogEngine>::hostage()->logMessage( _msg, false, false, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::init()
	{
		pybind::initialize();

		PyObject * main = initModule( MENGE_TEXT("__main__") );
		m_global = pybind::module_dict( main );

		PyObject * py_menge = initModule( MENGE_TEXT("Menge") );

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

		for( TListModulePath::const_iterator
			it = _listModulePath.begin(),
			it_end = _listModulePath.end();
		it != it_end;
		++it)
		{
			path_packet += (*it);
			path_packet += MENGE_TEXT(';');
		}

		StringA path_packet_A;
#	ifdef MENGE_UNICODE
		path_packet_A = Utils::WToA( path_packet );
#	else if
		path_packet_A = path_packet;
#	endif

		pybind::set_syspath( path_packet_A.c_str() );

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
	bool ScriptEngine::registerEntityType( const String& _type )
	{
		MENGE_LOG( MENGE_TEXT("register entity type %s ...")
			,_type.c_str() );

		PyObject * module = importModule( _type );

		if( module == 0 )
		{
			MENGE_LOG( MENGE_TEXT("failed") );
			return false;
		}

		StringA char_type_A;
#	ifdef MENGE_UNICODE
		char_type_A = Utils::WToA( _type );
#	else if
		char_type_A = _type;
#	endif

		try
		{
			PyObject * result = pybind::get_attr( module, char_type_A.c_str() );

			if( pybind::check_type( result ) == false )
			{
				MENGE_LOG( MENGE_TEXT("failed") );
				return false;
			}

			pybind::decref( result );

			MENGE_LOG( MENGE_TEXT("successful") );
		}
		catch (...)
		{
			handleException();
			return false;
		}

		m_mapEntitiesType.insert( std::make_pair( _type, module ) );

		String xml_path = Holder<Game>::hostage()
			->getPathEntities( _type );

		xml_path += MENGE_TEXT('/');
		xml_path += _type;
		xml_path += MENGE_TEXT(".xml");

		DataStreamInterface * file = 
			Holder<FileEngine>::hostage()
			->openFile( xml_path );
		 
		TMapEntitiesXML::iterator it_insert =
			m_mapEntitiesXML.insert( std::make_pair( _type, Blobject() ) ).first;

		std::streamsize size = file->size();
		Blobject & blob = it_insert->second;
		
		blob.resize( size );
		file->read( &blob[0], size );
		
		Holder<FileEngine>::hostage()
			->closeStream( file );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const String& _name )
	{
		MENGE_LOG( MENGE_TEXT("init module %s ...")
			,_name.c_str() 
			);

		StringA char_name_A;
#	ifdef MENGE_UNICODE
		char_name_A = Utils::WToA( _name );
#	else if
		char_name_A = _name;
#	endif


		try
		{
			PyObject * module = pybind::module_init( char_name_A.c_str() );
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
		MENGE_LOG( MENGE_TEXT("import module %s ...")
			,_file.c_str() );

		TMapModule::iterator it_find = m_mapModule.find( _file );

		StringA char_file_A;
#	ifdef MENGE_UNICODE
		char_file_A = Utils::WToA( _file );
#	else if
		char_file_A = _file;
#	endif

		if( it_find != m_mapModule.end() )
		{
			return it_find->second;
		}

		try
		{
			PyObject * module = pybind::module_import( char_file_A.c_str() );

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
			MENGE_LOG( MENGE_TEXT("Can't create entity [%s] (not module)\n")
				, _type.c_str() 
				);
		}

		StringA char_type_A;
#	ifdef MENGE_UNICODE
		char_type_A = Utils::WToA( _type );
#	else if
		char_type_A = _type;
#	endif

		PyObject * result = pybind::call_method( module, char_type_A.c_str(), "()" );

		if( result == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Can't create entity [%s] (invalid constructor)\n")
				, _type.c_str() 
				);

			return 0;
		}

		Entity * entity = pybind::extract<Entity*>( result );

		if( entity == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Can't create entity [%s] (invalid cast)\n")
				, _type.c_str() 
				);

			return 0;
		}

		entity->setName( _type );
		entity->setType( MENGE_TEXT("Entity") );
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

		String xml_path = Holder<Game>::hostage()
			->getPathEntities( _type );

		xml_path += MENGE_TEXT('/');
		xml_path += _type;
		xml_path += MENGE_TEXT(".xml");

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

		PyObject * result = pybind::call_method( module, "Arrow", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Arrow * arrow = pybind::extract<Arrow*>( result );

		arrow->setType( MENGE_TEXT("Arrow") );
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

		PyObject * result = pybind::call_method( module, "Scene", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Scene * scene = pybind::extract<Scene*>( result );

		scene->setEmbedding( result );
		scene->setType( MENGE_TEXT("Scene") );

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const StringA& _name )
	{
		return pybind::has_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getModuleFunction( PyObject * _module, const StringA & _name )
	{
		return pybind::get_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callModuleFunction( const String& _module, const StringA & _name, const char * _params, ... )
	{
		TMapModule::iterator it_find = m_mapModule.find( _module );

		if( it_find == m_mapModule.end() )
		{
			return 0;
		}

		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::call_method_va( it_find->second, _name.c_str(), _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callModuleFunction( PyObject * _module, const StringA & _name, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::call_method_va( _module, _name.c_str(), _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( PyObject * _object, const char * _params, va_list _valist )
	{
		PyObject * result = pybind::call_va( _object, _params, _valist );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( PyObject * _object, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::call_va( _object, _params, valist );

		va_end( valist ); 

		return result;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasMethod( Node * _node, const StringA & _name )
	{
		PyObject * script = _node->getEmbedding();

		if( script == 0 )
		{
			return false;
		}

		int res = pybind::has_attr( script, _name.c_str() );

		return res == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callMethod( Node * _node, const StringA & _name, const char * _params, ...  )
	{
		PyObject * script = _node->getEmbedding();

		if( script == 0 )
		{
			return false;
		}

		try
		{
			va_list valist;
			va_start(valist, _params);

			PyObject * result = pybind::call_method_va( script, _name.c_str(), _params, valist );

			va_end( valist ); 

			return result;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::parseBool( PyObject * _result )
	{
		return pybind::extract<bool>( _result );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::proxy( PyObject * _module, const StringA & _name, void * _impl )
	{
		PyObject * result = pybind::call_method( _module, _name.c_str(), "()" );
		if( result == 0 )
		{
			pybind::check_error();
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

}
