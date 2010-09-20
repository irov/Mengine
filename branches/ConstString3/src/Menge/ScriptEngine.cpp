#	include "ScriptEngine.h"

#	include "ScriptWrapper.h"

#	include "Logger/Logger.h"

#	include "Node.h"

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
		for( TMapCategoryPrototypies::iterator
			it_category = m_prototypies.begin(),
			it_category_end = m_prototypies.end();
		it_category != it_category_end;
		++it_category )
		{
			for( TMapPrototypies::iterator
				it = it_category->second.begin(),
				it_end = it_category->second.end();
			it != it_end;
			++it )
			{
				pybind::decref( it->second );
			}
		}

		pybind::finalize();
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

		PyObject * pyLogger = m_loger.embedding();
		pybind::setStdOutHandle( pyLogger );


		PyObject * pyErrorLogger = m_errorLogger.embedding();
		pybind::setStdErrorHandle( pyErrorLogger );		
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
	PyObject * ScriptEngine::importModule( const ConstString& _name )
	{
		TMapModules::iterator it_find = m_modules.find( _name );

		if( it_find != m_modules.end() )
		{
			return it_find->second;
		}

		PyObject * module = 0;

		try
		{
			module = pybind::module_import( _name.c_str() );
		}
		catch( ... )
		{
			ScriptEngine::handleException();
		}

		if( module == 0 )
		{			
			MENGE_LOG_WARNING( "ScriptEngine: invalid import module '%s':'%s'"
				, module_path.c_str()
				, _category.c_str()
				);

			return 0;
		}

		it_find_category->second.insert( std::make_pair( _name, module ) );

		return module;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyCodeObject *
		run_pyc_file(FILE *fp, const char *filename, PyObject *globals,
		PyObject *locals, PyCompilerFlags *flags)
	{
		PyCodeObject *co;
		PyObject *v;
		long magic;
		long PyImport_GetMagicNumber(void);

		magic = PyMarshal_ReadLongFromFile(fp);
		if (magic != PyImport_GetMagicNumber()) {
			PyErr_SetString(PyExc_RuntimeError,
				"Bad magic number in .pyc file");
			return NULL;
		}
		(void) PyMarshal_ReadLongFromFile(fp);
		v = PyMarshal_ReadLastObjectFromFile(fp);
		fclose(fp);
		if (v == NULL || !PyCode_Check(v)) {
			Py_XDECREF(v);
			PyErr_SetString(PyExc_RuntimeError,
				"Bad code object in .pyc file");
			return NULL;
		}
		co = (PyCodeObject *)v;
		v = PyEval_EvalCode(co, globals, locals);
		if (v && flags)
			flags->cf_flags |= (co->co_flags & PyCF_MASK);
		Py_DECREF(co);
		return v;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importPrototype( const ConstString& _name, const ConstString & _category, const ConstString & _path  )
	{
		TMapCategoryPrototypies::iterator it_find_category = m_prototypies.find( _category );

		if( it_find_category == m_prototypies.end() )
		{
			MENGE_LOG_WARNING( "ScriptEngine: import module '%s':'%s' - invalid category"
				, _name.c_str()
				, _category.c_str()
				);

			return 0;				 
		}

		TMapModules::iterator it_find = it_find_category->second.find( _name );

		if( it_find != it_find_category->second.end() )
		{
			return it_find->second;
		}

		MENGE_LOG_INFO( "ScriptEngine: import module '%s':'%s'"
			, _name.c_str()
			, _category.c_str()
			);

		PyObject* code = Py_CompileStringFlags(buff,path,Py_file_input,0);
		PyObject* mod = PyImport_ExecCodeModuleEx(name, code, path);


		PyObject * module = 0;

		String module_path = _name.str();
		module_path += ".";
		module_path += _name.str();

		try
		{			

			
			PyObject * py_module = pybind::module_import( module_path.c_str() );

			module = pybind::get_attr( py_module, _name.c_str() );
		}
		catch( ... )
		{
			ScriptEngine::handleException();
		}

		if( module == 0 )
		{			
			MENGE_LOG_WARNING( "ScriptEngine: invalid import module '%s':'%s'"
				, module_path.c_str()
				, _category.c_str()
				);

			return 0;
		}

		it_find_category->second.insert( std::make_pair( _name, module ) );

		return module;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setCurrentModule( PyObject * _module )
	{
		pybind::set_currentmodule( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	Node * ScriptEngine::createNode( const ConstString& _prototype, const ConstString& _type )
	{
		PyObject * module = ScriptEngine::get()
			->importPrototype( _prototype, _category );

		if( module == 0 )
		{
			MENGE_LOG_ERROR( "ScriptEngine: Can't create object '%s.%s' (not module)"
				, _type.c_str()
				, _category.c_str()
				);

			return 0;
		}

		PyObject * result = pybind::ask( module, "()" );

		if( result == 0 )
		{
			MENGE_LOG_ERROR( "ScriptEngine: Can't create object '%s.%s' (invalid cast)"
				, _type.c_str()
				, _category.c_str()
				);

			return 0;
		}

		Node * node = Helper::extractNodeT<Node>( result, _category );

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "ScriptEngine: Can't create node '%s.%s' (invalid cast)"
				, _type.c_str()
				, _category.c_str()
				);

			return 0;
		}

		node->setType( _category );

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::regWrapping( const ConstString& _type, ScriptClassInterface * _wrapper )
	{
		TMapScriptWrapper::iterator it_find = m_scriptWrapper.find( _type );

		if( it_find == m_scriptWrapper.end() )
		{
			m_scriptWrapper.insert( std::make_pair( _type, _wrapper ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////|
	PyObject * ScriptEngine::wrap( Node * _node )
	{
		const ConstString& type = _node->getType();

		TMapScriptWrapper::iterator it_find = m_scriptWrapper.find( type );

		if( it_find == m_scriptWrapper.end() )
		{
			return 0;
		}

		//try
		//{
		PyObject * embedded = it_find->second->wrap( _node );
		return embedded;
		//}
		//catch (...)
		//{
		//ScriptEngine::handleException();
		//}			

		return embedded;
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
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}
	//////////////////////////////////////////////////////////////////////////
}
