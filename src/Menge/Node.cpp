#	include "Node.h"
#	include "ScriptEngine.h"
#	include "NodeForeach.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_listener(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = Holder<ScriptEngine>::hostage()
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setListener( PyObject * _listener )
	{
		m_listener = _listener;
		this->_setListener();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setListener()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		struct ForeachCompile
			: public NodeForeach
		{
			ForeachCompile()
				: result(true)
			{
			}

			void apply( Node * children ) override
			{
				result = children->compile();
			}

			bool isCompile() const
			{
				return result;
			}

			bool result;
		};

		ForeachCompile functor;
		foreachChildren( functor );
		
		if( functor.isCompile() == false )
		{
			return false;
		}

		//MENGE_LOG( "Compiling Resource %s", this->getName().c_str() );
		bool result = Resource::compile();

		if( result == false )
		{
			//MENGE_LOG("Error: compiled Node '%s' is failed"
			//	, getName().c_str()
			//	);
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
		struct ForeachRelease
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->release();
			}
		};

		foreachChildren( ForeachRelease() );

		deactivate();

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::recompile()
	{
		bool status = isActivate();

		release();
		compile();

		if( isCompile() == false )
		{
			return false;
		}

		if( status )
		{
			activate();
		}

		return true;
	}
}