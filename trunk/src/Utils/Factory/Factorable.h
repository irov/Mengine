#	pragma once

namespace Menge
{
	class Factory;

    class FactorableException
    {
    };

	class Factorable
	{
	public:
		Factorable();
		virtual ~Factorable();

	public:
		void setFactory( Factory * _factory );
		inline Factory * getFactory() const;

	public:
		virtual void destroy();

	protected:
		virtual void _destroy();

#   ifdef _DEBUG
    protected:
        virtual void _checkDestroy();
#   endif

	protected:
		Factory * m_factory;

#   ifdef _DEBUG
        bool m_destroy;
#   endif
	};
	//////////////////////////////////////////////////////////////////////////
	inline Factory * Factorable::getFactory() const
	{
		return m_factory;
	}
}
