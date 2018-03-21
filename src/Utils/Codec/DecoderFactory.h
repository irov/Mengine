#   pragma once

#include "Interface/CodecInterface.h"
#include "Interface/StringizeInterface.h"

#include "Core/ServantBase.h"

#include "Factory/FactoryPool.h"

namespace Mengine
{     
    template<class T>
    class DecoderFactory
        : public ServantBase<DecoderFactoryInterface>
    {
    protected:
        bool initialize() override
        {
            m_factory = new FactoryPool<T, 8>();

            return true;
        }

    protected:
        DecoderInterfacePtr createDecoder() override
        {	
            T * decoder = m_factory->createObject();

            return decoder;
        }

	protected:
		void destroy() override
		{
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;

			delete this;
		}

    protected:
        FactoryPtr m_factory;
    };

    namespace Helper
    {
        template<class T>
        inline DecoderFactoryInterfacePtr registerDecoder( const Char * _type )
        {
            DecoderFactoryInterfacePtr decoder = new DecoderFactory<T>();

            if( decoder->initialize() == false )
            {
                return nullptr;
            }

            if( CODEC_SERVICE()
                ->registerDecoder( Helper::stringizeString( _type ), decoder ) == false )
            {
                return nullptr;
            }

            return decoder;
        }

        inline void unregisterDecoder( const Char * _type )
        {
            CODEC_SERVICE()
                ->unregisterDecoder( Helper::stringizeString( _type ) );
        }
    }
}
