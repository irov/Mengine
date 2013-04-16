#	pragma once

namespace Menge
{
	class Factory;

	class Factorable
	{
	public:
		Factorable();
		~Factorable();

	public:
		void setFactory( Factory * _factory );
		inline Factory * getFactory() const;

	public:
		void destroy();

	protected:
		virtual bool _destroy();

	protected:
		Factory * m_factory;
        bool m_destroy;
	};
	//////////////////////////////////////////////////////////////////////////
	inline Factory * Factorable::getFactory() const
	{
		return m_factory;
	}
}
