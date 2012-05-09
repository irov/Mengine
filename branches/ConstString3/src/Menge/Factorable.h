#	pragma once

namespace Menge
{
	class Factory;

	class Factorable
	{
	public:
		Factorable();
		virtual ~Factorable();

	public:
		void setFactory( Factory * _factory );
		inline Factory * getFactory() const;

	public:
		void create();
		void destroy();

	protected:
		virtual void _create();
		virtual void _destroy();

	protected:
		Factory * m_factory;
	};
	//////////////////////////////////////////////////////////////////////////
	inline Factory * Factorable::getFactory() const
	{
		return m_factory;
	}
}
