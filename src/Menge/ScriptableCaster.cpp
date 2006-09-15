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

#	include "Utility/singleton.h"

#	include <vector>

namespace RvEngine
{
	namespace ScriptableCaster
	{
		//////////////////////////////////////////////////////////////////////////
		namespace NVectorTypeCast
		{
			class CVectorTypeCast
				: public Utility::CSingleton<CVectorTypeCast>
			{
			public:
				CVectorTypeCast()
				{
					m_vectorFunctionTypeCast.resize(256);
				}

			public:
				luabind::adl::object * cast(size_t _id, Node * _node)
				{
					SceneManager *sceneMgr = _node->getSceneManager();
					ScriptEngine *scriptEngine = sceneMgr->getScriptEngine();

					CLuaScript *luaScript = scriptEngine->getLuaScript();

					return m_vectorFunctionTypeCast[_id](_node,luaScript);
				}

				void insert( size_t _id , TFunTypeCast _function)
				{
					size_t size = m_vectorFunctionTypeCast.size();
					if( size <= _id )
					{
						m_vectorFunctionTypeCast.resize(size*2);
					}
					m_vectorFunctionTypeCast[_id] = _function;
				}

			private: 
				std::vector<TFunTypeCast> m_vectorFunctionTypeCast;
			};	
			//////////////////////////////////////////////////////////////////////////
			CVectorTypeCast * get()
			{
				return CVectorTypeCast::GetInstance();
			}
			//////////////////////////////////////////////////////////////////////////
			luabind::adl::object * cast(size_t _id, Node * _node)
			{
				return get()->cast(_id,_node);
			}
			//////////////////////////////////////////////////////////////////////////
			void insert( size_t _id , TFunTypeCast _function)
			{
				get()->insert(_id,_function);
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