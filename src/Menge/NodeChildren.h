#	pragma once

#	include "Node.h"
//#	include "NodeForeach.h"

#	include <list>
#	include <algorithm>

namespace Menge
{	
/*	namespace Helper
	{
		void errorMessageChildrenIncorrectType( Node * _parent, Node * _children );
		void errorMessageChildrenAlreadyExsist( Node * _parent, Node * _children );
		void errorMessageParentIncorrectType( Node * _current, Node * _parent );
	}

	template<class TNode >
	class NodeChildren
		: public virtual Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		NodeChildren()
			: m_parent( 0 )
		{}
		//////////////////////////////////////////////////////////////////////////
		void setParent( Node * _parent ) override
		{
			TNode * node_t = dynamic_cast<TNode *>(_parent);

			if( node_t == 0 )
			{
				Helper::errorMessageParentIncorrectType( this, _parent );
			}
		
			m_parent = node_t;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * getParent() override
		{
			return m_parent;
		}
		//////////////////////////////////////////////////////////////////////////
		bool addChildren( Node * _node ) override
		{
			if( isChildren( _node ) )
			{
				Helper::errorMessageChildrenAlreadyExsist( this, _node );
				return false;
			}

			TNode * t_node = dynamic_cast<TNode *>( _node );

			if( t_node == 0 )
			{
				Helper::errorMessageChildrenIncorrectType( this, _node );
				return false;
			}

			if( _node->getParent() )
			{
				_node->getParent()->removeChildren( _node );
			}

			t_node->setParent( this );

			m_listChildren.push_back( t_node );

			_addChildren( t_node );

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
		void foreachChildren( NodeForeach & _foreach ) override
		{

			for( TListChildren::iterator
				it = m_listChildren.begin(),
				it_end = m_listChildren.end();
			it != it_end;
			)
			{
				_foreach.apply( *it++ );
			}
			
		}
		//////////////////////////////////////////////////////////////////////////
		Node * getChildren( const std::string & _name, bool _recursion ) override
		{
			for( TListChildren::iterator
				it = m_listChildren.begin(),
				it_end = m_listChildren.end();
			it != it_end;
			++it)
			{
				TNode * children = *it;
				if( children->getName() == _name )
				{
					return children;
				}

				if( _recursion )
				{
					if( Node * result = children->getChildren( _name, _recursion ) )
					{
						return result;
					}
				}
			}

			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		T * getChildrenT( const std::string & _name, bool recursion = false )
		{
			return dynamic_cast< T * >( getChildren( _name, recursion ) );
		}	
		//////////////////////////////////////////////////////////////////////////
		virtual void _addChildren( TNode * _node )
		{
			//Empty
		}
		//////////////////////////////////////////////////////////////////////////

	protected:
		TNode * m_parent;

		typedef std::list< TNode * > TListChildren;
		TListChildren m_listChildren;
	};*/
}
