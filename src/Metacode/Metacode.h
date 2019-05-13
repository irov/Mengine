#pragma once

#include "Metatype.h"

#include "metabuf/Metadata.hpp"

namespace Metacode
{
    const size_t header_size = 20;

    typedef uint32_t enum_t;

    uint32_t get_metacode_magic();
    uint32_t get_metacode_version();
    uint32_t get_metacode_protocol_version();
    uint32_t get_metacode_protocol_crc32();

    const char * getHeaderErrorMessage( Metabuf::HeaderError _error );

    Metabuf::HeaderError readHeader( const void * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion, uint32_t & _readProtocol, uint32_t & _needProtocol, uint32_t _metaVersion, uint32_t & _readMetaVersion );

    bool readStrings( const void * _buff, size_t _size, size_t & _read, uint32_t & _stringCount );
    const char * readString( const void * _buff, size_t _size, size_t & _read, uint32_t & _stringSize, int64_t & _stringHash );

    namespace Meta_Data
    {
        uint32_t getVersion();
    
        class Meta_DataBlock
            : public Metabuf::Metaparse
        { 
        public:
            Meta_DataBlock();
            ~Meta_DataBlock();
        
        public:
            template<class C, class M>
            void getm_Name( C _self, M _method ) const
            {
                (_self->*_method)( this->m_Name );
            }
            
            const Mengine::ConstString & get_Name() const
            {
                return this->m_Name;
            }
            
        public:
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
        
        protected:
            void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            void _parseGenerators( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_FragmentShader
                : public Metabuf::Metadata
            { 
            public:
                Meta_FragmentShader();
            
            public:
                template<class C, class M>
                void getm_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Name );
                }
                
                const Mengine::ConstString & get_Name() const
                {
                    return this->m_Name;
                }
                
                template<class C, class M>
                void getm_RenderPlatform( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_RenderPlatform );
                }
                
                const Mengine::ConstString & get_RenderPlatform() const
                {
                    return this->m_RenderPlatform;
                }
                
                bool has_File_Compile() const
                {
                    return m_File_Compile_successful;
                }
                
                template<class C, class M>
                bool getm_File_Compile( C _self, M _method ) const
                {
                    if( m_File_Compile_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Compile );
                
                    return true;
                }
                
                bool get_File_Compile( bool * _value ) const
                {
                    if( m_File_Compile_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Compile;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                bool m_File_Compile_successful;
                bool m_File_Compile;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_Include
                : public Metabuf::Metadata
            { 
            public:
                Meta_Include();
            
            public:
                template<class C, class M>
                void getm_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Path );
                }
                
                const Mengine::FilePath & get_Path() const
                {
                    return this->m_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_Path;
            };
            
            class Meta_Material
                : public Metabuf::Metadata
            { 
            public:
                Meta_Material();
            
            public:
                bool has_Debug() const
                {
                    return m_Debug_successful;
                }
                
                bool getd_Debug( bool _default ) const
                {
                    if( m_Debug_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Debug;
                }
                
                bool get_Debug( bool * _value ) const
                {
                    if( m_Debug_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Debug;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Debug( C _self, M _method ) const
                {
                    if( m_Debug_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Debug );
                
                    return true;
                }
                
                bool getd_Debug( bool * _value, const bool & _default ) const
                {
                    if( m_Debug_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Debug;
                
                    return true;
                }
                template<class C, class M>
                void getm_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Name );
                }
                
                const Mengine::ConstString & get_Name() const
                {
                    return this->m_Name;
                }
                
                bool has_RenderPlatform() const
                {
                    return m_RenderPlatform_successful;
                }
                
                template<class C, class M>
                bool getm_RenderPlatform( C _self, M _method ) const
                {
                    if( m_RenderPlatform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_RenderPlatform );
                
                    return true;
                }
                
                bool get_RenderPlatform( Mengine::ConstString * _value ) const
                {
                    if( m_RenderPlatform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                
                bool getd_RenderPlatform( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( m_RenderPlatform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                bool has_AlphaBlend_Enable() const
                {
                    return m_AlphaBlend_Enable_successful;
                }
                
                template<class C, class M>
                void getm_AlphaBlend_Enable( C _self, M _method ) const
                {
                    if( m_AlphaBlend_Enable_successful == false )
                    {
                        return false;
                    (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_AlphaBlend_Enable );
                    }
                }
                
                bool get_AlphaBlend_Enable() const
                {
                    if( m_AlphaBlend_Enable_successful == false )
                    {
                        return false;
                    }
                
                    return this->m_AlphaBlend_Enable;
                }
                
                bool has_BlendFactor_Dest() const
                {
                    return m_BlendFactor_Dest_successful;
                }
                
                template<class C, class M>
                bool getm_BlendFactor_Dest( C _self, M _method ) const
                {
                    if( m_BlendFactor_Dest_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_BlendFactor_Dest );
                
                    return true;
                }
                
                bool get_BlendFactor_Dest( Mengine::EBlendFactor * _value ) const
                {
                    if( m_BlendFactor_Dest_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_BlendFactor_Dest;
                
                    return true;
                }
                
                bool has_BlendFactor_Op() const
                {
                    return m_BlendFactor_Op_successful;
                }
                
                template<class C, class M>
                bool getm_BlendFactor_Op( C _self, M _method ) const
                {
                    if( m_BlendFactor_Op_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_BlendFactor_Op );
                
                    return true;
                }
                
                bool get_BlendFactor_Op( Mengine::EBlendOp * _value ) const
                {
                    if( m_BlendFactor_Op_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_BlendFactor_Op;
                
                    return true;
                }
                
                bool has_BlendFactor_Source() const
                {
                    return m_BlendFactor_Source_successful;
                }
                
                template<class C, class M>
                bool getm_BlendFactor_Source( C _self, M _method ) const
                {
                    if( m_BlendFactor_Source_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_BlendFactor_Source );
                
                    return true;
                }
                
                bool get_BlendFactor_Source( Mengine::EBlendFactor * _value ) const
                {
                    if( m_BlendFactor_Source_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_BlendFactor_Source;
                
                    return true;
                }
                
                bool has_DepthBufferTest_Enable() const
                {
                    return m_DepthBufferTest_Enable_successful;
                }
                
                template<class C, class M>
                void getm_DepthBufferTest_Enable( C _self, M _method ) const
                {
                    if( m_DepthBufferTest_Enable_successful == false )
                    {
                        return false;
                    (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_DepthBufferTest_Enable );
                    }
                }
                
                bool get_DepthBufferTest_Enable() const
                {
                    if( m_DepthBufferTest_Enable_successful == false )
                    {
                        return false;
                    }
                
                    return this->m_DepthBufferTest_Enable;
                }
                
                bool has_DepthBufferWrite_Enable() const
                {
                    return m_DepthBufferWrite_Enable_successful;
                }
                
                template<class C, class M>
                void getm_DepthBufferWrite_Enable( C _self, M _method ) const
                {
                    if( m_DepthBufferWrite_Enable_successful == false )
                    {
                        return false;
                    (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_DepthBufferWrite_Enable );
                    }
                }
                
                bool get_DepthBufferWrite_Enable() const
                {
                    if( m_DepthBufferWrite_Enable_successful == false )
                    {
                        return false;
                    }
                
                    return this->m_DepthBufferWrite_Enable;
                }
                
                bool has_Program_Name() const
                {
                    return m_Program_Name_successful;
                }
                
                template<class C, class M>
                bool getm_Program_Name( C _self, M _method ) const
                {
                    if( m_Program_Name_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Program_Name );
                
                    return true;
                }
                
                bool get_Program_Name( Mengine::ConstString * _value ) const
                {
                    if( m_Program_Name_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Program_Name;
                
                    return true;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_TextureStages
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_TextureStages();
                
                public:
                    template<class C, class M>
                    void getm_Stage( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Stage );
                    }
                    
                    uint32_t get_Stage() const
                    {
                        return this->m_Stage;
                    }
                    
                    bool has_AddressMode_Border() const
                    {
                        return m_AddressMode_Border_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AddressMode_Border( C _self, M _method ) const
                    {
                        if( m_AddressMode_Border_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AddressMode_Border );
                    
                        return true;
                    }
                    
                    bool get_AddressMode_Border( uint32_t * _value ) const
                    {
                        if( m_AddressMode_Border_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AddressMode_Border;
                    
                        return true;
                    }
                    
                    bool has_AddressMode_U() const
                    {
                        return m_AddressMode_U_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AddressMode_U( C _self, M _method ) const
                    {
                        if( m_AddressMode_U_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AddressMode_U );
                    
                        return true;
                    }
                    
                    bool get_AddressMode_U( Mengine::ETextureAddressMode * _value ) const
                    {
                        if( m_AddressMode_U_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AddressMode_U;
                    
                        return true;
                    }
                    
                    bool has_AddressMode_V() const
                    {
                        return m_AddressMode_V_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AddressMode_V( C _self, M _method ) const
                    {
                        if( m_AddressMode_V_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AddressMode_V );
                    
                        return true;
                    }
                    
                    bool get_AddressMode_V( Mengine::ETextureAddressMode * _value ) const
                    {
                        if( m_AddressMode_V_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AddressMode_V;
                    
                        return true;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    uint32_t m_Stage;
                    bool m_AddressMode_Border_successful;
                    uint32_t m_AddressMode_Border;
                    bool m_AddressMode_U_successful;
                    Mengine::ETextureAddressMode m_AddressMode_U;
                    bool m_AddressMode_V_successful;
                    Mengine::ETextureAddressMode m_AddressMode_V;
                };
                
            protected:
            protected:
                bool m_Debug_successful;
                bool m_Debug;
                Mengine::ConstString m_Name;
                bool m_RenderPlatform_successful;
                Mengine::ConstString m_RenderPlatform;
                bool m_AlphaBlend_Enable_successful;
                bool m_AlphaBlend_Enable;
                bool m_BlendFactor_Dest_successful;
                Mengine::EBlendFactor m_BlendFactor_Dest;
                bool m_BlendFactor_Op_successful;
                Mengine::EBlendOp m_BlendFactor_Op;
                bool m_BlendFactor_Source_successful;
                Mengine::EBlendFactor m_BlendFactor_Source;
                bool m_DepthBufferTest_Enable_successful;
                bool m_DepthBufferTest_Enable;
                bool m_DepthBufferWrite_Enable_successful;
                bool m_DepthBufferWrite_Enable;
                bool m_Program_Name_successful;
                Mengine::ConstString m_Program_Name;
            public:
                typedef Metabuf::Vector<Meta_TextureStages> VectorMeta_TextureStages;
            
                const VectorMeta_TextureStages & get_Includes_TextureStages() const
                {
                    return this->includes_Meta_TextureStages;
                }
            
            protected:
                VectorMeta_TextureStages includes_Meta_TextureStages;
            };
            
            class Meta_Program
                : public Metabuf::Metadata
            { 
            public:
                Meta_Program();
            
            public:
                template<class C, class M>
                void getm_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Name );
                }
                
                const Mengine::ConstString & get_Name() const
                {
                    return this->m_Name;
                }
                
                bool has_RenderPlatform() const
                {
                    return m_RenderPlatform_successful;
                }
                
                template<class C, class M>
                bool getm_RenderPlatform( C _self, M _method ) const
                {
                    if( m_RenderPlatform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_RenderPlatform );
                
                    return true;
                }
                
                bool get_RenderPlatform( Mengine::ConstString * _value ) const
                {
                    if( m_RenderPlatform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                
                bool getd_RenderPlatform( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( m_RenderPlatform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                template<class C, class M>
                void getm_FragmentShader_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_FragmentShader_Name );
                }
                
                const Mengine::ConstString & get_FragmentShader_Name() const
                {
                    return this->m_FragmentShader_Name;
                }
                
                template<class C, class M>
                void getm_Sampler_Count( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Sampler_Count );
                }
                
                uint32_t get_Sampler_Count() const
                {
                    return this->m_Sampler_Count;
                }
                
                template<class C, class M>
                void getm_VertexAttribute_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_VertexAttribute_Name );
                }
                
                const Mengine::ConstString & get_VertexAttribute_Name() const
                {
                    return this->m_VertexAttribute_Name;
                }
                
                template<class C, class M>
                void getm_VertexShader_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_VertexShader_Name );
                }
                
                const Mengine::ConstString & get_VertexShader_Name() const
                {
                    return this->m_VertexShader_Name;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::ConstString m_Name;
                bool m_RenderPlatform_successful;
                Mengine::ConstString m_RenderPlatform;
                Mengine::ConstString m_FragmentShader_Name;
                uint32_t m_Sampler_Count;
                Mengine::ConstString m_VertexAttribute_Name;
                Mengine::ConstString m_VertexShader_Name;
            };
            
            class Meta_VertexAttribute
                : public Metabuf::Metadata
            { 
            public:
                Meta_VertexAttribute();
            
            public:
                template<class C, class M>
                void getm_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Name );
                }
                
                const Mengine::ConstString & get_Name() const
                {
                    return this->m_Name;
                }
                
                template<class C, class M>
                void getm_RenderPlatform( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_RenderPlatform );
                }
                
                const Mengine::ConstString & get_RenderPlatform() const
                {
                    return this->m_RenderPlatform;
                }
                
                template<class C, class M>
                void getm_Element_Size( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Element_Size );
                }
                
                uint32_t get_Element_Size() const
                {
                    return this->m_Element_Size;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Attribute
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Attribute();
                
                public:
                    template<class C, class M>
                    void getm_Normalized( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Normalized );
                    }
                    
                    bool get_Normalized() const
                    {
                        return this->m_Normalized;
                    }
                    
                    template<class C, class M>
                    void getm_Offset( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Offset );
                    }
                    
                    uint32_t get_Offset() const
                    {
                        return this->m_Offset;
                    }
                    
                    template<class C, class M>
                    void getm_Size( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Size );
                    }
                    
                    uint32_t get_Size() const
                    {
                        return this->m_Size;
                    }
                    
                    template<class C, class M>
                    void getm_Stride( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Stride );
                    }
                    
                    uint32_t get_Stride() const
                    {
                        return this->m_Stride;
                    }
                    
                    template<class C, class M>
                    void getm_Type( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Type );
                    }
                    
                    Mengine::EVertexAttributeType get_Type() const
                    {
                        return this->m_Type;
                    }
                    
                    template<class C, class M>
                    void getm_Uniform( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Uniform );
                    }
                    
                    const Mengine::ConstString & get_Uniform() const
                    {
                        return this->m_Uniform;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    bool m_Normalized;
                    uint32_t m_Offset;
                    uint32_t m_Size;
                    uint32_t m_Stride;
                    Mengine::EVertexAttributeType m_Type;
                    Mengine::ConstString m_Uniform;
                };
                
            protected:
            protected:
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                uint32_t m_Element_Size;
            public:
                typedef Metabuf::Vector<Meta_Attribute> VectorMeta_Attribute;
            
                const VectorMeta_Attribute & get_Includes_Attribute() const
                {
                    return this->includes_Meta_Attribute;
                }
            
            protected:
                VectorMeta_Attribute includes_Meta_Attribute;
            };
            
            class Meta_VertexShader
                : public Metabuf::Metadata
            { 
            public:
                Meta_VertexShader();
            
            public:
                template<class C, class M>
                void getm_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Name );
                }
                
                const Mengine::ConstString & get_Name() const
                {
                    return this->m_Name;
                }
                
                template<class C, class M>
                void getm_RenderPlatform( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_RenderPlatform );
                }
                
                const Mengine::ConstString & get_RenderPlatform() const
                {
                    return this->m_RenderPlatform;
                }
                
                bool has_File_Compile() const
                {
                    return m_File_Compile_successful;
                }
                
                template<class C, class M>
                bool getm_File_Compile( C _self, M _method ) const
                {
                    if( m_File_Compile_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Compile );
                
                    return true;
                }
                
                bool get_File_Compile( bool * _value ) const
                {
                    if( m_File_Compile_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Compile;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                bool m_File_Compile_successful;
                bool m_File_Compile;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_Resource
                : public Metabuf::Metadata
            { 
            public:
                Meta_Resource();
                virtual ~Meta_Resource();
            
            public:
                template<class C, class M>
                void getm_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Name );
                }
                
                const Mengine::ConstString & get_Name() const
                {
                    return this->m_Name;
                }
                
                bool has_Precompile() const
                {
                    return m_Precompile_successful;
                }
                
                bool getd_Precompile( bool _default ) const
                {
                    if( m_Precompile_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Precompile;
                }
                
                bool get_Precompile( bool * _value ) const
                {
                    if( m_Precompile_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Precompile;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Precompile( C _self, M _method ) const
                {
                    if( m_Precompile_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Precompile );
                
                    return true;
                }
                
                bool getd_Precompile( bool * _value, const bool & _default ) const
                {
                    if( m_Precompile_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Precompile;
                
                    return true;
                }
                template<class C, class M>
                void getm_Type( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Type );
                }
                
                const Mengine::ConstString & get_Type() const
                {
                    return this->m_Type;
                }
                
                bool has_Unique() const
                {
                    return m_Unique_successful;
                }
                
                bool getd_Unique( bool _default ) const
                {
                    if( m_Unique_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Unique;
                }
                
                bool get_Unique( bool * _value ) const
                {
                    if( m_Unique_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Unique;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Unique( C _self, M _method ) const
                {
                    if( m_Unique_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Unique );
                
                    return true;
                }
                
                bool getd_Unique( bool * _value, const bool & _default ) const
                {
                    if( m_Unique_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Unique;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::ConstString m_Name;
                bool m_Precompile_successful;
                bool m_Precompile;
                Mengine::ConstString m_Type;
                bool m_Unique_successful;
                bool m_Unique;
            };
            
            class Meta_ResourceAstralax
                : public Meta_Resource
            { 
            public:
                Meta_ResourceAstralax();
            
            public:
                template<class C, class M>
                void getm_AtlasCount_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_AtlasCount_Value );
                }
                
                uint32_t get_AtlasCount_Value() const
                {
                    return this->m_AtlasCount_Value;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Atlas
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Atlas();
                
                public:
                    template<class C, class M>
                    void getm_Index( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Index );
                    }
                    
                    uint32_t get_Index() const
                    {
                        return this->m_Index;
                    }
                    
                    template<class C, class M>
                    void getm_ResourceName( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_ResourceName );
                    }
                    
                    const Mengine::ConstString & get_ResourceName() const
                    {
                        return this->m_ResourceName;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    uint32_t m_Index;
                    Mengine::ConstString m_ResourceName;
                };
                
            protected:
            protected:
                uint32_t m_AtlasCount_Value;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                Mengine::FilePath m_File_Path;
            public:
                typedef Metabuf::Vector<Meta_Atlas> VectorMeta_Atlas;
            
                const VectorMeta_Atlas & get_Includes_Atlas() const
                {
                    return this->includes_Meta_Atlas;
                }
            
            protected:
                VectorMeta_Atlas includes_Meta_Atlas;
            };
            
            class Meta_ResourceCal3dAnimation
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCal3dAnimation();
            
            public:
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCal3dMesh
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCal3dMesh();
            
            public:
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCal3dSkeleton
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCal3dSkeleton();
            
            public:
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCursorICO
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCursorICO();
            
            public:
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCursorSystem
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCursorSystem();
            
            public:
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceExternal
                : public Meta_Resource
            { 
            public:
                Meta_ResourceExternal();
            
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
            };
            
            class Meta_ResourceFile
                : public Meta_Resource
            { 
            public:
                Meta_ResourceFile();
            
            public:
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceHIT
                : public Meta_Resource
            { 
            public:
                Meta_ResourceHIT();
            
            public:
                bool has_File_Codec() const
                {
                    return m_File_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_File_Codec_successful;
                Mengine::ConstString m_File_Codec;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceImageData
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageData();
            
            public:
                bool has_File_Alpha() const
                {
                    return m_File_Alpha_successful;
                }
                
                template<class C, class M>
                bool getm_File_Alpha( C _self, M _method ) const
                {
                    if( m_File_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Alpha );
                
                    return true;
                }
                
                bool get_File_Alpha( bool * _value ) const
                {
                    if( m_File_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Alpha;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return m_File_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_MaxSize( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_MaxSize );
                }
                
                const mt::vec2f & get_File_MaxSize() const
                {
                    return this->m_File_MaxSize;
                }
                
                bool has_File_NoExist() const
                {
                    return m_File_NoExist_successful;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( m_File_NoExist_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( m_File_NoExist_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
                    return true;
                }
                
                bool has_File_Offset() const
                {
                    return m_File_Offset_successful;
                }
                
                template<class C, class M>
                bool getm_File_Offset( C _self, M _method ) const
                {
                    if( m_File_Offset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Offset );
                
                    return true;
                }
                
                bool get_File_Offset( mt::vec2f * _value ) const
                {
                    if( m_File_Offset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Offset;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
                bool has_File_Premultiply() const
                {
                    return m_File_Premultiply_successful;
                }
                
                template<class C, class M>
                bool getm_File_Premultiply( C _self, M _method ) const
                {
                    if( m_File_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Premultiply );
                
                    return true;
                }
                
                bool get_File_Premultiply( bool * _value ) const
                {
                    if( m_File_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Premultiply;
                
                    return true;
                }
                
                bool has_File_Size() const
                {
                    return m_File_Size_successful;
                }
                
                template<class C, class M>
                bool getm_File_Size( C _self, M _method ) const
                {
                    if( m_File_Size_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Size );
                
                    return true;
                }
                
                bool get_File_Size( mt::vec2f * _value ) const
                {
                    if( m_File_Size_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Size;
                
                    return true;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_File_Alpha_successful;
                bool m_File_Alpha;
                bool m_File_Codec_successful;
                Mengine::ConstString m_File_Codec;
                mt::vec2f m_File_MaxSize;
                bool m_File_NoExist_successful;
                bool m_File_NoExist;
                bool m_File_Offset_successful;
                mt::vec2f m_File_Offset;
                Mengine::FilePath m_File_Path;
                bool m_File_Premultiply_successful;
                bool m_File_Premultiply;
                bool m_File_Size_successful;
                mt::vec2f m_File_Size;
            };
            
            class Meta_ResourceImageDefault
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageDefault();
            
            public:
                bool has_File_Alpha() const
                {
                    return m_File_Alpha_successful;
                }
                
                template<class C, class M>
                bool getm_File_Alpha( C _self, M _method ) const
                {
                    if( m_File_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Alpha );
                
                    return true;
                }
                
                bool get_File_Alpha( bool * _value ) const
                {
                    if( m_File_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Alpha;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return m_File_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_MaxSize( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_MaxSize );
                }
                
                const mt::vec2f & get_File_MaxSize() const
                {
                    return this->m_File_MaxSize;
                }
                
                bool has_File_NoExist() const
                {
                    return m_File_NoExist_successful;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( m_File_NoExist_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( m_File_NoExist_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
                    return true;
                }
                
                bool has_File_Offset() const
                {
                    return m_File_Offset_successful;
                }
                
                template<class C, class M>
                bool getm_File_Offset( C _self, M _method ) const
                {
                    if( m_File_Offset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Offset );
                
                    return true;
                }
                
                bool get_File_Offset( mt::vec2f * _value ) const
                {
                    if( m_File_Offset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Offset;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
                bool has_File_Premultiply() const
                {
                    return m_File_Premultiply_successful;
                }
                
                template<class C, class M>
                bool getm_File_Premultiply( C _self, M _method ) const
                {
                    if( m_File_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Premultiply );
                
                    return true;
                }
                
                bool get_File_Premultiply( bool * _value ) const
                {
                    if( m_File_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Premultiply;
                
                    return true;
                }
                
                bool has_File_Size() const
                {
                    return m_File_Size_successful;
                }
                
                template<class C, class M>
                bool getm_File_Size( C _self, M _method ) const
                {
                    if( m_File_Size_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Size );
                
                    return true;
                }
                
                bool get_File_Size( mt::vec2f * _value ) const
                {
                    if( m_File_Size_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Size;
                
                    return true;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_File_Alpha_successful;
                bool m_File_Alpha;
                bool m_File_Codec_successful;
                Mengine::ConstString m_File_Codec;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                mt::vec2f m_File_MaxSize;
                bool m_File_NoExist_successful;
                bool m_File_NoExist;
                bool m_File_Offset_successful;
                mt::vec2f m_File_Offset;
                Mengine::FilePath m_File_Path;
                bool m_File_Premultiply_successful;
                bool m_File_Premultiply;
                bool m_File_Size_successful;
                mt::vec2f m_File_Size;
            };
            
            class Meta_ResourceImageSequence
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSequence();
            
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Sequence
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Sequence();
                
                public:
                    template<class C, class M>
                    void getm_Delay( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Delay );
                    }
                    
                    float get_Delay() const
                    {
                        return this->m_Delay;
                    }
                    
                    template<class C, class M>
                    void getm_ResourceImageName( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_ResourceImageName );
                    }
                    
                    const Mengine::ConstString & get_ResourceImageName() const
                    {
                        return this->m_ResourceImageName;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    float m_Delay;
                    Mengine::ConstString m_ResourceImageName;
                };
                
            protected:
            protected:
            public:
                typedef Metabuf::Vector<Meta_Sequence> VectorMeta_Sequence;
            
                const VectorMeta_Sequence & get_Includes_Sequence() const
                {
                    return this->includes_Meta_Sequence;
                }
            
            protected:
                VectorMeta_Sequence includes_Meta_Sequence;
            };
            
            class Meta_ResourceImageSolid
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSolid();
            
            public:
                template<class C, class M>
                void getm_Color_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Color_Value );
                }
                
                const Mengine::Color & get_Color_Value() const
                {
                    return this->m_Color_Value;
                }
                
                template<class C, class M>
                void getm_Size_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Size_Value );
                }
                
                const mt::vec2f & get_Size_Value() const
                {
                    return this->m_Size_Value;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::Color m_Color_Value;
                mt::vec2f m_Size_Value;
            };
            
            class Meta_ResourceImageSubstract
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSubstract();
            
            public:
                bool has_Image_Alpha() const
                {
                    return m_Image_Alpha_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Alpha( C _self, M _method ) const
                {
                    if( m_Image_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Alpha );
                
                    return true;
                }
                
                bool get_Image_Alpha( bool * _value ) const
                {
                    if( m_Image_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Alpha;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_Image_MaxSize( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_MaxSize );
                }
                
                const mt::vec2f & get_Image_MaxSize() const
                {
                    return this->m_Image_MaxSize;
                }
                
                template<class C, class M>
                void getm_Image_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_Name );
                }
                
                const Mengine::ConstString & get_Image_Name() const
                {
                    return this->m_Image_Name;
                }
                
                bool has_Image_Offset() const
                {
                    return m_Image_Offset_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Offset( C _self, M _method ) const
                {
                    if( m_Image_Offset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Offset );
                
                    return true;
                }
                
                bool get_Image_Offset( mt::vec2f * _value ) const
                {
                    if( m_Image_Offset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Offset;
                
                    return true;
                }
                
                bool has_Image_Premultiply() const
                {
                    return m_Image_Premultiply_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Premultiply( C _self, M _method ) const
                {
                    if( m_Image_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Premultiply );
                
                    return true;
                }
                
                bool get_Image_Premultiply( bool * _value ) const
                {
                    if( m_Image_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Premultiply;
                
                    return true;
                }
                
                bool has_Image_Size() const
                {
                    return m_Image_Size_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Size( C _self, M _method ) const
                {
                    if( m_Image_Size_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Size );
                
                    return true;
                }
                
                bool get_Image_Size( mt::vec2f * _value ) const
                {
                    if( m_Image_Size_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Size;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_Image_UV( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_UV );
                }
                
                const mt::uv4f & get_Image_UV() const
                {
                    return this->m_Image_UV;
                }
                
                bool has_Image_UVRotate() const
                {
                    return m_Image_UVRotate_successful;
                }
                
                template<class C, class M>
                bool getm_Image_UVRotate( C _self, M _method ) const
                {
                    if( m_Image_UVRotate_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_UVRotate );
                
                    return true;
                }
                
                bool get_Image_UVRotate( bool * _value ) const
                {
                    if( m_Image_UVRotate_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_UVRotate;
                
                    return true;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_Image_Alpha_successful;
                bool m_Image_Alpha;
                mt::vec2f m_Image_MaxSize;
                Mengine::ConstString m_Image_Name;
                bool m_Image_Offset_successful;
                mt::vec2f m_Image_Offset;
                bool m_Image_Premultiply_successful;
                bool m_Image_Premultiply;
                bool m_Image_Size_successful;
                mt::vec2f m_Image_Size;
                mt::uv4f m_Image_UV;
                bool m_Image_UVRotate_successful;
                bool m_Image_UVRotate;
            };
            
            class Meta_ResourceImageSubstractRGBAndAlpha
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSubstractRGBAndAlpha();
            
            public:
                template<class C, class M>
                void getm_Image_MaxSize( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_MaxSize );
                }
                
                const mt::vec2f & get_Image_MaxSize() const
                {
                    return this->m_Image_MaxSize;
                }
                
                template<class C, class M>
                void getm_Image_NameAlpha( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_NameAlpha );
                }
                
                const Mengine::ConstString & get_Image_NameAlpha() const
                {
                    return this->m_Image_NameAlpha;
                }
                
                template<class C, class M>
                void getm_Image_NameRGB( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_NameRGB );
                }
                
                const Mengine::ConstString & get_Image_NameRGB() const
                {
                    return this->m_Image_NameRGB;
                }
                
                bool has_Image_Offset() const
                {
                    return m_Image_Offset_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Offset( C _self, M _method ) const
                {
                    if( m_Image_Offset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Offset );
                
                    return true;
                }
                
                bool get_Image_Offset( mt::vec2f * _value ) const
                {
                    if( m_Image_Offset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Offset;
                
                    return true;
                }
                
                bool has_Image_Premultiply() const
                {
                    return m_Image_Premultiply_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Premultiply( C _self, M _method ) const
                {
                    if( m_Image_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Premultiply );
                
                    return true;
                }
                
                bool get_Image_Premultiply( bool * _value ) const
                {
                    if( m_Image_Premultiply_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Premultiply;
                
                    return true;
                }
                
                bool has_Image_Size() const
                {
                    return m_Image_Size_successful;
                }
                
                template<class C, class M>
                bool getm_Image_Size( C _self, M _method ) const
                {
                    if( m_Image_Size_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Size );
                
                    return true;
                }
                
                bool get_Image_Size( mt::vec2f * _value ) const
                {
                    if( m_Image_Size_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Size;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_Image_UVAlpha( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_UVAlpha );
                }
                
                const mt::uv4f & get_Image_UVAlpha() const
                {
                    return this->m_Image_UVAlpha;
                }
                
                bool has_Image_UVAlphaRotate() const
                {
                    return m_Image_UVAlphaRotate_successful;
                }
                
                template<class C, class M>
                bool getm_Image_UVAlphaRotate( C _self, M _method ) const
                {
                    if( m_Image_UVAlphaRotate_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_UVAlphaRotate );
                
                    return true;
                }
                
                bool get_Image_UVAlphaRotate( bool * _value ) const
                {
                    if( m_Image_UVAlphaRotate_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_UVAlphaRotate;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_Image_UVRGB( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_UVRGB );
                }
                
                const mt::uv4f & get_Image_UVRGB() const
                {
                    return this->m_Image_UVRGB;
                }
                
                bool has_Image_UVRGBRotate() const
                {
                    return m_Image_UVRGBRotate_successful;
                }
                
                template<class C, class M>
                bool getm_Image_UVRGBRotate( C _self, M _method ) const
                {
                    if( m_Image_UVRGBRotate_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_UVRGBRotate );
                
                    return true;
                }
                
                bool get_Image_UVRGBRotate( bool * _value ) const
                {
                    if( m_Image_UVRGBRotate_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_UVRGBRotate;
                
                    return true;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                mt::vec2f m_Image_MaxSize;
                Mengine::ConstString m_Image_NameAlpha;
                Mengine::ConstString m_Image_NameRGB;
                bool m_Image_Offset_successful;
                mt::vec2f m_Image_Offset;
                bool m_Image_Premultiply_successful;
                bool m_Image_Premultiply;
                bool m_Image_Size_successful;
                mt::vec2f m_Image_Size;
                mt::uv4f m_Image_UVAlpha;
                bool m_Image_UVAlphaRotate_successful;
                bool m_Image_UVAlphaRotate;
                mt::uv4f m_Image_UVRGB;
                bool m_Image_UVRGBRotate_successful;
                bool m_Image_UVRGBRotate;
            };
            
            class Meta_ResourceInternalObject
                : public Meta_Resource
            { 
            public:
                Meta_ResourceInternalObject();
            
            public:
                template<class C, class M>
                void getm_Internal_Group( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Internal_Group );
                }
                
                const Mengine::ConstString & get_Internal_Group() const
                {
                    return this->m_Internal_Group;
                }
                
                template<class C, class M>
                void getm_Internal_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Internal_Name );
                }
                
                const Mengine::ConstString & get_Internal_Name() const
                {
                    return this->m_Internal_Name;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::ConstString m_Internal_Group;
                Mengine::ConstString m_Internal_Name;
            };
            
            class Meta_ResourceModel3D
                : public Meta_Resource
            { 
            public:
                Meta_ResourceModel3D();
            
            public:
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_Dataflow() const
                {
                    return m_File_Dataflow_successful;
                }
                
                template<class C, class M>
                bool getm_File_Dataflow( C _self, M _method ) const
                {
                    if( m_File_Dataflow_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Dataflow );
                
                    return true;
                }
                
                bool get_File_Dataflow( Mengine::ConstString * _value ) const
                {
                    if( m_File_Dataflow_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Dataflow;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
                template<class C, class M>
                void getm_Image_Resource( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_Resource );
                }
                
                const Mengine::ConstString & get_Image_Resource() const
                {
                    return this->m_Image_Resource;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                bool m_File_Dataflow_successful;
                Mengine::ConstString m_File_Dataflow;
                Mengine::FilePath m_File_Path;
                Mengine::ConstString m_Image_Resource;
            };
            
            class Meta_ResourceMovie
                : public Meta_Resource
            { 
            public:
                Meta_ResourceMovie();
            
            public:
                bool has_Anchor_Point() const
                {
                    return m_Anchor_Point_successful;
                }
                
                template<class C, class M>
                bool getm_Anchor_Point( C _self, M _method ) const
                {
                    if( m_Anchor_Point_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Anchor_Point );
                
                    return true;
                }
                
                bool get_Anchor_Point( mt::vec3f * _value ) const
                {
                    if( m_Anchor_Point_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Anchor_Point;
                
                    return true;
                }
                
                bool has_Bounds_Box() const
                {
                    return m_Bounds_Box_successful;
                }
                
                template<class C, class M>
                bool getm_Bounds_Box( C _self, M _method ) const
                {
                    if( m_Bounds_Box_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Bounds_Box );
                
                    return true;
                }
                
                bool get_Bounds_Box( mt::box2f * _value ) const
                {
                    if( m_Bounds_Box_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Bounds_Box;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_Duration_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Duration_Value );
                }
                
                float get_Duration_Value() const
                {
                    return this->m_Duration_Value;
                }
                
                template<class C, class M>
                void getm_FrameDuration_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_FrameDuration_Value );
                }
                
                float get_FrameDuration_Value() const
                {
                    return this->m_FrameDuration_Value;
                }
                
                template<class C, class M>
                void getm_Height_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Height_Value );
                }
                
                float get_Height_Value() const
                {
                    return this->m_Height_Value;
                }
                
                bool has_KeyFramesPackPath_Codec() const
                {
                    return m_KeyFramesPackPath_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_KeyFramesPackPath_Codec( C _self, M _method ) const
                {
                    if( m_KeyFramesPackPath_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_KeyFramesPackPath_Codec );
                
                    return true;
                }
                
                bool get_KeyFramesPackPath_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_KeyFramesPackPath_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_KeyFramesPackPath_Codec;
                
                    return true;
                }
                
                bool has_KeyFramesPackPath_Converter() const
                {
                    return m_KeyFramesPackPath_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_KeyFramesPackPath_Converter( C _self, M _method ) const
                {
                    if( m_KeyFramesPackPath_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_KeyFramesPackPath_Converter );
                
                    return true;
                }
                
                bool get_KeyFramesPackPath_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_KeyFramesPackPath_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_KeyFramesPackPath_Converter;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_KeyFramesPackPath_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_KeyFramesPackPath_Path );
                }
                
                const Mengine::FilePath & get_KeyFramesPackPath_Path() const
                {
                    return this->m_KeyFramesPackPath_Path;
                }
                
                bool has_Loop_Segment() const
                {
                    return m_Loop_Segment_successful;
                }
                
                template<class C, class M>
                bool getm_Loop_Segment( C _self, M _method ) const
                {
                    if( m_Loop_Segment_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Loop_Segment );
                
                    return true;
                }
                
                bool get_Loop_Segment( mt::vec2f * _value ) const
                {
                    if( m_Loop_Segment_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Loop_Segment;
                
                    return true;
                }
                
                bool has_Offset_Point() const
                {
                    return m_Offset_Point_successful;
                }
                
                template<class C, class M>
                bool getm_Offset_Point( C _self, M _method ) const
                {
                    if( m_Offset_Point_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Offset_Point );
                
                    return true;
                }
                
                bool get_Offset_Point( mt::vec3f * _value ) const
                {
                    if( m_Offset_Point_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Offset_Point;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_Width_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Width_Value );
                }
                
                float get_Width_Value() const
                {
                    return this->m_Width_Value;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_MovieCamera3D
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_MovieCamera3D();
                
                public:
                    template<class C, class M>
                    void getm_CameraAspect( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_CameraAspect );
                    }
                    
                    float get_CameraAspect() const
                    {
                        return this->m_CameraAspect;
                    }
                    
                    template<class C, class M>
                    void getm_CameraFOV( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_CameraFOV );
                    }
                    
                    float get_CameraFOV() const
                    {
                        return this->m_CameraFOV;
                    }
                    
                    template<class C, class M>
                    void getm_CameraInterest( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_CameraInterest );
                    }
                    
                    const mt::vec3f & get_CameraInterest() const
                    {
                        return this->m_CameraInterest;
                    }
                    
                    template<class C, class M>
                    void getm_CameraPosition( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_CameraPosition );
                    }
                    
                    const mt::vec3f & get_CameraPosition() const
                    {
                        return this->m_CameraPosition;
                    }
                    
                    template<class C, class M>
                    void getm_Height( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Height );
                    }
                    
                    float get_Height() const
                    {
                        return this->m_Height;
                    }
                    
                    template<class C, class M>
                    void getm_Width( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Width );
                    }
                    
                    float get_Width() const
                    {
                        return this->m_Width;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    float m_CameraAspect;
                    float m_CameraFOV;
                    mt::vec3f m_CameraInterest;
                    mt::vec3f m_CameraPosition;
                    float m_Height;
                    float m_Width;
                };
                
                class Meta_MovieLayer2D
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_MovieLayer2D();
                
                public:
                    bool has_AnchorPoint() const
                    {
                        return m_AnchorPoint_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec3f * _value ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_BlendingMode() const
                    {
                        return m_BlendingMode_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_BlendingMode( C _self, M _method ) const
                    {
                        if( m_BlendingMode_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_BlendingMode );
                    
                        return true;
                    }
                    
                    bool get_BlendingMode( Mengine::ConstString * _value ) const
                    {
                        if( m_BlendingMode_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_BlendingMode;
                    
                        return true;
                    }
                    
                    bool getd_BlendingMode( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( m_BlendingMode_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_BlendingMode;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_In( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_In );
                    }
                    
                    float get_In() const
                    {
                        return this->m_In;
                    }
                    
                    template<class C, class M>
                    void getm_Index( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Index );
                    }
                    
                    uint32_t get_Index() const
                    {
                        return this->m_Index;
                    }
                    
                    bool has_Loop() const
                    {
                        return m_Loop_successful;
                    }
                    
                    bool getd_Loop( bool _default ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Loop;
                    }
                    
                    bool get_Loop( bool * _value ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Loop;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Loop( C _self, M _method ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Loop );
                    
                        return true;
                    }
                    
                    bool getd_Loop( bool * _value, const bool & _default ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Loop;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Name( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Name );
                    }
                    
                    const Mengine::ConstString & get_Name() const
                    {
                        return this->m_Name;
                    }
                    
                    template<class C, class M>
                    void getm_Out( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Out );
                    }
                    
                    float get_Out() const
                    {
                        return this->m_Out;
                    }
                    
                    bool has_Params() const
                    {
                        return m_Params_successful;
                    }
                    
                    uint32_t getd_Params( uint32_t _default ) const
                    {
                        if( m_Params_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Params;
                    }
                    
                    bool get_Params( uint32_t * _value ) const
                    {
                        if( m_Params_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Params( C _self, M _method ) const
                    {
                        if( m_Params_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Params );
                    
                        return true;
                    }
                    
                    bool getd_Params( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_Params_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    bool has_Parent() const
                    {
                        return m_Parent_successful;
                    }
                    
                    uint32_t getd_Parent( uint32_t _default ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Parent;
                    }
                    
                    bool get_Parent( uint32_t * _value ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Parent( C _self, M _method ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Parent );
                    
                        return true;
                    }
                    
                    bool getd_Parent( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    bool has_PlayCount() const
                    {
                        return m_PlayCount_successful;
                    }
                    
                    uint32_t getd_PlayCount( uint32_t _default ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_PlayCount;
                    }
                    
                    bool get_PlayCount( uint32_t * _value ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_PlayCount( C _self, M _method ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_PlayCount );
                    
                        return true;
                    }
                    
                    bool getd_PlayCount( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    bool has_Polygon() const
                    {
                        return m_Polygon_successful;
                    }
                    
                    bool getd_Polygon( bool _default ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Polygon;
                    }
                    
                    bool get_Polygon( bool * _value ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Polygon( C _self, M _method ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Polygon );
                    
                        return true;
                    }
                    
                    bool getd_Polygon( bool * _value, const bool & _default ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return m_Position_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec3f * _value ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Position_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return m_Rotation_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool get_Rotation( mt::vec3f * _value ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    bool getd_Rotation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return m_Scale_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec3f * _value ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Shape() const
                    {
                        return m_Shape_successful;
                    }
                    
                    bool getd_Shape( bool _default ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Shape;
                    }
                    
                    bool get_Shape( bool * _value ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Shape;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Shape( C _self, M _method ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Shape );
                    
                        return true;
                    }
                    
                    bool getd_Shape( bool * _value, const bool & _default ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Shape;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Source( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Source );
                    }
                    
                    const Mengine::ConstString & get_Source() const
                    {
                        return this->m_Source;
                    }
                    
                    bool has_StartInterval() const
                    {
                        return m_StartInterval_successful;
                    }
                    
                    float getd_StartInterval( float _default ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_StartInterval;
                    }
                    
                    bool get_StartInterval( float * _value ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_StartInterval( C _self, M _method ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_StartInterval );
                    
                        return true;
                    }
                    
                    bool getd_StartInterval( float * _value, const float & _default ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    bool has_Stretch() const
                    {
                        return m_Stretch_successful;
                    }
                    
                    float getd_Stretch( float _default ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Stretch;
                    }
                    
                    bool get_Stretch( float * _value ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Stretch( C _self, M _method ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Stretch );
                    
                        return true;
                    }
                    
                    bool getd_Stretch( float * _value, const float & _default ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    bool has_Switch() const
                    {
                        return m_Switch_successful;
                    }
                    
                    bool getd_Switch( bool _default ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Switch;
                    }
                    
                    bool get_Switch( bool * _value ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Switch( C _self, M _method ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Switch );
                    
                        return true;
                    }
                    
                    bool getd_Switch( bool * _value, const bool & _default ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    bool has_TimeRemap() const
                    {
                        return m_TimeRemap_successful;
                    }
                    
                    bool getd_TimeRemap( bool _default ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_TimeRemap;
                    }
                    
                    bool get_TimeRemap( bool * _value ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_TimeRemap;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_TimeRemap( C _self, M _method ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_TimeRemap );
                    
                        return true;
                    }
                    
                    bool getd_TimeRemap( bool * _value, const bool & _default ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_TimeRemap;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Type( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Type );
                    }
                    
                    const Mengine::ConstString & get_Type() const
                    {
                        return this->m_Type;
                    }
                    
                    bool has_Viewport() const
                    {
                        return m_Viewport_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Viewport( C _self, M _method ) const
                    {
                        if( m_Viewport_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Viewport );
                    
                        return true;
                    }
                    
                    bool get_Viewport( Mengine::Viewport * _value ) const
                    {
                        if( m_Viewport_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Viewport;
                    
                        return true;
                    }
                    
                    bool getd_Viewport( Mengine::Viewport * _value, const Mengine::Viewport & _default ) const
                    {
                        if( m_Viewport_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Viewport;
                    
                        return true;
                    }
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    bool m_AnchorPoint_successful;
                    mt::vec3f m_AnchorPoint;
                    bool m_BlendingMode_successful;
                    Mengine::ConstString m_BlendingMode;
                    float m_In;
                    uint32_t m_Index;
                    bool m_Loop_successful;
                    bool m_Loop;
                    Mengine::ConstString m_Name;
                    float m_Out;
                    bool m_Params_successful;
                    uint32_t m_Params;
                    bool m_Parent_successful;
                    uint32_t m_Parent;
                    bool m_PlayCount_successful;
                    uint32_t m_PlayCount;
                    bool m_Polygon_successful;
                    bool m_Polygon;
                    bool m_Position_successful;
                    mt::vec3f m_Position;
                    bool m_Rotation_successful;
                    mt::vec3f m_Rotation;
                    bool m_Scale_successful;
                    mt::vec3f m_Scale;
                    bool m_Shape_successful;
                    bool m_Shape;
                    Mengine::ConstString m_Source;
                    bool m_StartInterval_successful;
                    float m_StartInterval;
                    bool m_Stretch_successful;
                    float m_Stretch;
                    bool m_Switch_successful;
                    bool m_Switch;
                    bool m_TimeRemap_successful;
                    bool m_TimeRemap;
                    Mengine::ConstString m_Type;
                    bool m_Viewport_successful;
                    Mengine::Viewport m_Viewport;
                };
                
                class Meta_MovieLayer3D
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_MovieLayer3D();
                
                public:
                    bool has_AnchorPoint() const
                    {
                        return m_AnchorPoint_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec3f * _value ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_BlendingMode() const
                    {
                        return m_BlendingMode_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_BlendingMode( C _self, M _method ) const
                    {
                        if( m_BlendingMode_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_BlendingMode );
                    
                        return true;
                    }
                    
                    bool get_BlendingMode( Mengine::ConstString * _value ) const
                    {
                        if( m_BlendingMode_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_BlendingMode;
                    
                        return true;
                    }
                    
                    bool getd_BlendingMode( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( m_BlendingMode_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_BlendingMode;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_In( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_In );
                    }
                    
                    float get_In() const
                    {
                        return this->m_In;
                    }
                    
                    template<class C, class M>
                    void getm_Index( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Index );
                    }
                    
                    uint32_t get_Index() const
                    {
                        return this->m_Index;
                    }
                    
                    bool has_Loop() const
                    {
                        return m_Loop_successful;
                    }
                    
                    bool getd_Loop( bool _default ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Loop;
                    }
                    
                    bool get_Loop( bool * _value ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Loop;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Loop( C _self, M _method ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Loop );
                    
                        return true;
                    }
                    
                    bool getd_Loop( bool * _value, const bool & _default ) const
                    {
                        if( m_Loop_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Loop;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Name( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Name );
                    }
                    
                    const Mengine::ConstString & get_Name() const
                    {
                        return this->m_Name;
                    }
                    
                    template<class C, class M>
                    void getm_Out( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Out );
                    }
                    
                    float get_Out() const
                    {
                        return this->m_Out;
                    }
                    
                    bool has_Params() const
                    {
                        return m_Params_successful;
                    }
                    
                    uint32_t getd_Params( uint32_t _default ) const
                    {
                        if( m_Params_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Params;
                    }
                    
                    bool get_Params( uint32_t * _value ) const
                    {
                        if( m_Params_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Params( C _self, M _method ) const
                    {
                        if( m_Params_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Params );
                    
                        return true;
                    }
                    
                    bool getd_Params( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_Params_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    bool has_Parent() const
                    {
                        return m_Parent_successful;
                    }
                    
                    uint32_t getd_Parent( uint32_t _default ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Parent;
                    }
                    
                    bool get_Parent( uint32_t * _value ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Parent( C _self, M _method ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Parent );
                    
                        return true;
                    }
                    
                    bool getd_Parent( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_Parent_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    bool has_PlayCount() const
                    {
                        return m_PlayCount_successful;
                    }
                    
                    uint32_t getd_PlayCount( uint32_t _default ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_PlayCount;
                    }
                    
                    bool get_PlayCount( uint32_t * _value ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_PlayCount( C _self, M _method ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_PlayCount );
                    
                        return true;
                    }
                    
                    bool getd_PlayCount( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_PlayCount_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    bool has_Polygon() const
                    {
                        return m_Polygon_successful;
                    }
                    
                    bool getd_Polygon( bool _default ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Polygon;
                    }
                    
                    bool get_Polygon( bool * _value ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Polygon( C _self, M _method ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Polygon );
                    
                        return true;
                    }
                    
                    bool getd_Polygon( bool * _value, const bool & _default ) const
                    {
                        if( m_Polygon_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return m_Position_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec3f * _value ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Position_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return m_Rotation_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool get_Rotation( mt::vec3f * _value ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    bool getd_Rotation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return m_Scale_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec3f * _value ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Shape() const
                    {
                        return m_Shape_successful;
                    }
                    
                    bool getd_Shape( bool _default ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Shape;
                    }
                    
                    bool get_Shape( bool * _value ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Shape;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Shape( C _self, M _method ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Shape );
                    
                        return true;
                    }
                    
                    bool getd_Shape( bool * _value, const bool & _default ) const
                    {
                        if( m_Shape_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Shape;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Source( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Source );
                    }
                    
                    const Mengine::ConstString & get_Source() const
                    {
                        return this->m_Source;
                    }
                    
                    bool has_StartInterval() const
                    {
                        return m_StartInterval_successful;
                    }
                    
                    float getd_StartInterval( float _default ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_StartInterval;
                    }
                    
                    bool get_StartInterval( float * _value ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_StartInterval( C _self, M _method ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_StartInterval );
                    
                        return true;
                    }
                    
                    bool getd_StartInterval( float * _value, const float & _default ) const
                    {
                        if( m_StartInterval_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    bool has_Stretch() const
                    {
                        return m_Stretch_successful;
                    }
                    
                    float getd_Stretch( float _default ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Stretch;
                    }
                    
                    bool get_Stretch( float * _value ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Stretch( C _self, M _method ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Stretch );
                    
                        return true;
                    }
                    
                    bool getd_Stretch( float * _value, const float & _default ) const
                    {
                        if( m_Stretch_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    bool has_Switch() const
                    {
                        return m_Switch_successful;
                    }
                    
                    bool getd_Switch( bool _default ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Switch;
                    }
                    
                    bool get_Switch( bool * _value ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Switch( C _self, M _method ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Switch );
                    
                        return true;
                    }
                    
                    bool getd_Switch( bool * _value, const bool & _default ) const
                    {
                        if( m_Switch_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    bool has_TimeRemap() const
                    {
                        return m_TimeRemap_successful;
                    }
                    
                    bool getd_TimeRemap( bool _default ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_TimeRemap;
                    }
                    
                    bool get_TimeRemap( bool * _value ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_TimeRemap;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_TimeRemap( C _self, M _method ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_TimeRemap );
                    
                        return true;
                    }
                    
                    bool getd_TimeRemap( bool * _value, const bool & _default ) const
                    {
                        if( m_TimeRemap_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_TimeRemap;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Type( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Type );
                    }
                    
                    const Mengine::ConstString & get_Type() const
                    {
                        return this->m_Type;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    bool m_AnchorPoint_successful;
                    mt::vec3f m_AnchorPoint;
                    bool m_BlendingMode_successful;
                    Mengine::ConstString m_BlendingMode;
                    float m_In;
                    uint32_t m_Index;
                    bool m_Loop_successful;
                    bool m_Loop;
                    Mengine::ConstString m_Name;
                    float m_Out;
                    bool m_Params_successful;
                    uint32_t m_Params;
                    bool m_Parent_successful;
                    uint32_t m_Parent;
                    bool m_PlayCount_successful;
                    uint32_t m_PlayCount;
                    bool m_Polygon_successful;
                    bool m_Polygon;
                    bool m_Position_successful;
                    mt::vec3f m_Position;
                    bool m_Rotation_successful;
                    mt::vec3f m_Rotation;
                    bool m_Scale_successful;
                    mt::vec3f m_Scale;
                    bool m_Shape_successful;
                    bool m_Shape;
                    Mengine::ConstString m_Source;
                    bool m_StartInterval_successful;
                    float m_StartInterval;
                    bool m_Stretch_successful;
                    float m_Stretch;
                    bool m_Switch_successful;
                    bool m_Switch;
                    bool m_TimeRemap_successful;
                    bool m_TimeRemap;
                    Mengine::ConstString m_Type;
                };
                
            protected:
            protected:
                bool m_Anchor_Point_successful;
                mt::vec3f m_Anchor_Point;
                bool m_Bounds_Box_successful;
                mt::box2f m_Bounds_Box;
                float m_Duration_Value;
                float m_FrameDuration_Value;
                float m_Height_Value;
                bool m_KeyFramesPackPath_Codec_successful;
                Mengine::ConstString m_KeyFramesPackPath_Codec;
                bool m_KeyFramesPackPath_Converter_successful;
                Mengine::ConstString m_KeyFramesPackPath_Converter;
                Mengine::FilePath m_KeyFramesPackPath_Path;
                bool m_Loop_Segment_successful;
                mt::vec2f m_Loop_Segment;
                bool m_Offset_Point_successful;
                mt::vec3f m_Offset_Point;
                float m_Width_Value;
            public:
                typedef Metabuf::Vector<Meta_MovieCamera3D> VectorMeta_MovieCamera3D;
            
                const VectorMeta_MovieCamera3D & get_Includes_MovieCamera3D() const
                {
                    return this->includes_Meta_MovieCamera3D;
                }
            
            protected:
                VectorMeta_MovieCamera3D includes_Meta_MovieCamera3D;
            public:
                typedef Metabuf::Vector<Meta_MovieLayer2D> VectorMeta_MovieLayer2D;
            
                const VectorMeta_MovieLayer2D & get_Includes_MovieLayer2D() const
                {
                    return this->includes_Meta_MovieLayer2D;
                }
            
            protected:
                VectorMeta_MovieLayer2D includes_Meta_MovieLayer2D;
            public:
                typedef Metabuf::Vector<Meta_MovieLayer3D> VectorMeta_MovieLayer3D;
            
                const VectorMeta_MovieLayer3D & get_Includes_MovieLayer3D() const
                {
                    return this->includes_Meta_MovieLayer3D;
                }
            
            protected:
                VectorMeta_MovieLayer3D includes_Meta_MovieLayer3D;
            };
            
            class Meta_ResourceMovie2
                : public Meta_Resource
            { 
            public:
                Meta_ResourceMovie2();
            
            public:
                bool has_File_Dataflow() const
                {
                    return m_File_Dataflow_successful;
                }
                
                template<class C, class M>
                bool getm_File_Dataflow( C _self, M _method ) const
                {
                    if( m_File_Dataflow_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Dataflow );
                
                    return true;
                }
                
                bool get_File_Dataflow( Mengine::ConstString * _value ) const
                {
                    if( m_File_Dataflow_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Dataflow;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Composition
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Composition();
                
                public:
                    bool has_Bounds() const
                    {
                        return m_Bounds_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Bounds( C _self, M _method ) const
                    {
                        if( m_Bounds_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Bounds );
                    
                        return true;
                    }
                    
                    bool get_Bounds( mt::box2f * _value ) const
                    {
                        if( m_Bounds_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Bounds;
                    
                        return true;
                    }
                    
                    bool getd_Bounds( mt::box2f * _value, const mt::box2f & _default ) const
                    {
                        if( m_Bounds_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Bounds;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Duration( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Duration );
                    }
                    
                    float get_Duration() const
                    {
                        return this->m_Duration;
                    }
                    
                    template<class C, class M>
                    void getm_FrameDuration( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_FrameDuration );
                    }
                    
                    float get_FrameDuration() const
                    {
                        return this->m_FrameDuration;
                    }
                    
                    bool has_Master() const
                    {
                        return m_Master_successful;
                    }
                    
                    bool getd_Master( bool _default ) const
                    {
                        if( m_Master_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Master;
                    }
                    
                    bool get_Master( bool * _value ) const
                    {
                        if( m_Master_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Master;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Master( C _self, M _method ) const
                    {
                        if( m_Master_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Master );
                    
                        return true;
                    }
                    
                    bool getd_Master( bool * _value, const bool & _default ) const
                    {
                        if( m_Master_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Master;
                    
                        return true;
                    }
                    template<class C, class M>
                    void getm_Name( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Name );
                    }
                    
                    const Mengine::ConstString & get_Name() const
                    {
                        return this->m_Name;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                    class Meta_Layer
                        : public Metabuf::Metadata
                    { 
                    public:
                        Meta_Layer();
                    
                    public:
                        template<class C, class M>
                        void getm_Color( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Color );
                        }
                        
                        const Mengine::Color & get_Color() const
                        {
                            return this->m_Color;
                        }
                        
                        template<class C, class M>
                        void getm_Index( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Index );
                        }
                        
                        uint32_t get_Index() const
                        {
                            return this->m_Index;
                        }
                        
                        template<class C, class M>
                        void getm_Matrix( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Matrix );
                        }
                        
                        const mt::mat4f & get_Matrix() const
                        {
                            return this->m_Matrix;
                        }
                        
                        template<class C, class M>
                        void getm_Name( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Name );
                        }
                        
                        const Mengine::ConstString & get_Name() const
                        {
                            return this->m_Name;
                        }
                        
                        template<class C, class M>
                        void getm_Type( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Type );
                        }
                        
                        const Mengine::ConstString & get_Type() const
                        {
                            return this->m_Type;
                        }
                        
                    public:
                        bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    
                    protected:
                        void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                        void _preparationIncludes( uint32_t _id, uint32_t _count );
                        void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    
                    public:
                    protected:
                    protected:
                        Mengine::Color m_Color;
                        uint32_t m_Index;
                        mt::mat4f m_Matrix;
                        Mengine::ConstString m_Name;
                        Mengine::ConstString m_Type;
                    };
                    
                    class Meta_SubComposition
                        : public Metabuf::Metadata
                    { 
                    public:
                        Meta_SubComposition();
                    
                    public:
                        template<class C, class M>
                        void getm_Duration( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Duration );
                        }
                        
                        float get_Duration() const
                        {
                            return this->m_Duration;
                        }
                        
                        template<class C, class M>
                        void getm_FrameDuration( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_FrameDuration );
                        }
                        
                        float get_FrameDuration() const
                        {
                            return this->m_FrameDuration;
                        }
                        
                        template<class C, class M>
                        void getm_Index( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Index );
                        }
                        
                        uint32_t get_Index() const
                        {
                            return this->m_Index;
                        }
                        
                        template<class C, class M>
                        void getm_Name( C _self, M _method ) const
                        {
                            (_self->*_method)( this->m_Name );
                        }
                        
                        const Mengine::ConstString & get_Name() const
                        {
                            return this->m_Name;
                        }
                        
                    public:
                        bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    
                    protected:
                        void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                        void _preparationIncludes( uint32_t _id, uint32_t _count );
                        void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    
                    public:
                    protected:
                    protected:
                        float m_Duration;
                        float m_FrameDuration;
                        uint32_t m_Index;
                        Mengine::ConstString m_Name;
                    };
                    
                protected:
                protected:
                    bool m_Bounds_successful;
                    mt::box2f m_Bounds;
                    float m_Duration;
                    float m_FrameDuration;
                    bool m_Master_successful;
                    bool m_Master;
                    Mengine::ConstString m_Name;
                public:
                    typedef Metabuf::Vector<Meta_Layer> VectorMeta_Layer;
                
                    const VectorMeta_Layer & get_Includes_Layer() const
                    {
                        return this->includes_Meta_Layer;
                    }
                
                protected:
                    VectorMeta_Layer includes_Meta_Layer;
                public:
                    typedef Metabuf::Vector<Meta_SubComposition> VectorMeta_SubComposition;
                
                    const VectorMeta_SubComposition & get_Includes_SubComposition() const
                    {
                        return this->includes_Meta_SubComposition;
                    }
                
                protected:
                    VectorMeta_SubComposition includes_Meta_SubComposition;
                };
                
            protected:
            protected:
                bool m_File_Dataflow_successful;
                Mengine::ConstString m_File_Dataflow;
                Mengine::FilePath m_File_Path;
            public:
                typedef Metabuf::Vector<Meta_Composition> VectorMeta_Composition;
            
                const VectorMeta_Composition & get_Includes_Composition() const
                {
                    return this->includes_Meta_Composition;
                }
            
            protected:
                VectorMeta_Composition includes_Meta_Composition;
            };
            
            class Meta_ResourceMusic
                : public Meta_Resource
            { 
            public:
                Meta_ResourceMusic();
            
            public:
                bool has_DefaultVolume_Value() const
                {
                    return m_DefaultVolume_Value_successful;
                }
                
                template<class C, class M>
                bool getm_DefaultVolume_Value( C _self, M _method ) const
                {
                    if( m_DefaultVolume_Value_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_DefaultVolume_Value );
                
                    return true;
                }
                
                bool get_DefaultVolume_Value( float * _value ) const
                {
                    if( m_DefaultVolume_Value_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_DefaultVolume_Value;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return m_File_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_External() const
                {
                    return m_File_External_successful;
                }
                
                template<class C, class M>
                bool getm_File_External( C _self, M _method ) const
                {
                    if( m_File_External_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_External );
                
                    return true;
                }
                
                bool get_File_External( bool * _value ) const
                {
                    if( m_File_External_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_External;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_DefaultVolume_Value_successful;
                float m_DefaultVolume_Value;
                bool m_File_Codec_successful;
                Mengine::ConstString m_File_Codec;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                bool m_File_External_successful;
                bool m_File_External;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceParticle
                : public Meta_Resource
            { 
            public:
                Meta_ResourceParticle();
            
            public:
                template<class C, class M>
                void getm_AtlasCount_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_AtlasCount_Value );
                }
                
                uint32_t get_AtlasCount_Value() const
                {
                    return this->m_AtlasCount_Value;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Atlas
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Atlas();
                
                public:
                    template<class C, class M>
                    void getm_Index( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Index );
                    }
                    
                    uint32_t get_Index() const
                    {
                        return this->m_Index;
                    }
                    
                    template<class C, class M>
                    void getm_ResourceName( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_ResourceName );
                    }
                    
                    const Mengine::ConstString & get_ResourceName() const
                    {
                        return this->m_ResourceName;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    uint32_t m_Index;
                    Mengine::ConstString m_ResourceName;
                };
                
            protected:
            protected:
                uint32_t m_AtlasCount_Value;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                Mengine::FilePath m_File_Path;
            public:
                typedef Metabuf::Vector<Meta_Atlas> VectorMeta_Atlas;
            
                const VectorMeta_Atlas & get_Includes_Atlas() const
                {
                    return this->includes_Meta_Atlas;
                }
            
            protected:
                VectorMeta_Atlas includes_Meta_Atlas;
            };
            
            class Meta_ResourceShape
                : public Meta_Resource
            { 
            public:
                Meta_ResourceShape();
            
            public:
                template<class C, class M>
                void getm_Polygon_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Polygon_Value );
                }
                
                const Mengine::Polygon & get_Polygon_Value() const
                {
                    return this->m_Polygon_Value;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::Polygon m_Polygon_Value;
            };
            
            class Meta_ResourceSound
                : public Meta_Resource
            { 
            public:
                Meta_ResourceSound();
            
            public:
                bool has_DefaultVolume_Value() const
                {
                    return m_DefaultVolume_Value_successful;
                }
                
                template<class C, class M>
                bool getm_DefaultVolume_Value( C _self, M _method ) const
                {
                    if( m_DefaultVolume_Value_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_DefaultVolume_Value );
                
                    return true;
                }
                
                bool get_DefaultVolume_Value( float * _value ) const
                {
                    if( m_DefaultVolume_Value_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_DefaultVolume_Value;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return m_File_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
                bool has_IsStreamable_Value() const
                {
                    return m_IsStreamable_Value_successful;
                }
                
                template<class C, class M>
                bool getm_IsStreamable_Value( C _self, M _method ) const
                {
                    if( m_IsStreamable_Value_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_IsStreamable_Value );
                
                    return true;
                }
                
                bool get_IsStreamable_Value( bool * _value ) const
                {
                    if( m_IsStreamable_Value_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_IsStreamable_Value;
                
                    return true;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_DefaultVolume_Value_successful;
                float m_DefaultVolume_Value;
                bool m_File_Codec_successful;
                Mengine::ConstString m_File_Codec;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                Mengine::FilePath m_File_Path;
                bool m_IsStreamable_Value_successful;
                bool m_IsStreamable_Value;
            };
            
            class Meta_ResourceSpine
                : public Meta_Resource
            { 
            public:
                Meta_ResourceSpine();
            
            public:
                template<class C, class M>
                void getm_Atlas_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Atlas_Path );
                }
                
                const Mengine::FilePath & get_Atlas_Path() const
                {
                    return this->m_Atlas_Path;
                }
                
                template<class C, class M>
                void getm_Skeleton_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Skeleton_Path );
                }
                
                const Mengine::FilePath & get_Skeleton_Path() const
                {
                    return this->m_Skeleton_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Image
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Image();
                
                public:
                    template<class C, class M>
                    void getm_Name( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Name );
                    }
                    
                    const Mengine::ConstString & get_Name() const
                    {
                        return this->m_Name;
                    }
                    
                    template<class C, class M>
                    void getm_Resource( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Resource );
                    }
                    
                    const Mengine::ConstString & get_Resource() const
                    {
                        return this->m_Resource;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    Mengine::ConstString m_Name;
                    Mengine::ConstString m_Resource;
                };
                
            protected:
            protected:
                Mengine::FilePath m_Atlas_Path;
                Mengine::FilePath m_Skeleton_Path;
            public:
                typedef Metabuf::Vector<Meta_Image> VectorMeta_Image;
            
                const VectorMeta_Image & get_Includes_Image() const
                {
                    return this->includes_Meta_Image;
                }
            
            protected:
                VectorMeta_Image includes_Meta_Image;
            };
            
            class Meta_ResourceVideo
                : public Meta_Resource
            { 
            public:
                Meta_ResourceVideo();
            
            public:
                bool has_File_Alpha() const
                {
                    return m_File_Alpha_successful;
                }
                
                template<class C, class M>
                bool getm_File_Alpha( C _self, M _method ) const
                {
                    if( m_File_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Alpha );
                
                    return true;
                }
                
                bool get_File_Alpha( bool * _value ) const
                {
                    if( m_File_Alpha_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Alpha;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return m_File_Codec_successful;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( m_File_Codec_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return m_File_Converter_successful;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( m_File_Converter_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_Duration() const
                {
                    return m_File_Duration_successful;
                }
                
                template<class C, class M>
                bool getm_File_Duration( C _self, M _method ) const
                {
                    if( m_File_Duration_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Duration );
                
                    return true;
                }
                
                bool get_File_Duration( float * _value ) const
                {
                    if( m_File_Duration_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Duration;
                
                    return true;
                }
                
                bool has_File_FrameRate() const
                {
                    return m_File_FrameRate_successful;
                }
                
                template<class C, class M>
                bool getm_File_FrameRate( C _self, M _method ) const
                {
                    if( m_File_FrameRate_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_FrameRate );
                
                    return true;
                }
                
                bool get_File_FrameRate( float * _value ) const
                {
                    if( m_File_FrameRate_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_FrameRate;
                
                    return true;
                }
                
                bool has_File_NoSeek() const
                {
                    return m_File_NoSeek_successful;
                }
                
                template<class C, class M>
                bool getm_File_NoSeek( C _self, M _method ) const
                {
                    if( m_File_NoSeek_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoSeek );
                
                    return true;
                }
                
                bool get_File_NoSeek( bool * _value ) const
                {
                    if( m_File_NoSeek_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoSeek;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_File_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_Path );
                }
                
                const Mengine::FilePath & get_File_Path() const
                {
                    return this->m_File_Path;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_File_Alpha_successful;
                bool m_File_Alpha;
                bool m_File_Codec_successful;
                Mengine::ConstString m_File_Codec;
                bool m_File_Converter_successful;
                Mengine::ConstString m_File_Converter;
                bool m_File_Duration_successful;
                float m_File_Duration;
                bool m_File_FrameRate_successful;
                float m_File_FrameRate;
                bool m_File_NoSeek_successful;
                bool m_File_NoSeek;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceWindow
                : public Meta_Resource
            { 
            public:
                Meta_ResourceWindow();
            
            public:
                bool has_WindowBackground_ResourceImageName() const
                {
                    return m_WindowBackground_ResourceImageName_successful;
                }
                
                template<class C, class M>
                bool getm_WindowBackground_ResourceImageName( C _self, M _method ) const
                {
                    if( m_WindowBackground_ResourceImageName_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_WindowBackground_ResourceImageName );
                
                    return true;
                }
                
                bool get_WindowBackground_ResourceImageName( Mengine::ConstString * _value ) const
                {
                    if( m_WindowBackground_ResourceImageName_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_WindowBackground_ResourceImageName;
                
                    return true;
                }
                
                template<class C, class M>
                void getm_WindowBottom_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowBottom_Offset );
                }
                
                const mt::vec2f & get_WindowBottom_Offset() const
                {
                    return this->m_WindowBottom_Offset;
                }
                
                template<class C, class M>
                void getm_WindowBottom_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowBottom_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowBottom_ResourceImageName() const
                {
                    return this->m_WindowBottom_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowLeft_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowLeft_Offset );
                }
                
                const mt::vec2f & get_WindowLeft_Offset() const
                {
                    return this->m_WindowLeft_Offset;
                }
                
                template<class C, class M>
                void getm_WindowLeft_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowLeft_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowLeft_ResourceImageName() const
                {
                    return this->m_WindowLeft_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowLeftBottom_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowLeftBottom_Offset );
                }
                
                const mt::vec2f & get_WindowLeftBottom_Offset() const
                {
                    return this->m_WindowLeftBottom_Offset;
                }
                
                template<class C, class M>
                void getm_WindowLeftBottom_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowLeftBottom_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowLeftBottom_ResourceImageName() const
                {
                    return this->m_WindowLeftBottom_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowLeftTop_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowLeftTop_Offset );
                }
                
                const mt::vec2f & get_WindowLeftTop_Offset() const
                {
                    return this->m_WindowLeftTop_Offset;
                }
                
                template<class C, class M>
                void getm_WindowLeftTop_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowLeftTop_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowLeftTop_ResourceImageName() const
                {
                    return this->m_WindowLeftTop_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowRight_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowRight_Offset );
                }
                
                const mt::vec2f & get_WindowRight_Offset() const
                {
                    return this->m_WindowRight_Offset;
                }
                
                template<class C, class M>
                void getm_WindowRight_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowRight_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowRight_ResourceImageName() const
                {
                    return this->m_WindowRight_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowRightBottom_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowRightBottom_Offset );
                }
                
                const mt::vec2f & get_WindowRightBottom_Offset() const
                {
                    return this->m_WindowRightBottom_Offset;
                }
                
                template<class C, class M>
                void getm_WindowRightBottom_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowRightBottom_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowRightBottom_ResourceImageName() const
                {
                    return this->m_WindowRightBottom_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowRightTop_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowRightTop_Offset );
                }
                
                const mt::vec2f & get_WindowRightTop_Offset() const
                {
                    return this->m_WindowRightTop_Offset;
                }
                
                template<class C, class M>
                void getm_WindowRightTop_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowRightTop_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowRightTop_ResourceImageName() const
                {
                    return this->m_WindowRightTop_ResourceImageName;
                }
                
                template<class C, class M>
                void getm_WindowTop_Offset( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowTop_Offset );
                }
                
                const mt::vec2f & get_WindowTop_Offset() const
                {
                    return this->m_WindowTop_Offset;
                }
                
                template<class C, class M>
                void getm_WindowTop_ResourceImageName( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_WindowTop_ResourceImageName );
                }
                
                const Mengine::ConstString & get_WindowTop_ResourceImageName() const
                {
                    return this->m_WindowTop_ResourceImageName;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_WindowBackground_ResourceImageName_successful;
                Mengine::ConstString m_WindowBackground_ResourceImageName;
                mt::vec2f m_WindowBottom_Offset;
                Mengine::ConstString m_WindowBottom_ResourceImageName;
                mt::vec2f m_WindowLeft_Offset;
                Mengine::ConstString m_WindowLeft_ResourceImageName;
                mt::vec2f m_WindowLeftBottom_Offset;
                Mengine::ConstString m_WindowLeftBottom_ResourceImageName;
                mt::vec2f m_WindowLeftTop_Offset;
                Mengine::ConstString m_WindowLeftTop_ResourceImageName;
                mt::vec2f m_WindowRight_Offset;
                Mengine::ConstString m_WindowRight_ResourceImageName;
                mt::vec2f m_WindowRightBottom_Offset;
                Mengine::ConstString m_WindowRightBottom_ResourceImageName;
                mt::vec2f m_WindowRightTop_Offset;
                Mengine::ConstString m_WindowRightTop_ResourceImageName;
                mt::vec2f m_WindowTop_Offset;
                Mengine::ConstString m_WindowTop_ResourceImageName;
            };
            
        protected:
        protected:
            Mengine::ConstString m_Name;
        public:
            typedef Metabuf::Vector<Meta_FragmentShader> VectorMeta_FragmentShader;
        
            const VectorMeta_FragmentShader & get_Includes_FragmentShader() const
            {
                return this->includes_Meta_FragmentShader;
            }
        
        protected:
            VectorMeta_FragmentShader includes_Meta_FragmentShader;
        public:
            typedef Metabuf::Vector<Meta_Include> VectorMeta_Include;
        
            const VectorMeta_Include & get_Includes_Include() const
            {
                return this->includes_Meta_Include;
            }
        
        protected:
            VectorMeta_Include includes_Meta_Include;
        public:
            typedef Metabuf::Vector<Meta_Material> VectorMeta_Material;
        
            const VectorMeta_Material & get_Includes_Material() const
            {
                return this->includes_Meta_Material;
            }
        
        protected:
            VectorMeta_Material includes_Meta_Material;
        public:
            typedef Metabuf::Vector<Meta_Program> VectorMeta_Program;
        
            const VectorMeta_Program & get_Includes_Program() const
            {
                return this->includes_Meta_Program;
            }
        
        protected:
            VectorMeta_Program includes_Meta_Program;
        public:
            typedef Metabuf::Vector<Meta_VertexAttribute> VectorMeta_VertexAttribute;
        
            const VectorMeta_VertexAttribute & get_Includes_VertexAttribute() const
            {
                return this->includes_Meta_VertexAttribute;
            }
        
        protected:
            VectorMeta_VertexAttribute includes_Meta_VertexAttribute;
        public:
            typedef Metabuf::Vector<Meta_VertexShader> VectorMeta_VertexShader;
        
            const VectorMeta_VertexShader & get_Includes_VertexShader() const
            {
                return this->includes_Meta_VertexShader;
            }
        
        protected:
            VectorMeta_VertexShader includes_Meta_VertexShader;
        public:
            typedef Metabuf::Vector<Meta_Resource *> VectorMeta_Resource;
        
            const VectorMeta_Resource & get_Includes_Resource() const
            {
                return this->includes_Meta_Resource;
            }
        
        protected:
            VectorMeta_Resource includes_Meta_Resource;
        };
        
        class Meta_KeyFramesPack
            : public Metabuf::Metaparse
        { 
        public:
            Meta_KeyFramesPack();
        
        public:
            template<class C, class M>
            void getm_MaxIndex( C _self, M _method ) const
            {
                (_self->*_method)( this->m_MaxIndex );
            }
            
            uint32_t get_MaxIndex() const
            {
                return this->m_MaxIndex;
            }
            
            template<class C, class M>
            void getm_Version( C _self, M _method ) const
            {
                (_self->*_method)( this->m_Version );
            }
            
            uint32_t get_Version() const
            {
                return this->m_Version;
            }
            
        public:
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
        
        protected:
            void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_ImageShape
                : public Metabuf::Metadata
            { 
            public:
                Meta_ImageShape();
            
            public:
                bool has_Count() const
                {
                    return m_Count_successful;
                }
                
                uint32_t getd_Count( uint32_t _default ) const
                {
                    if( m_Count_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Count;
                }
                
                bool get_Count( uint32_t * _value ) const
                {
                    if( m_Count_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Count( C _self, M _method ) const
                {
                    if( m_Count_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Count );
                
                    return true;
                }
                
                bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                {
                    if( m_Count_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                template<class C, class M>
                void getm_ImageMaxSize( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_ImageMaxSize );
                }
                
                const mt::vec2f & get_ImageMaxSize() const
                {
                    return this->m_ImageMaxSize;
                }
                
                bool has_ImageOffset() const
                {
                    return m_ImageOffset_successful;
                }
                
                template<class C, class M>
                bool getm_ImageOffset( C _self, M _method ) const
                {
                    if( m_ImageOffset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_ImageOffset );
                
                    return true;
                }
                
                bool get_ImageOffset( mt::vec2f * _value ) const
                {
                    if( m_ImageOffset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_ImageOffset;
                
                    return true;
                }
                
                bool getd_ImageOffset( mt::vec2f * _value, const mt::vec2f & _default ) const
                {
                    if( m_ImageOffset_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_ImageOffset;
                
                    return true;
                }
                bool has_ImageSize() const
                {
                    return m_ImageSize_successful;
                }
                
                template<class C, class M>
                bool getm_ImageSize( C _self, M _method ) const
                {
                    if( m_ImageSize_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_ImageSize );
                
                    return true;
                }
                
                bool get_ImageSize( mt::vec2f * _value ) const
                {
                    if( m_ImageSize_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_ImageSize;
                
                    return true;
                }
                
                bool getd_ImageSize( mt::vec2f * _value, const mt::vec2f & _default ) const
                {
                    if( m_ImageSize_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_ImageSize;
                
                    return true;
                }
                bool has_Immutable() const
                {
                    return m_Immutable_successful;
                }
                
                bool getd_Immutable( bool _default ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Immutable;
                }
                
                bool get_Immutable( bool * _value ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Immutable( C _self, M _method ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Immutable );
                
                    return true;
                }
                
                bool getd_Immutable( bool * _value, const bool & _default ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                template<class C, class M>
                void getm_LayerIndex( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_LayerIndex );
                }
                
                uint32_t get_LayerIndex() const
                {
                    return this->m_LayerIndex;
                }
                
                bool has_Subtract() const
                {
                    return m_Subtract_successful;
                }
                
                bool getd_Subtract( bool _default ) const
                {
                    if( m_Subtract_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Subtract;
                }
                
                bool get_Subtract( bool * _value ) const
                {
                    if( m_Subtract_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Subtract;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Subtract( C _self, M _method ) const
                {
                    if( m_Subtract_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Subtract );
                
                    return true;
                }
                
                bool getd_Subtract( bool * _value, const bool & _default ) const
                {
                    if( m_Subtract_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Subtract;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Shape
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Shape();
                
                public:
                    template<class C, class M>
                    void getm_Polygon( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Polygon );
                    }
                    
                    const Mengine::Polygon & get_Polygon() const
                    {
                        return this->m_Polygon;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    Mengine::Polygon m_Polygon;
                };
                
            protected:
            protected:
                bool m_Count_successful;
                uint32_t m_Count;
                mt::vec2f m_ImageMaxSize;
                bool m_ImageOffset_successful;
                mt::vec2f m_ImageOffset;
                bool m_ImageSize_successful;
                mt::vec2f m_ImageSize;
                bool m_Immutable_successful;
                bool m_Immutable;
                uint32_t m_LayerIndex;
                bool m_Subtract_successful;
                bool m_Subtract;
            public:
                typedef Metabuf::Vector<Meta_Shape> VectorMeta_Shape;
            
                const VectorMeta_Shape & get_Includes_Shape() const
                {
                    return this->includes_Meta_Shape;
                }
            
            protected:
                VectorMeta_Shape includes_Meta_Shape;
            };
            
            class Meta_KeyFrames2D
                : public Metabuf::Metadata
            { 
            public:
                Meta_KeyFrames2D();
            
            public:
                bool has_Count() const
                {
                    return m_Count_successful;
                }
                
                uint32_t getd_Count( uint32_t _default ) const
                {
                    if( m_Count_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Count;
                }
                
                bool get_Count( uint32_t * _value ) const
                {
                    if( m_Count_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Count( C _self, M _method ) const
                {
                    if( m_Count_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Count );
                
                    return true;
                }
                
                bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                {
                    if( m_Count_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                bool has_Immutable() const
                {
                    return m_Immutable_successful;
                }
                
                bool getd_Immutable( bool _default ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Immutable;
                }
                
                bool get_Immutable( bool * _value ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Immutable( C _self, M _method ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Immutable );
                
                    return true;
                }
                
                bool getd_Immutable( bool * _value, const bool & _default ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                template<class C, class M>
                void getm_LayerIndex( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_LayerIndex );
                }
                
                uint32_t get_LayerIndex() const
                {
                    return this->m_LayerIndex;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_KeyFrame2D
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_KeyFrame2D();
                
                public:
                    bool has_AnchorPoint() const
                    {
                        return m_AnchorPoint_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec2f * _value ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec2f * _value, const mt::vec2f & _default ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_Count() const
                    {
                        return m_Count_successful;
                    }
                    
                    uint32_t getd_Count( uint32_t _default ) const
                    {
                        if( m_Count_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Count;
                    }
                    
                    bool get_Count( uint32_t * _value ) const
                    {
                        if( m_Count_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Count( C _self, M _method ) const
                    {
                        if( m_Count_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Count );
                    
                        return true;
                    }
                    
                    bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_Count_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    bool has_Opacity() const
                    {
                        return m_Opacity_successful;
                    }
                    
                    float getd_Opacity( float _default ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Opacity;
                    }
                    
                    bool get_Opacity( float * _value ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Opacity( C _self, M _method ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Opacity );
                    
                        return true;
                    }
                    
                    bool getd_Opacity( float * _value, const float & _default ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return m_Position_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec2f * _value ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec2f * _value, const mt::vec2f & _default ) const
                    {
                        if( m_Position_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return m_Rotation_successful;
                    }
                    
                    float getd_Rotation( float _default ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Rotation;
                    }
                    
                    bool get_Rotation( float * _value ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool getd_Rotation( float * _value, const float & _default ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return m_Scale_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec2f * _value ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec2f * _value, const mt::vec2f & _default ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Volume() const
                    {
                        return m_Volume_successful;
                    }
                    
                    float getd_Volume( float _default ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Volume;
                    }
                    
                    bool get_Volume( float * _value ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Volume( C _self, M _method ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Volume );
                    
                        return true;
                    }
                    
                    bool getd_Volume( float * _value, const float & _default ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    bool m_AnchorPoint_successful;
                    mt::vec2f m_AnchorPoint;
                    bool m_Count_successful;
                    uint32_t m_Count;
                    bool m_Opacity_successful;
                    float m_Opacity;
                    bool m_Position_successful;
                    mt::vec2f m_Position;
                    bool m_Rotation_successful;
                    float m_Rotation;
                    bool m_Scale_successful;
                    mt::vec2f m_Scale;
                    bool m_Volume_successful;
                    float m_Volume;
                };
                
            protected:
            protected:
                bool m_Count_successful;
                uint32_t m_Count;
                bool m_Immutable_successful;
                bool m_Immutable;
                uint32_t m_LayerIndex;
            public:
                typedef Metabuf::Vector<Meta_KeyFrame2D> VectorMeta_KeyFrame2D;
            
                const VectorMeta_KeyFrame2D & get_Includes_KeyFrame2D() const
                {
                    return this->includes_Meta_KeyFrame2D;
                }
            
            protected:
                VectorMeta_KeyFrame2D includes_Meta_KeyFrame2D;
            };
            
            class Meta_KeyFrames3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_KeyFrames3D();
            
            public:
                bool has_Count() const
                {
                    return m_Count_successful;
                }
                
                uint32_t getd_Count( uint32_t _default ) const
                {
                    if( m_Count_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Count;
                }
                
                bool get_Count( uint32_t * _value ) const
                {
                    if( m_Count_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Count( C _self, M _method ) const
                {
                    if( m_Count_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Count );
                
                    return true;
                }
                
                bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                {
                    if( m_Count_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                bool has_Immutable() const
                {
                    return m_Immutable_successful;
                }
                
                bool getd_Immutable( bool _default ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Immutable;
                }
                
                bool get_Immutable( bool * _value ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Immutable( C _self, M _method ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Immutable );
                
                    return true;
                }
                
                bool getd_Immutable( bool * _value, const bool & _default ) const
                {
                    if( m_Immutable_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                template<class C, class M>
                void getm_LayerIndex( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_LayerIndex );
                }
                
                uint32_t get_LayerIndex() const
                {
                    return this->m_LayerIndex;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_KeyFrame3D
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_KeyFrame3D();
                
                public:
                    bool has_AnchorPoint() const
                    {
                        return m_AnchorPoint_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec3f * _value ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_AnchorPoint_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_Count() const
                    {
                        return m_Count_successful;
                    }
                    
                    uint32_t getd_Count( uint32_t _default ) const
                    {
                        if( m_Count_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Count;
                    }
                    
                    bool get_Count( uint32_t * _value ) const
                    {
                        if( m_Count_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Count( C _self, M _method ) const
                    {
                        if( m_Count_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Count );
                    
                        return true;
                    }
                    
                    bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( m_Count_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    bool has_Opacity() const
                    {
                        return m_Opacity_successful;
                    }
                    
                    float getd_Opacity( float _default ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Opacity;
                    }
                    
                    bool get_Opacity( float * _value ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Opacity( C _self, M _method ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Opacity );
                    
                        return true;
                    }
                    
                    bool getd_Opacity( float * _value, const float & _default ) const
                    {
                        if( m_Opacity_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    bool has_Orientation() const
                    {
                        return m_Orientation_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Orientation( C _self, M _method ) const
                    {
                        if( m_Orientation_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Orientation );
                    
                        return true;
                    }
                    
                    bool get_Orientation( mt::vec3f * _value ) const
                    {
                        if( m_Orientation_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Orientation;
                    
                        return true;
                    }
                    
                    bool getd_Orientation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Orientation_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Orientation;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return m_Position_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec3f * _value ) const
                    {
                        if( m_Position_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Position_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return m_Rotation_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool get_Rotation( mt::vec3f * _value ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    bool getd_Rotation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Rotation_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return m_Scale_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec3f * _value ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( m_Scale_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Volume() const
                    {
                        return m_Volume_successful;
                    }
                    
                    float getd_Volume( float _default ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            return _default;
                        }
                    
                        return this->m_Volume;
                    }
                    
                    bool get_Volume( float * _value ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Volume( C _self, M _method ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Volume );
                    
                        return true;
                    }
                    
                    bool getd_Volume( float * _value, const float & _default ) const
                    {
                        if( m_Volume_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    bool m_AnchorPoint_successful;
                    mt::vec3f m_AnchorPoint;
                    bool m_Count_successful;
                    uint32_t m_Count;
                    bool m_Opacity_successful;
                    float m_Opacity;
                    bool m_Orientation_successful;
                    mt::vec3f m_Orientation;
                    bool m_Position_successful;
                    mt::vec3f m_Position;
                    bool m_Rotation_successful;
                    mt::vec3f m_Rotation;
                    bool m_Scale_successful;
                    mt::vec3f m_Scale;
                    bool m_Volume_successful;
                    float m_Volume;
                };
                
            protected:
            protected:
                bool m_Count_successful;
                uint32_t m_Count;
                bool m_Immutable_successful;
                bool m_Immutable;
                uint32_t m_LayerIndex;
            public:
                typedef Metabuf::Vector<Meta_KeyFrame3D> VectorMeta_KeyFrame3D;
            
                const VectorMeta_KeyFrame3D & get_Includes_KeyFrame3D() const
                {
                    return this->includes_Meta_KeyFrame3D;
                }
            
            protected:
                VectorMeta_KeyFrame3D includes_Meta_KeyFrame3D;
            };
            
            class Meta_Polygon
                : public Metabuf::Metadata
            { 
            public:
                Meta_Polygon();
            
            public:
                template<class C, class M>
                void getm_LayerIndex( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_LayerIndex );
                }
                
                uint32_t get_LayerIndex() const
                {
                    return this->m_LayerIndex;
                }
                
                template<class C, class M>
                void getm_Value( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Value );
                }
                
                const Mengine::Polygon & get_Value() const
                {
                    return this->m_Value;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                uint32_t m_LayerIndex;
                Mengine::Polygon m_Value;
            };
            
            class Meta_TimeRemap
                : public Metabuf::Metadata
            { 
            public:
                Meta_TimeRemap();
            
            public:
                template<class C, class M>
                void getm_LayerIndex( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_LayerIndex );
                }
                
                uint32_t get_LayerIndex() const
                {
                    return this->m_LayerIndex;
                }
                
                template<class C, class M>
                void getm_Time( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Time );
                }
                
                const Mengine::Floats & get_Time() const
                {
                    return this->m_Time;
                }
                
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                uint32_t m_LayerIndex;
                Mengine::Floats m_Time;
            };
            
        protected:
        protected:
            uint32_t m_MaxIndex;
            uint32_t m_Version;
        public:
            typedef Metabuf::Vector<Meta_ImageShape> VectorMeta_ImageShape;
        
            const VectorMeta_ImageShape & get_Includes_ImageShape() const
            {
                return this->includes_Meta_ImageShape;
            }
        
        protected:
            VectorMeta_ImageShape includes_Meta_ImageShape;
        public:
            typedef Metabuf::Vector<Meta_KeyFrames2D> VectorMeta_KeyFrames2D;
        
            const VectorMeta_KeyFrames2D & get_Includes_KeyFrames2D() const
            {
                return this->includes_Meta_KeyFrames2D;
            }
        
        protected:
            VectorMeta_KeyFrames2D includes_Meta_KeyFrames2D;
        public:
            typedef Metabuf::Vector<Meta_KeyFrames3D> VectorMeta_KeyFrames3D;
        
            const VectorMeta_KeyFrames3D & get_Includes_KeyFrames3D() const
            {
                return this->includes_Meta_KeyFrames3D;
            }
        
        protected:
            VectorMeta_KeyFrames3D includes_Meta_KeyFrames3D;
        public:
            typedef Metabuf::Vector<Meta_Polygon> VectorMeta_Polygon;
        
            const VectorMeta_Polygon & get_Includes_Polygon() const
            {
                return this->includes_Meta_Polygon;
            }
        
        protected:
            VectorMeta_Polygon includes_Meta_Polygon;
        public:
            typedef Metabuf::Vector<Meta_TimeRemap> VectorMeta_TimeRemap;
        
            const VectorMeta_TimeRemap & get_Includes_TimeRemap() const
            {
                return this->includes_Meta_TimeRemap;
            }
        
        protected:
            VectorMeta_TimeRemap includes_Meta_TimeRemap;
        };
        
        class Meta_Pak
            : public Metabuf::Metaparse
        { 
        public:
            Meta_Pak();
        
        public:
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
        
        protected:
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_Datas
                : public Metabuf::Metadata
            { 
            public:
                Meta_Datas();
            
            public:
                bool has_Platform() const
                {
                    return m_Platform_successful;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( m_Platform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Data
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Data();
                
                public:
                    template<class C, class M>
                    void getm_Name( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Name );
                    }
                    
                    const Mengine::ConstString & get_Name() const
                    {
                        return this->m_Name;
                    }
                    
                    template<class C, class M>
                    void getm_Path( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Path );
                    }
                    
                    const Mengine::FilePath & get_Path() const
                    {
                        return this->m_Path;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    Mengine::ConstString m_Name;
                    Mengine::FilePath m_Path;
                };
                
            protected:
            protected:
                bool m_Platform_successful;
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Data> VectorMeta_Data;
            
                const VectorMeta_Data & get_Includes_Data() const
                {
                    return this->includes_Meta_Data;
                }
            
            protected:
                VectorMeta_Data includes_Meta_Data;
            };
            
            class Meta_Fonts
                : public Metabuf::Metadata
            { 
            public:
                Meta_Fonts();
            
            public:
                template<class C, class M>
                void getm_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Path );
                }
                
                const Mengine::FilePath & get_Path() const
                {
                    return this->m_Path;
                }
                
                bool has_Platform() const
                {
                    return m_Platform_successful;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( m_Platform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                Mengine::FilePath m_Path;
                bool m_Platform_successful;
                Mengine::Tags m_Platform;
            };
            
            class Meta_Materials
                : public Metabuf::Metadata
            { 
            public:
                Meta_Materials();
            
            public:
                bool has_Platform() const
                {
                    return m_Platform_successful;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( m_Platform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Material
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Material();
                
                public:
                    template<class C, class M>
                    void getm_Path( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Path );
                    }
                    
                    const Mengine::FilePath & get_Path() const
                    {
                        return this->m_Path;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    Mengine::FilePath m_Path;
                };
                
            protected:
            protected:
                bool m_Platform_successful;
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Material> VectorMeta_Material;
            
                const VectorMeta_Material & get_Includes_Material() const
                {
                    return this->includes_Meta_Material;
                }
            
            protected:
                VectorMeta_Material includes_Meta_Material;
            };
            
            class Meta_Resources
                : public Metabuf::Metadata
            { 
            public:
                Meta_Resources();
            
            public:
                bool has_Demand() const
                {
                    return m_Demand_successful;
                }
                
                bool getd_Demand( bool _default ) const
                {
                    if( m_Demand_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Demand;
                }
                
                bool get_Demand( bool * _value ) const
                {
                    if( m_Demand_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Demand;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Demand( C _self, M _method ) const
                {
                    if( m_Demand_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Demand );
                
                    return true;
                }
                
                bool getd_Demand( bool * _value, const bool & _default ) const
                {
                    if( m_Demand_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Demand;
                
                    return true;
                }
                bool has_Ignored() const
                {
                    return m_Ignored_successful;
                }
                
                bool getd_Ignored( bool _default ) const
                {
                    if( m_Ignored_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_Ignored;
                }
                
                bool get_Ignored( bool * _value ) const
                {
                    if( m_Ignored_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Ignored;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Ignored( C _self, M _method ) const
                {
                    if( m_Ignored_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Ignored );
                
                    return true;
                }
                
                bool getd_Ignored( bool * _value, const bool & _default ) const
                {
                    if( m_Ignored_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Ignored;
                
                    return true;
                }
                bool has_Platform() const
                {
                    return m_Platform_successful;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( m_Platform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Resource
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Resource();
                
                public:
                    template<class C, class M>
                    void getm_Path( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Path );
                    }
                    
                    const Mengine::FilePath & get_Path() const
                    {
                        return this->m_Path;
                    }
                    
                    bool has_Tags() const
                    {
                        return m_Tags_successful;
                    }
                    
                    template<class C, class M>
                    bool getm_Tags( C _self, M _method ) const
                    {
                        if( m_Tags_successful == false )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Tags );
                    
                        return true;
                    }
                    
                    bool get_Tags( Mengine::Tags * _value ) const
                    {
                        if( m_Tags_successful == false )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Tags;
                    
                        return true;
                    }
                    
                    bool getd_Tags( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                    {
                        if( m_Tags_successful == false )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Tags;
                    
                        return true;
                    }
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    Mengine::FilePath m_Path;
                    bool m_Tags_successful;
                    Mengine::Tags m_Tags;
                };
                
            protected:
            protected:
                bool m_Demand_successful;
                bool m_Demand;
                bool m_Ignored_successful;
                bool m_Ignored;
                bool m_Platform_successful;
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Resource> VectorMeta_Resource;
            
                const VectorMeta_Resource & get_Includes_Resource() const
                {
                    return this->includes_Meta_Resource;
                }
            
            protected:
                VectorMeta_Resource includes_Meta_Resource;
            };
            
            class Meta_Scripts
                : public Metabuf::Metadata
            { 
            public:
                Meta_Scripts();
            
            public:
                bool has_Finalizer() const
                {
                    return m_Finalizer_successful;
                }
                
                template<class C, class M>
                bool getm_Finalizer( C _self, M _method ) const
                {
                    if( m_Finalizer_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Finalizer );
                
                    return true;
                }
                
                bool get_Finalizer( Mengine::ConstString * _value ) const
                {
                    if( m_Finalizer_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Finalizer;
                
                    return true;
                }
                
                bool getd_Finalizer( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( m_Finalizer_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Finalizer;
                
                    return true;
                }
                bool has_Initializer() const
                {
                    return m_Initializer_successful;
                }
                
                template<class C, class M>
                bool getm_Initializer( C _self, M _method ) const
                {
                    if( m_Initializer_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Initializer );
                
                    return true;
                }
                
                bool get_Initializer( Mengine::ConstString * _value ) const
                {
                    if( m_Initializer_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Initializer;
                
                    return true;
                }
                
                bool getd_Initializer( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( m_Initializer_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Initializer;
                
                    return true;
                }
                bool has_Module() const
                {
                    return m_Module_successful;
                }
                
                template<class C, class M>
                bool getm_Module( C _self, M _method ) const
                {
                    if( m_Module_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Module );
                
                    return true;
                }
                
                bool get_Module( Mengine::ConstString * _value ) const
                {
                    if( m_Module_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Module;
                
                    return true;
                }
                
                bool getd_Module( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( m_Module_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Module;
                
                    return true;
                }
                template<class C, class M>
                void getm_Path( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Path );
                }
                
                const Mengine::FilePath & get_Path() const
                {
                    return this->m_Path;
                }
                
                bool has_Platform() const
                {
                    return m_Platform_successful;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( m_Platform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_Finalizer_successful;
                Mengine::ConstString m_Finalizer;
                bool m_Initializer_successful;
                Mengine::ConstString m_Initializer;
                bool m_Module_successful;
                Mengine::ConstString m_Module;
                Mengine::FilePath m_Path;
                bool m_Platform_successful;
                Mengine::Tags m_Platform;
            };
            
            class Meta_Texts
                : public Metabuf::Metadata
            { 
            public:
                Meta_Texts();
            
            public:
                bool has_Platform() const
                {
                    return m_Platform_successful;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( m_Platform_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( m_Platform_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Text
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Text();
                
                public:
                    template<class C, class M>
                    void getm_Path( C _self, M _method ) const
                    {
                        (_self->*_method)( this->m_Path );
                    }
                    
                    const Mengine::FilePath & get_Path() const
                    {
                        return this->m_Path;
                    }
                    
                public:
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                
                protected:
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                protected:
                    Mengine::FilePath m_Path;
                };
                
            protected:
            protected:
                bool m_Platform_successful;
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Text> VectorMeta_Text;
            
                const VectorMeta_Text & get_Includes_Text() const
                {
                    return this->includes_Meta_Text;
                }
            
            protected:
                VectorMeta_Text includes_Meta_Text;
            };
            
        protected:
        protected:
        public:
            typedef Metabuf::Vector<Meta_Datas> VectorMeta_Datas;
        
            const VectorMeta_Datas & get_Includes_Datas() const
            {
                return this->includes_Meta_Datas;
            }
        
        protected:
            VectorMeta_Datas includes_Meta_Datas;
        public:
            typedef Metabuf::Vector<Meta_Fonts> VectorMeta_Fonts;
        
            const VectorMeta_Fonts & get_Includes_Fonts() const
            {
                return this->includes_Meta_Fonts;
            }
        
        protected:
            VectorMeta_Fonts includes_Meta_Fonts;
        public:
            typedef Metabuf::Vector<Meta_Materials> VectorMeta_Materials;
        
            const VectorMeta_Materials & get_Includes_Materials() const
            {
                return this->includes_Meta_Materials;
            }
        
        protected:
            VectorMeta_Materials includes_Meta_Materials;
        public:
            typedef Metabuf::Vector<Meta_Resources> VectorMeta_Resources;
        
            const VectorMeta_Resources & get_Includes_Resources() const
            {
                return this->includes_Meta_Resources;
            }
        
        protected:
            VectorMeta_Resources includes_Meta_Resources;
        public:
            typedef Metabuf::Vector<Meta_Scripts> VectorMeta_Scripts;
        
            const VectorMeta_Scripts & get_Includes_Scripts() const
            {
                return this->includes_Meta_Scripts;
            }
        
        protected:
            VectorMeta_Scripts includes_Meta_Scripts;
        public:
            typedef Metabuf::Vector<Meta_Texts> VectorMeta_Texts;
        
            const VectorMeta_Texts & get_Includes_Texts() const
            {
                return this->includes_Meta_Texts;
            }
        
        protected:
            VectorMeta_Texts includes_Meta_Texts;
        };
        
        class Meta_Texts
            : public Metabuf::Metaparse
        { 
        public:
            Meta_Texts();
        
        public:
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
        
        protected:
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_Text
                : public Metabuf::Metadata
            { 
            public:
                Meta_Text();
            
            public:
                bool has_CharOffset() const
                {
                    return m_CharOffset_successful;
                }
                
                float getd_CharOffset( float _default ) const
                {
                    if( m_CharOffset_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_CharOffset;
                }
                
                bool get_CharOffset( float * _value ) const
                {
                    if( m_CharOffset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_CharOffset;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_CharOffset( C _self, M _method ) const
                {
                    if( m_CharOffset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_CharOffset );
                
                    return true;
                }
                
                bool getd_CharOffset( float * _value, const float & _default ) const
                {
                    if( m_CharOffset_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_CharOffset;
                
                    return true;
                }
                bool has_Font() const
                {
                    return m_Font_successful;
                }
                
                template<class C, class M>
                bool getm_Font( C _self, M _method ) const
                {
                    if( m_Font_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Font );
                
                    return true;
                }
                
                bool get_Font( Mengine::ConstString * _value ) const
                {
                    if( m_Font_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Font;
                
                    return true;
                }
                
                bool getd_Font( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( m_Font_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Font;
                
                    return true;
                }
                template<class C, class M>
                void getm_Key( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Key );
                }
                
                const Mengine::ConstString & get_Key() const
                {
                    return this->m_Key;
                }
                
                bool has_LineOffset() const
                {
                    return m_LineOffset_successful;
                }
                
                float getd_LineOffset( float _default ) const
                {
                    if( m_LineOffset_successful == false )
                    {
                        return _default;
                    }
                
                    return this->m_LineOffset;
                }
                
                bool get_LineOffset( float * _value ) const
                {
                    if( m_LineOffset_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_LineOffset;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_LineOffset( C _self, M _method ) const
                {
                    if( m_LineOffset_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_LineOffset );
                
                    return true;
                }
                
                bool getd_LineOffset( float * _value, const float & _default ) const
                {
                    if( m_LineOffset_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_LineOffset;
                
                    return true;
                }
                bool has_Value() const
                {
                    return m_Value_successful;
                }
                
                template<class C, class M>
                bool getm_Value( C _self, M _method ) const
                {
                    if( m_Value_successful == false )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Value );
                
                    return true;
                }
                
                bool get_Value( Mengine::WString * _value ) const
                {
                    if( m_Value_successful == false )
                    {
                        return false;
                    }
                
                    *_value = this->m_Value;
                
                    return true;
                }
                
                bool getd_Value( Mengine::WString * _value, const Mengine::WString & _default ) const
                {
                    if( m_Value_successful == false )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Value;
                
                    return true;
                }
            public:
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            
            protected:
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            protected:
                bool m_CharOffset_successful;
                float m_CharOffset;
                bool m_Font_successful;
                Mengine::ConstString m_Font;
                Mengine::ConstString m_Key;
                bool m_LineOffset_successful;
                float m_LineOffset;
                bool m_Value_successful;
                Mengine::WString m_Value;
            };
            
        protected:
        protected:
        public:
            typedef Metabuf::Vector<Meta_Text> VectorMeta_Text;
        
            const VectorMeta_Text & get_Includes_Text() const
            {
                return this->includes_Meta_Text;
            }
        
        protected:
            VectorMeta_Text includes_Meta_Text;
        };
        
    }
}
