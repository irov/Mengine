#	pragma once

namespace Menge
{
	//! Reference - интерфейс для ресурса.

	class Resource
	{
	public:
		//! Конструктор.
		/*!
		*
		*/
		Resource();

	public:
		//! Компиляция ресурса. Например загрузка картинки, эмиттеров и т.д.
		/*!
		* return если компиляция прошла успешно
		*/
		virtual bool compile();

		//! Освобождение ресурса.
		/*!
		*
		*/
		virtual void release();

		//! Проверка является ли ресурс скомпилированным.
		/*!
		* return если скомпилирован
		*/
		inline bool isCompile() const;

	public:
		bool recompile();

	protected:
		virtual bool _compile();
		virtual void _release();

	protected:
		bool m_compile;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCompile() const
	{
		return m_compile;
	}
}
