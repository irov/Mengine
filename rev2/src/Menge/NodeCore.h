#	pragma once

#	include "Node.h"
#	include "NodeEventable.h"
#	include "NodeResource.h"

#	include "Scriptable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <algorithm>

#	include <map>

namespace Menge
{
	class NodeCore
		: public virtual Node
		, public NodeEventable
		, public NodeResource
		, public Scriptable
	{
	public:
		NodeCore();

		~NodeCore();

		void destroy() override;

	public:
		void render() override;
		bool isRenderable() override;
	
	public:
		void hide( bool _value );
		bool isHide() const;	
 
	protected:
		virtual void _render();
  
		virtual bool _renderBegin();
		virtual void _renderEnd();

	protected:
		bool m_hide;
	//
	public:
		bool activate() override;
		void deactivate() override;
		bool isActivate() override;

	public:
		void enable() override;
		void disable() override;
		bool isEnable() override;

	public:
		void update( float _timing ) override;
		bool isUpdatable() override;
		void setUpdatable( bool _updatable ) override;

	public:
		void setName( const std::string & _name ) override;
		const std::string & getName() const override;

		void setType( const std::string & _type ) override;
		const std::string & getType() const override;

	public:
		void loader( XmlElement *xml ) override;

	public:
		Scriptable * getScriptable() override;

	public:		
		void debugRender() override;

	public:
		void setParent( Node * _parent ) override;
		Node * getParent();

		bool addChildren( Node * _node ) override;
		void removeChildren( Node *_node) override;

		bool isChildren( Node *_node) override;
		void foreachChildren( NodeForeach & _foreach ) override;

		Node * getChildren( const std::string & _name, bool _recursion ) override;
		virtual void _addChildren( Node * _node );

	protected:
		virtual void _destroy();
		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual void _debugRender();

	protected:
		bool m_active;
		bool m_enable;
		bool m_updatable;

		std::string m_name;
		std::string m_type;

	protected:
		Node * m_parent;

		typedef std::list< Node * > TListChildren;
		TListChildren m_listChildren;
	};

}