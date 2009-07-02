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
		Factory * getFactory() const;

	public:
		void destroy();

	public:
		virtual void _destroy();

	protected:
		Factory * m_factory;
	};
}
