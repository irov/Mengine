#	pragma once

#	ifndef MENGINE_FACTORABLE_DEBUG_ENABLE
#	ifdef _DEBUG
#	define MENGINE_FACTORABLE_DEBUG
#	endif
#	else
#	define MENGINE_FACTORABLE_DEBUG
#	endif

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
		void setImmortal( bool _value );

	public:
		virtual void destroy();

	protected:
		virtual void _destroy();

#   ifdef MENGINE_FACTORABLE_DEBUG
	public:
		bool isDestroyed() const;

    protected:
        virtual void _checkDestroy();
#   endif

	protected:
		Factory * m_factory;

#   ifdef MENGINE_FACTORABLE_DEBUG
        bool m_destroy;
		bool m_immortal;
#   endif
	};
	//////////////////////////////////////////////////////////////////////////
	inline Factory * Factorable::getFactory() const
	{
		return m_factory;
	}
}
