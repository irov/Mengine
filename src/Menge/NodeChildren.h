#	pragma once

#	include "Node.h"
#	include "NodeForeach.h"

#	include <list>
#	include <algorithm>

namespace Menge
{	
	class NodeChildrenImpl
	{
	public:
		Node * createChildrenImpl( const std::string & _name );
	};

	template<class TNode >
	class NodeChildren
		: public virtual Node
		, public NodeChildrenImpl
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		NodeChildren()
			: m_parent( 0 )
		{}
		//////////////////////////////////////////////////////////////////////////
		void setParent( Node * _parent ) override
		{
			m_parent = dynamic_cast<TNode *>(_parent);
		}
		//////////////////////////////////////////////////////////////////////////
		TNode * getParent()
		{
			return m_parent;
		}
		//////////////////////////////////////////////////////////////////////////
		bool addChildren( Node * _node ) override
		{
			if( isChildren( _node ) )
			{
				return false;
			}

			TNode * t_node = dynamic_cast<TNode *>( _node );

			t_node->setParent( this );

			m_listChildren.push_back( t_node );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void removeChildren( Node *_node) override
		{
			TListChildren::iterator it_find = 
				std::find( m_listChildren.begin(), m_listChildren.end(), _node );

			if( it_find != m_listChildren.end() )
			{
				m_listChildren.erase( it_find );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		bool isChildren( Node *_node) override
		{
			TListChildren::iterator it_find = 
				std::find( m_listChildren.begin(), m_listChildren.end(), _node );

			return it_find != m_listChildren.end();
		}
		//////////////////////////////////////////////////////////////////////////
		void foreachChildren( const NodeForeach & _foreach ) override
		{
			for each( TNode * children in m_listChildren )
			{
				_foreach.apply( children );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TNode * getChildren( const std::string & _name )
		{
			for each( TNode * children in m_listChildren )
			{
				if( children->getName() == _name )
				{
					return children;
				}
			}

			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		T * getChildrenT( const std::string & _name )
		{
			return static_cast< T * >( getChildren( _name ) );
		}	
		///////////////////////////////////////////////////////////////////////
		TNode * createChildren( const std::string &_type )
		{
			Node * node = createChildrenImpl( _type );

			addChildren( node );
			
			return dynamic_cast<TNode*>(node);
		}
		//////////////////////////////////////////////////////////////////////////
		template< class T >
		T * createChildrenT( const std::string &_type )
		{
			return static_cast<T*>( createChildren( _type ) );
		}

	protected:
		TNode * m_parent;

		typedef std::list< TNode * > TListChildren;
		TListChildren m_listChildren;
	};
}