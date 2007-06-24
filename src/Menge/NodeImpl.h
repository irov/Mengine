#	pragma once

#	include "Node.h"
#	include "Eventable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{
	class NodeImpl
		: public Node
		, public Eventable
	{
		OBJECT_DECLARE( NodeImpl )

	public:
		typedef std::list<Node *> TListChildren;

	public:
		NodeImpl();

		virtual ~NodeImpl();
		virtual void destroy();

	public:
		virtual bool activate();
		virtual void deactivate();

		virtual bool isActive();

	public:
		virtual void setName(const std::string & _name);
		virtual const std::string & getName()const;

		virtual void setType(const std::string & _type);
		virtual const std::string & getType()const;


	public:
		virtual void setParent(Node *node);
		virtual Node * getParent();

		template<class T>
		T * getParentT()
		{
			return static_cast<Scene*>(getParent());
		}

		virtual bool isRoot();

	public:
		virtual Node * createChildren( const std::string &_type );

		template<class T>
		T * createChildrenT( const std::string &_type )
		{
			return static_cast<T*>(createChildren(_type));
		}

		virtual bool addChildren(Node *_node);

		virtual bool isChildren(Node *_node);
		virtual bool isChildren(const std::string &_name);

		virtual Node * getChildren(const std::string &_name);

		template<class T>
		T * getChildrenT(const std::string &_name)
		{
			return static_cast<T*>(getChildren(_name));
		}

		virtual Node * nextChildren();
		virtual Node * beginChildren();

		virtual void visitChildren( Visitor *_visitor );

		virtual void removeChildren(Node *_node);
		virtual void removeChildren(const std::string &_name);

	public:
		virtual void update(float _timing);
		virtual void loader(TiXmlElement *xml);

	public:
		virtual void debugRender();

	public:
		virtual void registerEvent( const std::string &_name, ScriptObject * _func  );
		virtual ScriptObject * event( const std::string &_name );

		virtual	void setScriptable( ScriptObject * _scriptable );
		virtual ScriptObject * getScriptable();
		virtual bool isScriptable() const;

	protected:

		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual void _debugRender();

		virtual void _updateParent();
		virtual void _lostChildren(Node *_node, bool _valid);

		virtual void _visit( Visitor * _visitor );

	protected:
		bool m_active;

		std::string m_name;
		std::string m_type;

		Node * m_parent;
		TListChildren m_listChildren;
		TListChildren::iterator m_iteratorChildren;

		ScriptObject * m_scriptable;

	private:
		TListChildren::iterator _findChildren(const std::string &_name);

		friend class NodeProxy;
	};

}