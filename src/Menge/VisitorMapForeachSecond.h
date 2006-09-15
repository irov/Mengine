#	pragma once

#	include "VisitorBase.h"

#	include "Utility/map_algorithm.h"

namespace RvEngine
{
	namespace Visitor
	{
		template<class T, class M, class F>
		class FMapForeachSecond
			: public Base
		{
		public:
			FMapForeachSecond(M &map,F f)
				: m_function(f)
			{
				Utility::map_for_each_second(map,&Node::visit,this);
			}

		public:
			void execute(Node *obj)
			{
				obj->visitChildren(this);
			}

			void execute( T * obj)
			{
				(obj->*m_function)();
				
				obj->visitChildren(this);
			}

			F m_function;
		};


		template<
			class M, 
			class T_Result,
			class T_Class>
			void mapForeachSecond(M &m, T_Result (T_Class::*f)())
		{
			typedef T_Result (T_Class::*F)();

			FMapForeachSecond<T_Class,M,F>(m,f);	
		}

		template<
			class M, 
			class T_Result,
			class T_Class,
			class T_ClassArg>
			void mapForeachSecond(M &m, T_Result (T_Class::*f)(T_ClassArg))
		{
			typedef T_Result (T_Class::*F)(T_ClassArg);

			FMapForeachSecond<T_Class,M,F>(m,f);	
		}
	}
}