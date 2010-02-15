/*
 *	Affector.h
 *
 *	Created by _Berserk_ on 2.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ValueInterpolator.h"

#	include "Scriptable.h"

namespace Menge
{
	class Affector
	{
	public:
		Affector( PyObject * _cb, int _type );
		virtual ~Affector();

	public:
		int getType() const;
		void setEndFlag( bool _endFlag );

	public:
		virtual bool affect( float _timing ) = 0;
		virtual void stop() = 0;

	protected:
		void call( Scriptable * _scriptable );

	protected:
		PyObject * m_cb;
		int m_type;
		bool m_endFlag;		
	};

	template<class C, class M>
	class MemeberAffector
		: public Affector
	{
	public:
		MemeberAffector( PyObject* _cb, int _type, C * _self, M _method )
			: Affector(_cb, _type)
			, m_self(_self)
			, m_method(_method)
		{
		}

	protected:
		C * m_self;
		M m_method;
	};

	template<class C, class M, class T, template<class> class Interpolator>
	class MemberAffectorInterpolate
		: public MemeberAffector<C,M>
	{
	public:
		MemberAffectorInterpolate( PyObject* _cb, int _type, C * _self, M _method )
			: MemeberAffector<C,M>(_cb, _type, _self, _method)
		{
		}

	protected:
		bool affect( float _timing ) override
		{
			bool finish = true;
			if( m_endFlag == true )
			{
				T value;
				finish = m_interpolator.update( _timing, &value );
				(m_self->*m_method)( value );
			}

			if( finish == false )
			{
				return false;
			}

			this->call( m_self );

			return true;
		}

		void stop() override
		{
			m_endFlag = false;
			m_interpolator.stop();
		}

	protected:
		Interpolator<T> m_interpolator;
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateLinear
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>
	{
	public:
		template<class ABS>
		MemberAffectorInterpolateLinear<C,M,T>( PyObject* _cb, int _type, C * _self, M _method
										, T _start, T _end, float _time, ABS _abs)
			: MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>(_cb, _type, _self, _method)
		{
			m_interpolator.start( _start, _end, _time, _abs );
		}
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateQuadratic
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>
	{
	public:
		template< typename ABS >
		MemberAffectorInterpolateQuadratic( PyObject* _cb, int _type
			, C * _self, M _method
			, T _start, T _end, T _v0
			, float _time
			, ABS _abs )
			: MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>( _cb, _type, _self, _method )
		{
			m_interpolator.start( _start, _end, _v0, _time, _abs );
		}
	};

	namespace NodeAffectorCreator
	{
		template<class C, class M, class T, class ABS>
		Affector* 
			newNodeAffectorInterpolateLinear( PyObject* _cb, int _type
											, C * _self, M _method
											, T _start, T _end, float _time, ABS _abs )
		{
			return new MemberAffectorInterpolateLinear<C, M, T>( _cb, _type
				, _self, _method
				, _start, _end, _time
				, _abs );
		}

		template<class C, class M, class T, class ABS>
		Affector*
			newNodeAffectorInterpolateQuadratic( PyObject* _cb, int _type
												, C * _self, M _method
												, T _start, T _end, T _v0, float _time, ABS _abs )
		{
			return new MemberAffectorInterpolateQuadratic<C, M, T>( _cb, _type
				, _self, _method
				, _start, _end, _v0, _time
				, _abs );
		}
	}
}	// namespace Menge
