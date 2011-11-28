#	pragma once

#	include <typeinfo>

namespace pybind
{
	class adapter_interface
	{
	public:
		virtual ~adapter_interface()
		{
		};
	};

	template<class C, class B = adapter_interface>
	class class_adapter_interface
		: public B
	{
	protected:
		class_adapter_interface()
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	protected:
		const char * getClassName() const
		{
			return m_class_name;
		}

		const char * getScopeName() const
		{
			return m_scope_name;
		}

	protected:
		const char * m_class_name;
		const char * m_scope_name;
	};
}