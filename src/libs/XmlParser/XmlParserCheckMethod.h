#	pragma once

#	include "Config/Typedef.h"

namespace XmlParserCheckMethod
{
	template<class F>
	struct check_method_param0
	{
		template<class T> struct check_method_param0_impl{ typedef void param0_type; };

		template<class T> struct remove_refrences{ typedef T type; };
		template<class T> struct remove_refrences<T &>{	typedef T type; };
		template<class T> struct remove_refrences<const T &>{ typedef T type; };

		template<class R, class C, class P0>
		struct check_method_param0_impl< R (C::*)( P0 ) >
		{
			typedef typename remove_refrences<P0>::type param0_type;
		};

		typedef check_method_param0_impl<F> result_impl;
		typedef typename result_impl::param0_type type;
	};

	template<class C, class F>
	static void check_member( C _self, F _method, const Menge::TChar * _value )
	{
		typedef typename check_method_param0<F>::type type;

		type var;
		XmlParserCast::attribute_value_cast( var, _value );
		(_self->*_method)(var);
	}
}