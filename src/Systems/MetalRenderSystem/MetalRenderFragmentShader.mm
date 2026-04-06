#include "MetalRenderFragmentShader.h"

#include "Kernel/Logger.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderFragmentShader::MetalRenderFragmentShader()
        : m_function( nil )
        , m_library( nil )
        , m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderFragmentShader::~MetalRenderFragmentShader()
    {
        MENGINE_ASSERTION_FATAL( m_function == nil, "fragment shader is not release" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory )
    {
        m_name = _name;
        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderFragmentShader::finalize()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderFragmentShader::compile()
    {
        if( m_compile == true )
        {
            return true;
        }

        LOGGER_INFO( "render", "compile fragment shader '%s'"
            , m_name.c_str()
        );

        const Char * str_source = m_memory->getBuffer();
        NSUInteger str_size = (NSUInteger)m_memory->getSize();

        NSString * source = [[NSString alloc] initWithBytes:str_source length:str_size encoding:NSUTF8StringEncoding];

        NSError * error = nil;
        MTLCompileOptions * options = [[MTLCompileOptions alloc] init];

        id<MTLLibrary> library = [m_device newLibraryWithSource:source options:options error:&error];

        if( library == nil )
        {
            LOGGER_ERROR( "fragment shader '%s' compilation error: %s"
                , m_name.c_str()
                , [[error localizedDescription] UTF8String]
            );

            return false;
        }

        id<MTLFunction> function = [library newFunctionWithName:@"fragmentShader"];

        if( function == nil )
        {
            LOGGER_ERROR( "fragment shader '%s' function 'fragmentShader' not found"
                , m_name.c_str()
            );

            return false;
        }

        m_library = library;
        m_function = function;
        m_compile = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderFragmentShader::release()
    {
        LOGGER_INFO( "render", "release fragment shader '%s'"
            , m_name.c_str()
        );

        m_function = nil;
        m_library = nil;
        m_compile = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderFragmentShader::isCompile() const
    {
        return m_compile;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLFunction> MetalRenderFragmentShader::getFunction() const
    {
        return m_function;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderFragmentShader::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderFragmentShader::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
