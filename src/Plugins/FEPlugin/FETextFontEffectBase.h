#pragma once

#include "Interface/TextFontEffectInterface.h"

#include "FEInterface.h"
#include "FEDataInterface.h"

#include "fe/fe.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FETextFontEffectBase
        : public TextFontEffectInterface
        , virtual public UnknownFEInterface
    {
    public:
        FETextFontEffectBase();
        ~FETextFontEffectBase() override;

    public:
        void setEffectName( const ConstString & _effectName ) override;
        const ConstString & getEffectName() const override;

    public:
        void setEffectSample( uint32_t _effectSample ) override;
        uint32_t getEffectSample() const override;
        float getEffectSampleInv() const override;

    protected:
        bool compileFEBundle( fe_bundle * _bundle );

    protected:
        bool apply( uint32_t _width, uint32_t _height, uint32_t _rows, uint32_t _pitch, const void * _buffer, uint32_t _channel, int32_t _left, int32_t _top, const LambdaFontEffectProvider & _provider ) override;

    public:
        bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch() override;

    protected:
        virtual bool _prefetch( const PrefetcherObserverInterfacePtr & _observer );
        virtual bool _unfetch();

    protected:
        ConstString m_effectName;
        uint32_t m_effectSample;
        float m_effectSampleInv;

        uint32_t m_ttfLayoutCount;

        const fe_node * m_ttfEffectNodes[FE_MAX_PINS] = {nullptr};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FETextFontEffectBase> FETextFontEffectBasePtr;
    //////////////////////////////////////////////////////////////////////////
}