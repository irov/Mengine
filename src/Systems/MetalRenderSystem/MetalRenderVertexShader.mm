#include "MetalRenderVertexShader.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexShader::MetalRenderVertexShader()
        : m_function( nil )
        , m_library( nil )
        , m_precompiled( false )
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
    bool MetalRenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompiled )
    {
        m_name = _name;
        m_memory = _memory;
        m_precompiled = _precompiled;

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
        uint32_t referenceCount = m_compileReferenceCount.increfReferenceCount();

        if( referenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_function == nil, "vertex shader is already compile" );

            LOGGER_INFO( "render", "compile vertex shader '%s' precompiled [%u]"
                , m_name.c_str()
                , m_precompiled
            );

            if( m_device == nil )
            {
                LOGGER_ERROR( "vertex shader '%s' Metal device is nil"
                    , m_name.c_str()
                );

                return false;
            }

            const void * buffer = m_memory->getBuffer();
            NSUInteger bufferSize = (NSUInteger)m_memory->getSize();

            if( bufferSize == 0 )
            {
                LOGGER_ERROR( "vertex shader '%s' source buffer is empty (size 0)"
                    , m_name.c_str()
                );

                return false;
            }

            id<MTLLibrary> library = nil;

            if( m_precompiled == true )
            {
                NSError * error = nil;

                dispatch_data_t data = dispatch_data_create( buffer, bufferSize, nil, DISPATCH_DATA_DESTRUCTOR_DEFAULT );

                library = [m_device newLibraryWithData:data error:&error];

                if( library == nil )
                {
                    if( error != nil )
                    {
                        LOGGER_ERROR( "vertex shader '%s' precompiled library load error: %s"
                            , m_name.c_str()
                            , [[error localizedDescription] UTF8String]
                        );
                    }
                    else
                    {
                        LOGGER_ERROR( "vertex shader '%s' precompiled library load failed with no error (buffer size: %u)"
                            , m_name.c_str()
                            , (uint32_t)bufferSize
                        );
                    }

                    return false;
                }
            }
            else
            {
                const Char * str_source = (const Char *)buffer;

                NSString * source = [[NSString alloc] initWithBytes:str_source length:bufferSize encoding:NSUTF8StringEncoding];

                if( source == nil )
                {
                    LOGGER_ERROR( "vertex shader '%s' source is not valid UTF-8 (buffer may contain precompiled data, check 'Compile' attribute)"
                        , m_name.c_str()
                    );

                    return false;
                }

                NSError * error = nil;
                MTLCompileOptions * options = [[MTLCompileOptions alloc] init];

                library = [m_device newLibraryWithSource:source options:options error:&error];

                if( library == nil )
                {
                    if( error != nil )
                    {
                        LOGGER_ERROR( "vertex shader '%s' compilation error: %s"
                            , m_name.c_str()
                            , [[error localizedDescription] UTF8String]
                        );
                    }
                    else
                    {
                        LOGGER_ERROR( "vertex shader '%s' compilation failed with no error (source length: %u)"
                            , m_name.c_str()
                            , (uint32_t)[source length]
                        );
                    }

                    return false;
                }
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
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexShader::release()
    {
        uint32_t referenceCount = m_compileReferenceCount.decrefReferenceCount();

        if( referenceCount == 0 )
        {
            m_function = nil;
            m_library = nil;
        }
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
