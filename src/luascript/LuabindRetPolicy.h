// ***************************************************************
//  LuabindRetPolicy.h   version:  1.0   ·  date: 05/30/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	pragma once

#	include <luabind/object.hpp>

#	include <boost/function_types/function_type_parameter.hpp>
#	include <boost/function_types/function_type_arity.hpp>
#	include <boost/function_types/function_type_result.hpp>
#	include <boost/function_types/function_type_class.hpp>

namespace luabind_ex
{
	template<class F, F fn,class P,class R,class C>
		inline luabind::object return_t(C *c )
	{
		return P()((c->*fn)());
	}

	template<class F, F fn,class P,class R,class C,class A0 >
		inline luabind::object return_t(C *c, A0 a0 )
	{
		return P()((c->*fn)(a0));
	}

	template<class F, F fn,class P,class R,class C,class A0,class A1>
		inline luabind::object return_t(C *c, A0 a0 , A1 a1 )
	{
		return P()((c->*fn)(a0,a1));
	}

	template<class F, F fn,class P,class R,class C,class A0,class A1,class A2>
		inline luabind::object return_t(C *c, A0 a0 , A1 a1 , A2 a2)
	{
		return P()((c->*fn)(a0,a1,a2));
	}

	template<class F,class R,class C>
	struct funct_w0
	{
		typedef luabind::object (*type)(C *);

		template<R (C::*Fp)(),class P>
			type ret()
		{
			return return_t<F,Fp,P,R,C>;
		}
	};

	template<class F,class R,class C, class P0>
	struct funct_w1
	{
		typedef luabind::object (*type)(C *,P0);

		template<R (C::*Fp)(P0),class P>
			type ret()
		{
			return return_t<F,Fp,P,R,C,P0>;
		}
	};

	template<class F,class R, class C, class P0, class P1>
	struct funct_w2
	{
		typedef luabind::object (*type)(C *,P0,P1);

		template<R (C::*Fp)(P0,P1),class P>
			type ret()
		{
			return return_t<F,Fp,P,R,C,P0,P1>;
		}
	};

	template<class F,class R, class C, class P0, class P1, class P2>
	struct funct_w3
	{
		typedef luabind::object (*type)(C *,P0,P1,P2);

		template<R (C::*Fp)(P0,P1,P2),class P>
			type ret()
		{
			return return_t<F,Fp,P,R,C,P0,P1,P2>;
		}
	};

	template<class F,class R,class C, size_t I>
	struct funct_w_r_i{};

	template<class F,class R,class C>
	struct funct_w_r_i<F,R,C,0>
	{
		typedef funct_w0<F,R,C> type;
	};

	template<class F,class R,class C>
	struct funct_w_r_i<F,R,C,1>
	{
		typedef typename boost::function_type_parameter_c<F,0>::type arg0_t;

		typedef funct_w1<F,R,C,arg0_t> type;
	};

	template<class F,class R,class C>
	struct funct_w_r_i<F,R,C,2>
	{
		typedef typename boost::function_type_parameter_c<F,0>::type arg0_t;
		typedef typename boost::function_type_parameter_c<F,1>::type arg1_t;

		typedef funct_w2<F,R,C,arg0_t,arg1_t> type;
	};

	template<class F,class R,class C>
	struct funct_w_r_i<F,R,C,3>
	{
		typedef typename boost::function_type_parameter_c<F,0>::type arg0_t;
		typedef typename boost::function_type_parameter_c<F,1>::type arg1_t;
		typedef typename boost::function_type_parameter_c<F,2>::type arg2_t;

		typedef funct_w3<F,R,C,arg0_t,arg1_t,arg2_t> type;
	};

	template<class F>
	struct funct_w_r
	{
		typedef typename boost::function_type_class<F>::type class_t;
		typedef typename boost::function_type_result<F>::type result_t;

		typedef typename funct_w_r_i<F,result_t,class_t,boost::function_type_arity<F>::value>::type type;
	};

	template<class F>
		typename funct_w_r<F>::type ret_policy(F f)
	{
		return typename funct_w_r<F>::type();
	}
}