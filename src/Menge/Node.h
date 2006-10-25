#	pragma once

#	include "ObjectDeclare.h"

#	include <list>

namespace Menge
{
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

		virtual bool isExternal()const;

		virtual void setResource(const std::string &_file);
		virtual const std::string & getResource()const;

	public:
		virtual void setParent(Node *node);
		virtual Node * getParent();

		virtual bool isRoot();

	public:
		virtual Node * createChildren(const std::string &_name, const std::string &_type);

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

		template<class T_Result,class T_Class>
		void dynamicForeach( T_Result (T_Class::*f)() )
		{
			for( TListChildren::iterator 
				it = m_listChildren.begin(),
				it_end = m_listChildren.end();
			it != it_end;
			++it)
			{
				T_Class *obj = dynamic_cast<T_Class*>(*it);

				if( obj )
				{
					(obj->*f)();
				}

				(*it)->dynamicForeach(f);
			}
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

		virtual void _updateParent();
		virtual void _lostChildren(Node *_node, bool _valid);

	protected:
		bool m_active;
		bool m_compile;

		std::string m_name;
		std::string m_type;

		bool m_external;
		std::string m_resource;

		Node * m_parent;
		TListChildren m_listChildren;

		luabind::adl::object * m_scriptObject;	

	private:
		TListChildren::iterator _findChildren(const std::string &_name);

		friend class NodeProxy;
	};

}