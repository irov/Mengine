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
		bool compile() override;
		void release() override;

		bool isCompile() override;

	public:
		void setName(const std::string &name) override;
		const std::string & getName()const override;

		void setType(const std::string &type) override;
		const std::string & getType()const override;

		size_t getTypeId()const override;

		bool isExternal()const override;

		void setResource(const std::string &_file) override;
		const std::string & getResource()const override;

	public:
		void setParent(Node *node) override;
		Node * getParent() override;

		bool isRoot() override;

	public:
		Node * createChildren(const std::string &_name, const std::string &_type) override;

		bool addChildren(Node *_node) override;
	
		bool isChildren(Node *_node) override;
		bool isChildren(const std::string &_name) override;

		Node * getChildren(const std::string &_name)override;

		void foreachFunc(TForEachFunc _func) override;
		void foreachChildren( Visitor *_visitor) override;

		void removeChildren(Node *_node) override;
		void removeChildren(const std::string &_name) override;

	public:
		void update(float _timing) override;
		void loader(TiXmlElement *xml) override;
		void visit( Visitor * _visitor) override;

	public:
		void debugRender() override;

	public:
		luabind::adl::object * getScriptable() override;

	public:
		void _lostChildren(Node *_node, bool _valid) override;

	protected:
		Node *m_node;
	};
}