#	pragma once

#	include "Node.h"

#	include "ObjectDeclare.h"

namespace Menge
{
	class NodeProxy
		: virtual public Node
	{
	public:
		NodeProxy( Node * _node);
		virtual ~NodeProxy();

		void destroy() override;

	public:
		bool activate() override;
		void deactivate() override;

		bool isActive() override;

	public:
		void setName(const std::string & _name) override;
		const std::string & getName()const override;

		void setType( const std::string & _type) override;
		const std::string & getType()const override;

	public:
		Scriptable * getScriptable() override;

	public:
		void setParent( Node * _node ) override;
		bool addChildren( Node * _node ) override;
		void removeChildren( Node * _node ) override;
		Node * getChildren( const std::string & _name, bool _recursion ) override;
		bool isChildren( Node * _node ) override;
		void foreachChildren( NodeForeach & _foreach ) override;

	public:
		void update( size_t _timing, const Viewport & _viewport ) override;
		void loader(TiXmlElement * _xml) override;

	public:
		void debugRender() override;

	protected:
		Node *m_node;
	};
}