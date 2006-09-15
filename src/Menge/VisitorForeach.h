#	pragma once

#	include "VisitorBase.h"

#	include "Utility/algorithm.h"

namespace RvEngine
{
	namespace Visitor
	{
		template<class T, class C, class F>
		class FContainerForeach
			: public Base
		{
		public:
			FContainerForeach(C &c,F f)
				: m_function(f)
			{
				Utility::for_each(c,&Node::visit,this);
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

		template<class C, class T_Result,class T_Class>
		void foreach(C &c, T_Result (T_Class::*f)())
		{
			typedef T_Result (T_Class::*F)();

			FContainerForeach<T_Class,C,F>(c,f);
		}

		template<class C, class T_Result, class T_Class, class T_ClassArg>
		void foreach(C &c, T_Result (T_Class::*f)(T_ClassArg))
		{
			typedef T_Result (T_Class::*F)(T_ClassArg);

			FContainerForeach<T_Class,C,F>(c,f);	
		}
	}
}