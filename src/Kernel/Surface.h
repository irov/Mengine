#pragma once

#include "Kernel/Factorable.h"

#include "Kernel/Identity.h"
#include "Kernel/Visitable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Materialable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/UpdateContext.h"

#include "Kernel/Color.h"

#include "math/vec2.h"
#include "math/uv4.h"

#ifdef MENGINE_DEBUG
#include <type_traits>
#endif

namespace Mengine
{
    class Surface
        : public Factorable
        , public Identity
        , public Visitable
        , public Scriptable
        , public Materialable
        , public Compilable
        , public Animatable
        , public Eventable
        , public Unknowable
    {
        DECLARE_VISITABLE_BASE();

    public:
        Surface();
        ~Surface() override;

    public:
        virtual const mt::vec2f & getMaxSize() const = 0;
        virtual const mt::vec2f & getSize() const = 0;
        virtual const mt::vec2f & getOffset() const = 0;

        virtual uint32_t getUVCount() const = 0;
        virtual const mt::uv4f & getUV( uint32_t _index ) const = 0;

        virtual void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out ) const = 0;

        virtual const Color & getColor() const = 0;

    public:
        bool initialize();

    protected:
        virtual bool _initialize();

    public:
        void activate();
        void deactivate();

    public:
        virtual bool update( const UpdateContext * _context ) = 0;

    protected:
        virtual void _activate();
        virtual void _deactivate();
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        namespace Detail
        {
            template<class T>
            struct reinterpret_surface_cast_void_t
            {
                typedef void * type;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class U>
            struct reinterpret_surface_cast_void_t<const U *>
            {
                typedef const void * type;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T reinterpretSurfaceCast( void * _surface )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "reinterpret surface cast use on non 'Surfacable' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            try
            {
                if( dynamic_cast<typename Detail::reinterpret_surface_cast_void_t<T>::type>(static_cast<T>(_surface)) == nullptr )
                {
                    throw;
                }
            }
            catch( const std::exception & )
            {
                throw;
            }
#endif

            return reinterpret_cast<T>(_surface);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticSurfaceCast( Surface * _surface )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "static surface cast use on non 'Surfacable' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_surface) == nullptr )
            {
                throw;
            }
#endif

            return static_cast<T>(_surface);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticSurfaceCast( const Surface * _surface )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "static surface cast use on non 'Surfacable' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_surface) == nullptr )
            {
                throw;
            }
#endif

            return static_cast<T>(_surface);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicSurfaceCast( Surface * _surface )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "dynamic surface cast use on non 'Surfacable' type");
#endif

            T t = dynamic_cast<T>(_surface);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicSurfaceCast( const Surface * _surface )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "dynamic surface cast use on non 'Surfacable' type");
#endif

            T t = dynamic_cast<T>(_surface);

            return t;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Surface> SurfacePtr;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveSurfacePtr = IntrusivePtr<T, Surface>;
    //////////////////////////////////////////////////////////////////////////
}
