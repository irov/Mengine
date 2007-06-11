#	pragma once

#	include "Node.h"

namespace Menge
{
	class NodeProxy
		: public Node
	{
	public:
		NodeProxy( Node *_node);
		virtual ~NodeProxy();

	public:
		bool activate() override;
		void deactivate() override;

		bool isActive() override;

	public:
		void setName(const std::string &name) override;
		const std::string & getName()const override;

		void setType(const std::string &type) override;
		const std::string & getType()const override;

	public:
		void setParent(Node *node) override;
		Node * getParent() override;

		bool isRoot() override;

	public:
		bool addChildren(Node *_node) override;
		bool isChildren(Node *_node) override;
		void visitChildren( Visitor *_visitor) override;
		void removeChildren(Node *_node) override;

		Node * nextChildren() override;
		Node * beginChildren() override;

	public:
		void update(float _timing) override;
		void loader(TiXmlElement *xml) override;
		void visit( Visitor * _visitor) override;

	public:
		void debugRender() override;

	public:
		virtual void registerEvent( const std::string &_name, ScriptObject * _func  );
		virtual ScriptObject * event( const std::string &_name );

	public:
		void _lostChildren(Node *_node, bool _valid) override;

	protected:
		Node *m_node;
	};
}