#	pragma once

#	include "FactorableDeclare.h"

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
		void destroy();

	protected:
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
