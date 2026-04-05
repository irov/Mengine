#include "MetalRenderVertexShader.h"

#include "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include "Kernel/Logger.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexShader::MetalRenderVertexShader()
        : m_function( nil )
        , m_library( nil )
        , m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexShader::~MetalRenderVertexShader()
    {
        MENGINE_ASSERTION_FATAL( m_function == nil, "vertex shader is not release" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory )
    {
        m_name = _name;
        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexShader::finalize()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexShader::compile()
    {
        if( m_compile == true )
        {
            return true;
        }

        LOGGER_INFO( "render", "compile vertex shader '%s'"
            , m_name.c_str()
        );

        MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        id<MTLDevice> device = extension->getMetalDevice();

        const Char * str_source = m_memory->getBuffer();
        NSUInteger str_size = (NSUInteger)m_memory->getSize();

        NSString * source = [[NSString alloc] initWithBytes:str_source length:str_size encoding:NSUTF8StringEncoding];

        NSError * error = nil;
        MTLCompileOptions * options = [[MTLCompileOptions alloc] init];

        id<MTLLibrary> library = [device newLibraryWithSource:source options:options error:&error];

        if( library == nil )
        {
            LOGGER_ERROR( "vertex shader '%s' compilation error: %s"
                , m_name.c_str()
                , [[error localizedDescription] UTF8String]
            );

            return false;
        }

        id<MTLFunction> function = [library newFunctionWithName:@"vertexShader"];

        if( function == nil )
        {
            LOGGER_ERROR( "vertex shader '%s' function 'vertexShader' not found"
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
    void MetalRenderVertexShader::release()
    {
        LOGGER_INFO( "render", "release vertex shader '%s'"
            , m_name.c_str()
        );

        m_function = nil;
        m_library = nil;
        m_compile = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexShader::isCompile() const
    {
        return m_compile;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLFunction> MetalRenderVertexShader::getFunction() const
    {
        return m_function;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexShader::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexShader::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
