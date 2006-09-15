#	pragma once

#	include "ObjectDeclare.h"

#	include <list>

namespace RvEngine
{
	class SceneManager;
	class ScriptEngine;

	class Node
	{
		OBJECT_DECLARE(Node);

	public:
		typedef std::list<Node *> TListChildren;

	public:
		Node();
		virtual ~Node();

	public:
		virtual bool activate();
		virtual void deactivate();

		virtual bool isActive();
		
	public:
		virtual bool compile();
		virtual void release();

		virtual bool isCompile();

	public:
		virtual void setName(const std::string &name);
		virtual const std::string & getName()const;

		virtual void setType(const std::string &type);
		virtual const std::string & getType()const;

		virtual size_t getTypeId()const;

	public:		
		virtual void setSceneManager(SceneManager *sceneManager);
		virtual SceneManager * getSceneManager();

	public:
		virtual void setParent(Node *node);
		virtual Node * getParent();

		virtual void updateParent();
		virtual bool isRoot();

	public:
		virtual bool addChildren(Node *_node);
		virtual void visitChildren(Visitor::Base *visitor);

		virtual bool isChildren(Node *_node);
		virtual bool isChildren(const std::string &_name);

		virtual Node * getChildren(const std::string &_name);

		template<class T>
		T * getChildrenT(const std::string &_name)
		{
			return static_cast<T*>(getChildren(_name));
		}

		virtual void removeChildren(Node *_node);
		virtual void removeChildren(const std::string &_name);

	public:
		virtual void update(float _timing);
		virtual void loader(TiXmlElement *xml);

	public:
		virtual void debugRender();
		virtual void _debugRender();

	public:
		virtual luabind::adl::object * getScriptable();

	protected:
		virtual bool _activate();
		virtual void _deactivate();

		virtual bool _compile();
		virtual void _release();		

	protected:
		bool m_active;
		bool m_compile;

		std::string m_name;
		std::string m_type;

		Node * m_parent;
		TListChildren m_listChildren;

		luabind::adl::object * m_scriptObject;	

		SceneManager *m_sceneManager;

	private:
		TListChildren::iterator _findChildren(const std::string &_name);

		friend class NodeProxy;
	};

}