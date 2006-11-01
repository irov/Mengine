// ***************************************************************
//  ScriptObjectHelper.cpp   version:  1.0   ·  date: 05/10/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************


#	include "ScriptableCaster.h"

#	include "SceneManager.h"

#	include "NodeFactory.h"

#	include "Node.h"
#	include "ScriptEngine.h"

#	include <vector>

namespace Menge
{
	namespace ScriptableCaster
	{
		//////////////////////////////////////////////////////////////////////////
		namespace NVectorTypeCast
		{
			class CVectorTypeCast
			{
				typedef std::vector<TFunTypeCast> TVectorFunctionTypeCast;

			public:
				static TVectorFunctionTypeCast & vectorFunctionTypeCast()
				{
					static TVectorFunctionTypeCast m_vectorFunctionTypeCast(256);
					return m_vectorFunctionTypeCast;
				}

			public:
				static luabind::adl::object * cast(size_t _id, Node * _node)
				{
					CLuaScript *luaScript = 
						Keeper<ScriptEngine>::hostage()
						->getLuaScript();

					return vectorFunctionTypeCast()[_id](_node,luaScript);
				}

				static void insert( size_t _id , TFunTypeCast _function)
				{
					size_t size = vectorFunctionTypeCast().size();
					if( size <= _id )
					{
						vectorFunctionTypeCast().resize(size*2);
					}
					vectorFunctionTypeCast()[_id] = _function;
				}
			};	
			//////////////////////////////////////////////////////////////////////////
			luabind::adl::object * cast(size_t _id, Node * _node)
			{
				return CVectorTypeCast::cast(_id,_node);
			}
			//////////////////////////////////////////////////////////////////////////
			void insert( size_t _id , TFunTypeCast _function)
			{
				CVectorTypeCast::insert(_id,_function);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		Auto::Auto(
			const std::string &_key, 
			TFunTypeCast _func)
		{
			registry(_key,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		void registry(const std::string &_key, TFunTypeCast _func)
		{
			size_t typeId = NodeFactory::getId(_key);

			NVectorTypeCast::insert(typeId,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		luabind::adl::object * cast(size_t _typeId,Node *_node)
		{
			return NVectorTypeCast::cast(_typeId,_node);
		}
		//////////////////////////////////////////////////////////////////////////
		luabind::adl::object * cast(const std::string & _key,Node *_node)
		{
			size_t typeId = NodeFactory::getId(_key);

			return cast(typeId,_node);
		}
		//////////////////////////////////////////////////////////////////////////
		luabind::adl::object * cast(Node *_node)
		{
			size_t typeId = _node->getTypeId();

			return cast(typeId,_node);
		}
	}
}