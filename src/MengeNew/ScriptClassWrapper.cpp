#	include "ScriptClassWrapper.h"

#	include "ScriptEngine.h"

#	include "Node.h"

#	include <map>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::map< std::string, ScriptClassWrapper * > TMapClassWrapper;
	//////////////////////////////////////////////////////////////////////////
	static TMapClassWrapper & getMapWrapper()
	{
		static TMapClassWrapper m;
		return m;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassWrapperFactory::regWrapping( const std::string & _type, ScriptClassWrapper * _wrapper )
	{
		TMapClassWrapper & mapWrapper = getMapWrapper();

		TMapClassWrapper::iterator it_find = mapWrapper.find( _type );

		if( it_find == mapWrapper.end() )
		{
			mapWrapper.insert( std::make_pair( _type, _wrapper ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptClassWrapperFactory::wrap( const std::string & _type, Node * _node )
	{
		TMapClassWrapper & mapWrapper = getMapWrapper();

		TMapClassWrapper::iterator it_find = mapWrapper.find( _type );

		if( it_find != mapWrapper.end() )
		{
			//try
			//{
				PyObject * scriptable = it_find->second->wrap( _node );
				return scriptable;
			//}
			//catch (...)
			//{
				//ScriptEngine::handleException();
			//}			
		}

		return 0;
	}
}