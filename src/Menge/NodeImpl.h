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


		virtual bool isExternal()const;

		virtual void setResource(const std::string &_file);
		virtual const std::string & getResource()const;

	public:
		virtual void setParent(Node *node);
		virtual Node * getParent();

		virtual bool isRoot();

	public:
		virtual Node * createChildren(const std::string &_name, const std::string &_type);

		template<class T>
		T * createChildrenT(const std::string &_name, const std::string &_type)
		{
			return static_cast<T*>(createChildren(_name,_type));
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

		template<class F>
		void foreachChildren( F f )
		{
			std::for_each( 
				m_listChildren.begin(), 
				m_listChildren.end(), 
				f 
				);
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
		virtual void registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  );
		virtual const lua_boost::lua_functor * event( const std::string &_name );

	protected:

		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual bool _compile();
		virtual void _release();

		virtual void _debugRender();

		virtual void _updateParent();
		virtual void _lostChildren(Node *_node, bool _valid);

		virtual void _visit( Visitor * _visitor );

	protected:
		bool m_active;
		bool m_compile;

		bool m_childrenForeach;

		std::string m_name;
		std::string m_type;

		bool m_external;
		std::string m_resource;

		Node * m_parent;
		TListChildren m_listChildren;
		TListChildren::iterator m_iteratorChildren;

	private:
		TListChildren::iterator _findChildren(const std::string &_name);

		friend class NodeProxy;
	};

}