#   pragma once

#   include "Interface/CodecInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Core/ServantBase.h"

#	include "Factory/FactoryPool.h"

namespace Menge
{
    template<class T>
    class EncoderFactory
        : public ServantBase<EncoderFactoryInterface>
    {
    protected:
        bool initialize() override
        {
            m_factory = new FactoryPool<T, 8>();

            return true;
        }

    protected:
        EncoderInterfacePtr createEncoder() override
        {
            T * encoder = m_factory->createObject();
            
            return encoder;
        }

	protected:
		void destroy() override
		{
			delete this;
		}

    protected:
        FactoryPtr m_factory;
    };

    namespace Helper
    {
        template<class T>
        inline EncoderFactoryInterfacePtr registerEncoder( const Char * _type )
        {
            EncoderFactoryInterfacePtr encoder = new EncoderFactory<T>();

			if( encoder->initialize() == false )
			{
				return nullptr;
			}

            CODEC_SERVICE()
                ->registerEncoder( Helper::stringizeString( _type ), encoder );

            return encoder;
        }

        inline void unregisterEncoder( const Char * _type )
        {
            CODEC_SERVICE()
                ->unregisterEncoder( Helper::stringizeString( _type ) );
        }
    }
}