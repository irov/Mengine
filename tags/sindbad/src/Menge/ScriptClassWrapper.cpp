#	include "ScriptClassWrapper.h"

#	include "ScriptEngine.h"

#	include "Node.h"

#	include <map>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::map< String, ScriptClassWrapper * > TMapClassWrapper;
	//////////////////////////////////////////////////////////////////////////
	static TMapClassWrapper & getMapWrapper()
	{
		static TMapClassWrapper m;
		return m;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassWrapperFactory::regWrapping( const String& _type, ScriptClassWrapper * _wrapper )
	{
		TMapClassWrapper & mapWrapper = getMapWrapper();

		TMapClassWrapper::iterator it_find = mapWrapper.find( _type );

		if( it_find == mapWrapper.end() )
		{
			mapWrapper.insert( std::make_pair( _type, _wrapper ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptClassWrapperFactory::wrap( const String& _type, Node * _node )
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
