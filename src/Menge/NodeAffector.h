/*
 *	NodeAffector.h
 *
 *	Created by _Berserk_ on 2.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "pybind/pybind.hpp"
#	include "Node.h"

#	include "ValueInterpolator.h"

namespace Menge
{
	class Node;

	class NodeAffector
	{
	public:
		NodeAffector( PyObject* _endCallback, int _type );
		virtual ~NodeAffector();

		virtual bool affect( Node* _node, float _timing ) = 0;
		virtual void stop() = 0;

		int getType() const;
		void setEndFlag( bool _endFlag );

	protected:
		PyObject* m_endCallback;
		bool m_endFlag;
		int m_type;
	};

	template< typename T, typename F >
	class NodeAffectorInterpolateLinear
		: public NodeAffector
	{
	public:
		template< typename ABS >
		NodeAffectorInterpolateLinear( PyObject* _endCallback, int _type
										, T _start, T _end, float _time, ABS _abs
										, F _func )
			: NodeAffector( _endCallback, _type )
			, m_memberFunc( _func )
		{
			m_interpolator.start( _start, _end, _time, _abs );
		}

		bool affect( Node* _node, float _timing ) override
		{
			T value;
			bool finish = true;
			if( m_interpolator.isStarted() == true )
			{
				finish = m_interpolator.update( _timing, &value );
				(_node->*m_memberFunc)( value );
			}
			if( finish == true )
			{
				pybind::call( m_endCallback, "(Ob)", _node->getEmbedding(), m_endFlag );
				return true;
			}

			return false;
		}

		void stop() override
		{
			m_endFlag = false;
			m_interpolator.stop();
		}

	protected:
		ValueInterpolatorLinear<T> m_interpolator;
		F m_memberFunc;
	};

	template< typename T, typename F >
	class NodeAffectorInterpolateQuadratic
		: public NodeAffector
	{
	public:
		template< typename ABS >
		NodeAffectorInterpolateQuadratic( PyObject* _endCallback, int _type,
			T _start, 
			T _end, 
			T _v0,
			float _time, 
			ABS _abs, 
			F _func )
			: NodeAffector( _endCallback, _type )
			, m_memberFunc( _func )
		{
			m_interpolator.start( _start, _end, _v0, _time, _abs );
		}

		bool affect( Node* _node, float _timing ) override
		{
			T value;
			bool end = m_interpolator.update( _timing, &value );
			(_node->*m_memberFunc)( value );
			if( end == true )
			{
				pybind::call( m_endCallback, "(Ob)", _node->getEmbedding(), m_endFlag );
				return true;
			}

			return false;
		}

		void stop() override
		{
			m_endFlag = false;
			m_interpolator.stop();
		}

	protected:
		ValueInterpolatorQuadratic<T> m_interpolator;
		F m_memberFunc;
	};

	namespace NodeAffectorCreator
	{
		template< typename T, typename F, typename ABS >
		NodeAffector*
			newNodeAffectorInterpolateLinear( PyObject* _endCallback, int _type
											, T _start, T _end, float _time, ABS _abs
											, F _func )
		{
			return (NodeAffector*) new NodeAffectorInterpolateLinear< T, F >( _endCallback, _type, _start, _end, _time, _abs, _func );
		}

		template< typename T, typename F, typename ABS >
		NodeAffector*
			newNodeAffectorInterpolateQuadratic( PyObject* _endCallback, int _type,
			T _start, T _end, T _v0, float _time, ABS _abs, 
			F _func )
		{
			return (NodeAffector*) new NodeAffectorInterpolateQuadratic< T, F >( _endCallback, _type,
				_start, _end, _v0, _time, _abs, _func );
		}
	}
}	// namespace Menge
