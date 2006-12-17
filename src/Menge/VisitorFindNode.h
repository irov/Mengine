#	pragma once

#	include "VisitorMask.h"


namespace Menge
{

	template<class T>
	class VisitorFindNode
		: public VisitorMask<T>
	{
	public:
		VisitorFindNode( const std::string &_name )
			: m_name(_name)
			, m_node(0)
		{

		}

	public:
		T * result()
		{
			return m_node;
		}

	protected:
		void mask_call( T *_node) override
		{
			const std::string &name = _node->getName();
			if( m_name == name )
			{
				m_node = _node;
			}
		}

	private:
		T * m_node;
		std::string m_name;
	};

	template<class T>
	T * findNode( Node * _were, const std::string & _name )
	{
		VisitorFindNode<T> v(_name);
		v.apply(_were);
		T *res = v.result();
		return res;
	}

}