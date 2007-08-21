#	pragma once

#	include <string>

class TiXmlElement;

namespace Menge
{
	
	class NodeForeach;
	class Event;

	//pure-virtual interface
	class Node 
	{
	public:
		virtual ~Node(){};
		virtual void destroy() = 0;

	public:
		virtual void setName(const std::string &name) = 0;
		virtual const std::string & getName() const = 0;

		virtual void setType( const std::string & type ) = 0;
		virtual const std::string & getType() const = 0;

	public:
		virtual bool activate() = 0;
		virtual void deactivate() = 0;

		virtual bool isActive() = 0;

	public:
		virtual void setParent( Node * _node ) = 0;

		virtual bool addChildren( Node * _node ) = 0;
		virtual void removeChildren( Node * _node ) = 0;
		virtual bool isChildren( Node * _node ) = 0;

		virtual void foreachChildren( const NodeForeach & _foreach ) = 0;

	public:
		virtual void registerEvent( const std::string &_name, Event * _event  ) = 0;
		virtual Event * getEvent( const std::string &_name ) = 0;

	public:
		virtual void update( float _timing ) = 0;
		virtual void loader( TiXmlElement * xml ) = 0;

	public:
		virtual void debugRender() = 0;
	};
}
