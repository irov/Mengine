#	pragma once

#	include "Config/Typedef.h"
#	include "XmlParserCast.h"

namespace Menge
{
	namespace XmlParserCheckMethod
	{
		template<class T>
		struct type_wrap
		{
			typedef T type;
		};

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

		template<class F>
		struct check_func_param0
		{
			template<class T> struct check_func_param0_impl{ typedef void param0_type; };

			template<class T> struct remove_refrences{ typedef T type; };
			template<class T> struct remove_refrences<T &>{	typedef T type; };
			template<class T> struct remove_refrences<const T &>{ typedef T type; };

			template<class R, class P0>
			struct check_func_param0_impl< R (*)( P0 ) >
			{
				typedef typename remove_refrences<P0>::type param0_type;
			};

			typedef check_func_param0_impl<F> result_impl;
			typedef typename result_impl::param0_type type;
		};

		template<class C, class F>
		static void check_method( C * _self, F _method, const Menge::TChar * _value, XmlElement * _element )
		{
			typedef typename check_method_param0<F>::type type;

			type var;
			XmlParserCast::attribute_value_cast( var, _value, _element );
			(_self->*_method)(var);
		}

		template<class F>
		static void check_func( F _func, const Menge::TChar * _value, XmlElement * _element )
		{
			typedef typename check_func_param0<F>::type type;

			type var;
			XmlParserCast::attribute_value_cast( var, _value, _element );
			(*_func)(var);
		}

		template<class F, class A1>
		static void check_func_arg1( F _func, const Menge::TChar * _value, XmlElement * _element, A1 & _arg1 )
		{
			typedef typename check_func_param0<F>::type type;

			type var;
			XmlParserCast::attribute_value_cast( var, _value, _element );
			(*_func)(var, _arg1);
		}

		template<class C, class F, class A1>
		static void check_method_arg1( C * _self, F _method, const Menge::TChar * _value, XmlElement * _element, A1 & _arg1 )
		{
			typedef typename check_method_param0<F>::type type;

			type var;
			XmlParserCast::attribute_value_cast( var, _value, _element );
			(_self->*_method)(var, _arg1);
		}

		template<class C, class F1, class F2>
		static void check_method_if( C _self, F1 _method1, F2 _method2, const Menge::TChar * _value, XmlElement * _element )
		{
			bool var;
			XmlParserCast::attribute_value_cast( var, _value, _element );

			if( var )
			{
				(_self->*_method1)();
			}
			else
			{
				(_self->*_method2)();
			}
		}

		template<class C, class F, class W>
		static void check_method_t( C * _self, F _method, const Menge::TChar * _value, XmlElement * _element, W )
		{
			typedef typename check_method_param0<F>::type type;

			typename W::type var;
			XmlParserCast::attribute_value_cast( var, _value, _element );
			(_self->*_method)( static_cast<type>(var) );
		}

	}
}