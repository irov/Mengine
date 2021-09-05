#include "FETextFontEffectBase.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectBase::FETextFontEffectBase()
        : m_effectSample( 1 )
        , m_effectSampleInv( 1.f )
        , m_layoutCount( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectBase::~FETextFontEffectBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FETextFontEffectBase::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & FETextFontEffectBase::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    void FETextFontEffectBase::setEffectName( const ConstString & _effectName )
    {
        m_effectName = _effectName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FETextFontEffectBase::getEffectName() const
    {
        return m_effectName;
    }
    //////////////////////////////////////////////////////////////////////////
    void FETextFontEffectBase::setEffectSample( uint32_t _effectSample )
    {
        m_effectSample = _effectSample;

        m_effectSampleInv = 1.f / (float)m_effectSample;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FETextFontEffectBase::getEffectSample() const
    {
        return m_effectSample;
    }
    //////////////////////////////////////////////////////////////////////////
    float FETextFontEffectBase::getEffectSampleInv() const
    {
        return m_effectSampleInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FETextFontEffectBase::getLayoutCount() const
    {
        return m_layoutCount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectBase::compileFEBundle( fe_bundle * _bundle )
    {
        fe_effect * effect = fe_bundle_get_effect_by_name( _bundle, m_effectName.c_str() );

        MENGINE_ASSERTION_MEMORY_PANIC( effect );

        fe_node * effect_node_out = fe_effect_find_node_by_type( effect, fe_node_type_out );

        if( effect_node_out->properties_int[0] == 0 )
        {
            m_effectNodes[0] = effect_node_out;

            m_layoutCount = 1;

            return true;
        }

        uint32_t layoutCount = 0;

        for( int32_t i = 0; i != FE_MAX_PINS; ++i )
        {
            int32_t index = FE_MAX_PINS - i - 1;

            const fe_pin & pin = effect_node_out->in[index];

            const fe_node * pin_node = pin.node;

            if( pin_node == nullptr )
            {
                continue;
            }

            m_effectNodes[layoutCount] = pin_node;

            ++layoutCount;
        }

        m_layoutCount = layoutCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectBase::apply( uint32_t _width, uint32_t _rows, uint32_t _pitch, const void * _buffer, uint32_t _channel, int32_t _left, int32_t _top, uint32_t _height, const LambdaFontEffectProvider & _provider )
    {
        MENGINE_PROFILER_CATEGORY();

        int32_t im_image_w = _width;
        int32_t im_image_h = _rows;
        int32_t im_image_pitch = _pitch;
        const void * im_image_data = _buffer;

        FE_IMAGE_FORMAT im_image_format = TF_UNDEFINED;

        switch( _channel )
        {
        case 1:
            im_image_format = FE_IMG_A8;
            break;
        case 4:
            im_image_format = FE_IMG_R8G8B8A8;
            break;
        default:
            break;
        }

        int32_t im_x = _left;
        int32_t im_y = _top;

        fe_im bgra_res[FE_MAX_PINS];
        fe_image bgra_images[FE_MAX_PINS];
        for( uint32_t layoutIndex = 0; layoutIndex != m_layoutCount; ++layoutIndex )
        {
            const fe_node * effect_node_layout = m_effectNodes[layoutIndex];

            fe_im & res = bgra_res[layoutIndex];
            fe_image & bgra_image = bgra_images[layoutIndex];

            int32_t fe_height = (int32_t)(_height * m_effectSample);

            fe_node_apply( fe_height
                , im_x
                , im_y
                , im_image_w
                , im_image_h
                , im_image_format
                , im_image_pitch
                , im_image_data
                , effect_node_layout
                , &res );

#ifdef MENGINE_RENDER_TEXTURE_RGBA
            fe_image_create( &bgra_image, res.image.w, res.image.h, FE_IMG_R8G8B8A8 );
#else
            fe_image_create( &bgra_image, res.image.w, res.image.h, FE_IMG_B8G8R8A8 );
#endif

            fe_image_blit( &res.image, &bgra_image );

            fe_image_free( &res.image );
        }

        for( uint32_t layoutIndex = 0; layoutIndex != m_layoutCount; ++layoutIndex )
        {
            const fe_im & res = bgra_res[layoutIndex];
            const fe_image & bgra_image = bgra_images[layoutIndex];

            _provider( layoutIndex, res.x, -res.y, bgra_image.w, bgra_image.h, bgra_image.data, bgra_image.pitch, bgra_image.bytespp );
        }

        for( uint32_t layoutIndex = 0; layoutIndex != m_layoutCount; ++layoutIndex )
        {
            fe_image & bgra_image = bgra_images[layoutIndex];

            fe_image_free( &bgra_image );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectBase::prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        bool successful = this->_prefetch( _observer );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectBase::unfetch()
    {
        bool successful = this->_unfetch();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectBase::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        _observer->onPrefetchPreparation();
        _observer->onPrefetchComplete( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectBase::_unfetch()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
