#include "MetalRenderFragmentShader.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderFragmentShader::MetalRenderFragmentShader()
        : m_function( nil )
        , m_library( nil )
        , m_precompiled( false )
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
    bool MetalRenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompiled )
    {
        m_name = _name;
        m_memory = _memory;
        m_precompiled = _precompiled;

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
        uint32_t referenceCount = m_compileReferenceCount.increfReferenceCount();

        if( referenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_function == nil, "fragment shader is already compile" );

            LOGGER_INFO( "render", "compile fragment shader '%s' precompiled [%u]"
                , m_name.c_str()
                , m_precompiled
            );

            const void * buffer = m_memory->getBuffer();
            NSUInteger bufferSize = (NSUInteger)m_memory->getSize();

            id<MTLLibrary> library = nil;

            if( m_precompiled == true )
            {
                NSError * error = nil;

                dispatch_data_t data = dispatch_data_create( buffer, bufferSize, nil, DISPATCH_DATA_DESTRUCTOR_DEFAULT );

                library = [m_device newLibraryWithData:data error:&error];

                if( library == nil )
                {
                    LOGGER_ERROR( "fragment shader '%s' precompiled library load error: %s"
                        , m_name.c_str()
                        , [[error localizedDescription] UTF8String]
                    );

                    return false;
                }
            }
            else
            {
                const Char * str_source = (const Char *)buffer;

                NSString * source = [[NSString alloc] initWithBytes:str_source length:bufferSize encoding:NSUTF8StringEncoding];

                if( source == nil )
                {
                    LOGGER_ERROR( "fragment shader '%s' source is not valid UTF-8 (buffer may contain precompiled data, check 'Compile' attribute)"
                        , m_name.c_str()
                    );

                    return false;
                }

                NSError * error = nil;
                MTLCompileOptions * options = [[MTLCompileOptions alloc] init];

                library = [m_device newLibraryWithSource:source options:options error:&error];

                if( library == nil )
                {
                    LOGGER_ERROR( "fragment shader '%s' compilation error: %s"
                        , m_name.c_str()
                        , [[error localizedDescription] UTF8String]
                    );

                    return false;
                }
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
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderFragmentShader::release()
    {
        uint32_t referenceCount = m_compileReferenceCount.decrefReferenceCount();

        if( referenceCount == 0 )
        {
            m_function = nil;
            m_library = nil;
        }
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
