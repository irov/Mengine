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

    uint32_t getInternalStringsCount();
    const char * getInternalString( uint32_t _index, uint32_t & _stringSize, int64_t & _stringHash );
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
            // cppcheck-suppress duplInheritedMember
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
        
        protected:
            // cppcheck-suppress duplInheritedMember
            void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            // cppcheck-suppress duplInheritedMember
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            // cppcheck-suppress duplInheritedMember
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            void _parseGenerators( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_FragmentShader
                : public Metabuf::Metaparse
            { 
            public:
                Meta_FragmentShader();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Compile = (1 <<1),
                    EMETA_File_Converter = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
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
                    return (m_flagNoRequiredAttribute & EMETA_File_Compile) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Compile( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Compile) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Compile );
                
                    return true;
                }
                
                bool get_File_Compile( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Compile) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Compile;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                bool m_File_Compile;
                Mengine::ConstString m_File_Converter;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_Include
                : public Metabuf::Metaparse
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::FilePath m_Path;
            };
            
            class Meta_Material
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Material();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Debug = (1 <<1),
                    EMETA_RenderPlatform = (1 <<0),
                    EMETA_AlphaBlend_Enable = (1 <<9),
                    EMETA_BlendFactor_Dest = (1 <<3),
                    EMETA_BlendFactor_Op = (1 <<4),
                    EMETA_BlendFactor_Source = (1 <<2),
                    EMETA_DepthBufferTest_Enable = (1 <<10),
                    EMETA_DepthBufferWrite_Enable = (1 <<11),
                    EMETA_Program_Name = (1 <<12),
                    EMETA_SeparateAlphaBlend_Enable = (1 <<8),
                    EMETA_SeparateAlphaBlendFactor_Dest = (1 <<6),
                    EMETA_SeparateAlphaBlendFactor_Op = (1 <<7),
                    EMETA_SeparateAlphaBlendFactor_Source = (1 <<5),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Debug() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Debug) != 0;
                }
                
                bool getd_Debug( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Debug) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Debug;
                }
                
                bool get_Debug( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Debug) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Debug;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Debug( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Debug) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Debug );
                
                    return true;
                }
                
                bool getd_Debug( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Debug) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_RenderPlatform) != 0;
                }
                
                template<class C, class M>
                bool getm_RenderPlatform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_RenderPlatform );
                
                    return true;
                }
                
                bool get_RenderPlatform( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                
                bool getd_RenderPlatform( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                bool has_AlphaBlend_Enable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_AlphaBlend_Enable) != 0;
                }
                
                template<class C, class M>
                void getm_AlphaBlend_Enable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_AlphaBlend_Enable) == 0 )
                    {
                        (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_AlphaBlend_Enable );
                    }
                }
                
                bool get_AlphaBlend_Enable() const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_AlphaBlend_Enable) == 0 )
                    {
                        return false;
                    }
                
                    return this->m_AlphaBlend_Enable;
                }
                
                bool has_BlendFactor_Dest() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_BlendFactor_Dest) != 0;
                }
                
                template<class C, class M>
                bool getm_BlendFactor_Dest( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Dest) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_BlendFactor_Dest );
                
                    return true;
                }
                
                bool get_BlendFactor_Dest( Mengine::EBlendFactor * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Dest) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_BlendFactor_Dest;
                
                    return true;
                }
                
                bool has_BlendFactor_Op() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_BlendFactor_Op) != 0;
                }
                
                template<class C, class M>
                bool getm_BlendFactor_Op( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Op) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_BlendFactor_Op );
                
                    return true;
                }
                
                bool get_BlendFactor_Op( Mengine::EBlendOp * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Op) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_BlendFactor_Op;
                
                    return true;
                }
                
                bool has_BlendFactor_Source() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_BlendFactor_Source) != 0;
                }
                
                template<class C, class M>
                bool getm_BlendFactor_Source( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Source) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_BlendFactor_Source );
                
                    return true;
                }
                
                bool get_BlendFactor_Source( Mengine::EBlendFactor * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Source) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_BlendFactor_Source;
                
                    return true;
                }
                
                bool has_DepthBufferTest_Enable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_DepthBufferTest_Enable) != 0;
                }
                
                template<class C, class M>
                void getm_DepthBufferTest_Enable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DepthBufferTest_Enable) == 0 )
                    {
                        (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_DepthBufferTest_Enable );
                    }
                }
                
                bool get_DepthBufferTest_Enable() const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DepthBufferTest_Enable) == 0 )
                    {
                        return false;
                    }
                
                    return this->m_DepthBufferTest_Enable;
                }
                
                bool has_DepthBufferWrite_Enable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_DepthBufferWrite_Enable) != 0;
                }
                
                template<class C, class M>
                void getm_DepthBufferWrite_Enable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DepthBufferWrite_Enable) == 0 )
                    {
                        (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_DepthBufferWrite_Enable );
                    }
                }
                
                bool get_DepthBufferWrite_Enable() const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DepthBufferWrite_Enable) == 0 )
                    {
                        return false;
                    }
                
                    return this->m_DepthBufferWrite_Enable;
                }
                
                bool has_Program_Name() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Program_Name) != 0;
                }
                
                template<class C, class M>
                bool getm_Program_Name( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Program_Name) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Program_Name );
                
                    return true;
                }
                
                bool get_Program_Name( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Program_Name) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Program_Name;
                
                    return true;
                }
                
                bool has_SeparateAlphaBlend_Enable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlend_Enable) != 0;
                }
                
                template<class C, class M>
                void getm_SeparateAlphaBlend_Enable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlend_Enable) == 0 )
                    {
                        (_self->*_method)( false );
                    }
                    else
                    {
                        (_self->*_method)( this->m_SeparateAlphaBlend_Enable );
                    }
                }
                
                bool get_SeparateAlphaBlend_Enable() const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlend_Enable) == 0 )
                    {
                        return false;
                    }
                
                    return this->m_SeparateAlphaBlend_Enable;
                }
                
                bool has_SeparateAlphaBlendFactor_Dest() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Dest) != 0;
                }
                
                template<class C, class M>
                bool getm_SeparateAlphaBlendFactor_Dest( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Dest) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_SeparateAlphaBlendFactor_Dest );
                
                    return true;
                }
                
                bool get_SeparateAlphaBlendFactor_Dest( Mengine::EBlendFactor * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Dest) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_SeparateAlphaBlendFactor_Dest;
                
                    return true;
                }
                
                bool has_SeparateAlphaBlendFactor_Op() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Op) != 0;
                }
                
                template<class C, class M>
                bool getm_SeparateAlphaBlendFactor_Op( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Op) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_SeparateAlphaBlendFactor_Op );
                
                    return true;
                }
                
                bool get_SeparateAlphaBlendFactor_Op( Mengine::EBlendOp * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Op) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_SeparateAlphaBlendFactor_Op;
                
                    return true;
                }
                
                bool has_SeparateAlphaBlendFactor_Source() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Source) != 0;
                }
                
                template<class C, class M>
                bool getm_SeparateAlphaBlendFactor_Source( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Source) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_SeparateAlphaBlendFactor_Source );
                
                    return true;
                }
                
                bool get_SeparateAlphaBlendFactor_Source( Mengine::EBlendFactor * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Source) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_SeparateAlphaBlendFactor_Source;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_TextureStages
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_TextureStages();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_AddressMode_Border = (1 <<2),
                        EMETA_AddressMode_U = (1 <<0),
                        EMETA_AddressMode_V = (1 <<1),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
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
                        return (m_flagNoRequiredAttribute & EMETA_AddressMode_Border) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AddressMode_Border( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AddressMode_Border) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AddressMode_Border );
                    
                        return true;
                    }
                    
                    bool get_AddressMode_Border( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AddressMode_Border) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AddressMode_Border;
                    
                        return true;
                    }
                    
                    bool has_AddressMode_U() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_AddressMode_U) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AddressMode_U( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AddressMode_U) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AddressMode_U );
                    
                        return true;
                    }
                    
                    bool get_AddressMode_U( Mengine::ETextureAddressMode * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AddressMode_U) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AddressMode_U;
                    
                        return true;
                    }
                    
                    bool has_AddressMode_V() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_AddressMode_V) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AddressMode_V( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AddressMode_V) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AddressMode_V );
                    
                        return true;
                    }
                    
                    bool get_AddressMode_V( Mengine::ETextureAddressMode * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AddressMode_V) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AddressMode_V;
                    
                        return true;
                    }
                    
                public:
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    uint32_t m_Stage;
                    uint32_t m_AddressMode_Border;
                    Mengine::ETextureAddressMode m_AddressMode_U;
                    Mengine::ETextureAddressMode m_AddressMode_V;
                };
                
            protected:
                bool m_Debug;
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                bool m_AlphaBlend_Enable;
                Mengine::EBlendFactor m_BlendFactor_Dest;
                Mengine::EBlendOp m_BlendFactor_Op;
                Mengine::EBlendFactor m_BlendFactor_Source;
                bool m_DepthBufferTest_Enable;
                bool m_DepthBufferWrite_Enable;
                Mengine::ConstString m_Program_Name;
                bool m_SeparateAlphaBlend_Enable;
                Mengine::EBlendFactor m_SeparateAlphaBlendFactor_Dest;
                Mengine::EBlendOp m_SeparateAlphaBlendFactor_Op;
                Mengine::EBlendFactor m_SeparateAlphaBlendFactor_Source;
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Program();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_RenderPlatform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
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
                    return (m_flagNoRequiredAttribute & EMETA_RenderPlatform) != 0;
                }
                
                template<class C, class M>
                bool getm_RenderPlatform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_RenderPlatform );
                
                    return true;
                }
                
                bool get_RenderPlatform( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_RenderPlatform;
                
                    return true;
                }
                
                bool getd_RenderPlatform( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                Mengine::ConstString m_FragmentShader_Name;
                uint32_t m_Sampler_Count;
                Mengine::ConstString m_VertexAttribute_Name;
                Mengine::ConstString m_VertexShader_Name;
            };
            
            class Meta_VertexAttribute
                : public Metabuf::Metaparse
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Attribute
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Attribute();
                
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    uint32_t m_Index;
                    bool m_Normalized;
                    uint32_t m_Offset;
                    uint32_t m_Size;
                    uint32_t m_Stride;
                    Mengine::EVertexAttributeType m_Type;
                    Mengine::ConstString m_Uniform;
                };
                
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_VertexShader();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Compile = (1 <<1),
                    EMETA_File_Converter = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
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
                    return (m_flagNoRequiredAttribute & EMETA_File_Compile) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Compile( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Compile) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Compile );
                
                    return true;
                }
                
                bool get_File_Compile( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Compile) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Compile;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Name;
                Mengine::ConstString m_RenderPlatform;
                bool m_File_Compile;
                Mengine::ConstString m_File_Converter;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_Resource
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Resource();
                virtual ~Meta_Resource();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Precompile = (1 <<1),
                    EMETA_Unique = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
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
                    return (m_flagNoRequiredAttribute & EMETA_Precompile) != 0;
                }
                
                bool getd_Precompile( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Precompile) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Precompile;
                }
                
                bool get_Precompile( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Precompile) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Precompile;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Precompile( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Precompile) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Precompile );
                
                    return true;
                }
                
                bool getd_Precompile( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Precompile) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Unique) != 0;
                }
                
                bool getd_Unique( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Unique) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Unique;
                }
                
                bool get_Unique( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Unique) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Unique;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Unique( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Unique) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Unique );
                
                    return true;
                }
                
                bool getd_Unique( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Unique) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Unique;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Name;
                bool m_Precompile;
                Mengine::ConstString m_Type;
                bool m_Unique;
            };
            
            class Meta_ResourceAstralax
                : public Meta_Resource
            { 
            public:
                Meta_ResourceAstralax();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Converter = (1 <<2),
                    EMETA_File_NoExist = (1 <<3),
                };
                
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
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Atlas
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    uint32_t m_Index;
                    Mengine::ConstString m_ResourceName;
                };
                
            protected:
                uint32_t m_AtlasCount_Value;
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
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
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_NoExist = (1 <<2),
                };
                
            public:
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCal3dMesh
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCal3dMesh();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_NoExist = (1 <<2),
                };
                
            public:
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCal3dSkeleton
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCal3dSkeleton();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_NoExist = (1 <<2),
                };
                
            public:
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCursorICO
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCursorICO();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_NoExist = (1 <<2),
                };
                
            public:
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceCursorSystem
                : public Meta_Resource
            { 
            public:
                Meta_ResourceCursorSystem();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_NoExist = (1 <<2),
                };
                
            public:
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceExternal
                : public Meta_Resource
            { 
            public:
                Meta_ResourceExternal();
            
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
            };
            
            class Meta_ResourceFile
                : public Meta_Resource
            { 
            public:
                Meta_ResourceFile();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_NoExist = (1 <<2),
                };
                
            public:
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceHIT
                : public Meta_Resource
            { 
            public:
                Meta_ResourceHIT();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Codec = (1 <<2),
                    EMETA_File_Converter = (1 <<3),
                    EMETA_File_NoExist = (1 <<4),
                };
                
            public:
                bool has_File_Codec() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_File_Codec;
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceImageData
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageData();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Alpha = (1 <<3),
                    EMETA_File_Codec = (1 <<2),
                    EMETA_File_NoExist = (1 <<7),
                    EMETA_File_Offset = (1 <<6),
                    EMETA_File_Premultiply = (1 <<4),
                    EMETA_File_Size = (1 <<5),
                    EMETA_File_TrimAtlas = (1 <<8),
                };
                
            public:
                bool has_File_Alpha() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Alpha) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Alpha( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Alpha );
                
                    return true;
                }
                
                bool get_File_Alpha( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Alpha;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
                    return true;
                }
                
                bool has_File_Offset() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Offset) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Offset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Offset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Offset );
                
                    return true;
                }
                
                bool get_File_Offset( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Offset) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_File_Premultiply) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Premultiply( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Premultiply );
                
                    return true;
                }
                
                bool get_File_Premultiply( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Premultiply;
                
                    return true;
                }
                
                bool has_File_Size() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Size) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Size( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Size) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Size );
                
                    return true;
                }
                
                bool get_File_Size( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Size) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Size;
                
                    return true;
                }
                
                bool has_File_TrimAtlas() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) != 0;
                }
                
                template<class C, class M>
                bool getm_File_TrimAtlas( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_TrimAtlas );
                
                    return true;
                }
                
                bool get_File_TrimAtlas( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_TrimAtlas;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_Alpha;
                Mengine::ConstString m_File_Codec;
                mt::vec2f m_File_MaxSize;
                bool m_File_NoExist;
                mt::vec2f m_File_Offset;
                Mengine::FilePath m_File_Path;
                bool m_File_Premultiply;
                mt::vec2f m_File_Size;
                bool m_File_TrimAtlas;
            };
            
            class Meta_ResourceImageDefault
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageDefault();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Alpha = (1 <<4),
                    EMETA_File_Codec = (1 <<2),
                    EMETA_File_Converter = (1 <<3),
                    EMETA_File_NoExist = (1 <<8),
                    EMETA_File_Offset = (1 <<7),
                    EMETA_File_Premultiply = (1 <<5),
                    EMETA_File_Size = (1 <<6),
                    EMETA_File_TrimAtlas = (1 <<9),
                };
                
            public:
                bool has_File_Alpha() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Alpha) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Alpha( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Alpha );
                
                    return true;
                }
                
                bool get_File_Alpha( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Alpha;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
                    return true;
                }
                
                bool has_File_Offset() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Offset) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Offset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Offset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Offset );
                
                    return true;
                }
                
                bool get_File_Offset( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Offset) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_File_Premultiply) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Premultiply( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Premultiply );
                
                    return true;
                }
                
                bool get_File_Premultiply( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Premultiply;
                
                    return true;
                }
                
                bool has_File_Size() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Size) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Size( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Size) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Size );
                
                    return true;
                }
                
                bool get_File_Size( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Size) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Size;
                
                    return true;
                }
                
                bool has_File_TrimAtlas() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) != 0;
                }
                
                template<class C, class M>
                bool getm_File_TrimAtlas( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_TrimAtlas );
                
                    return true;
                }
                
                bool get_File_TrimAtlas( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_TrimAtlas;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_Alpha;
                Mengine::ConstString m_File_Codec;
                Mengine::ConstString m_File_Converter;
                mt::vec2f m_File_MaxSize;
                bool m_File_NoExist;
                mt::vec2f m_File_Offset;
                Mengine::FilePath m_File_Path;
                bool m_File_Premultiply;
                mt::vec2f m_File_Size;
                bool m_File_TrimAtlas;
            };
            
            class Meta_ResourceImageEmpty
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageEmpty();
            
            public:
                template<class C, class M>
                void getm_File_MaxSize( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_File_MaxSize );
                }
                
                const mt::vec2f & get_File_MaxSize() const
                {
                    return this->m_File_MaxSize;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                mt::vec2f m_File_MaxSize;
            };
            
            class Meta_ResourceImageSequence
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSequence();
            
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Sequence
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    float m_Delay;
                    Mengine::ConstString m_ResourceImageName;
                };
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::Color m_Color_Value;
                mt::vec2f m_Size_Value;
            };
            
            class Meta_ResourceImageSubstract
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSubstract();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Image_Alpha = (1 <<3),
                    EMETA_Image_Offset = (1 <<6),
                    EMETA_Image_Premultiply = (1 <<4),
                    EMETA_Image_Size = (1 <<5),
                    EMETA_Image_UVRotate = (1 <<2),
                };
                
            public:
                bool has_Image_Alpha() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Image_Alpha) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Alpha( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Alpha );
                
                    return true;
                }
                
                bool get_Image_Alpha( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Alpha) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Image_Offset) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Offset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Offset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Offset );
                
                    return true;
                }
                
                bool get_Image_Offset( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Offset) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Offset;
                
                    return true;
                }
                
                bool has_Image_Premultiply() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Premultiply( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Premultiply );
                
                    return true;
                }
                
                bool get_Image_Premultiply( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Premultiply;
                
                    return true;
                }
                
                bool has_Image_Size() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Image_Size) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Size( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Size) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Size );
                
                    return true;
                }
                
                bool get_Image_Size( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Size) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Image_UVRotate) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_UVRotate( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_UVRotate) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_UVRotate );
                
                    return true;
                }
                
                bool get_Image_UVRotate( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_UVRotate) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_UVRotate;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_Image_Alpha;
                mt::vec2f m_Image_MaxSize;
                Mengine::ConstString m_Image_Name;
                mt::vec2f m_Image_Offset;
                bool m_Image_Premultiply;
                mt::vec2f m_Image_Size;
                mt::uv4f m_Image_UV;
                bool m_Image_UVRotate;
            };
            
            class Meta_ResourceImageSubstractRGBAndAlpha
                : public Meta_Resource
            { 
            public:
                Meta_ResourceImageSubstractRGBAndAlpha();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Image_Offset = (1 <<6),
                    EMETA_Image_Premultiply = (1 <<4),
                    EMETA_Image_Size = (1 <<5),
                    EMETA_Image_UVAlphaRotate = (1 <<3),
                    EMETA_Image_UVRGBRotate = (1 <<2),
                };
                
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
                    return (m_flagNoRequiredAttribute & EMETA_Image_Offset) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Offset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Offset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Offset );
                
                    return true;
                }
                
                bool get_Image_Offset( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Offset) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Offset;
                
                    return true;
                }
                
                bool has_Image_Premultiply() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Premultiply( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Premultiply );
                
                    return true;
                }
                
                bool get_Image_Premultiply( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_Premultiply;
                
                    return true;
                }
                
                bool has_Image_Size() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Image_Size) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_Size( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Size) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_Size );
                
                    return true;
                }
                
                bool get_Image_Size( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_Size) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Image_UVAlphaRotate) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_UVAlphaRotate( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_UVAlphaRotate) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_UVAlphaRotate );
                
                    return true;
                }
                
                bool get_Image_UVAlphaRotate( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_UVAlphaRotate) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Image_UVRGBRotate) != 0;
                }
                
                template<class C, class M>
                bool getm_Image_UVRGBRotate( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_UVRGBRotate) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Image_UVRGBRotate );
                
                    return true;
                }
                
                bool get_Image_UVRGBRotate( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Image_UVRGBRotate) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Image_UVRGBRotate;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                mt::vec2f m_Image_MaxSize;
                Mengine::ConstString m_Image_NameAlpha;
                Mengine::ConstString m_Image_NameRGB;
                mt::vec2f m_Image_Offset;
                bool m_Image_Premultiply;
                mt::vec2f m_Image_Size;
                mt::uv4f m_Image_UVAlpha;
                bool m_Image_UVAlphaRotate;
                mt::uv4f m_Image_UVRGB;
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Internal_Group;
                Mengine::ConstString m_Internal_Name;
            };
            
            class Meta_ResourceJSON
                : public Meta_Resource
            { 
            public:
                Meta_ResourceJSON();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Converter = (1 <<2),
                    EMETA_File_NoExist = (1 <<3),
                };
                
            public:
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceModel3D
                : public Meta_Resource
            { 
            public:
                Meta_ResourceModel3D();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Converter = (1 <<3),
                    EMETA_File_Dataflow = (1 <<2),
                    EMETA_File_NoExist = (1 <<4),
                };
                
            public:
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_Dataflow() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Dataflow) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Dataflow( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Dataflow) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Dataflow );
                
                    return true;
                }
                
                bool get_File_Dataflow( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Dataflow) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Dataflow;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_File_Converter;
                Mengine::ConstString m_File_Dataflow;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
                Mengine::ConstString m_Image_Resource;
            };
            
            class Meta_ResourceMovie
                : public Meta_Resource
            { 
            public:
                Meta_ResourceMovie();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Anchor_Point = (1 <<5),
                    EMETA_Bounds_Box = (1 <<7),
                    EMETA_KeyFramesPackPath_Codec = (1 <<2),
                    EMETA_KeyFramesPackPath_Converter = (1 <<3),
                    EMETA_Loop_Segment = (1 <<4),
                    EMETA_Offset_Point = (1 <<6),
                };
                
            public:
                bool has_Anchor_Point() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Anchor_Point) != 0;
                }
                
                template<class C, class M>
                bool getm_Anchor_Point( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Anchor_Point) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Anchor_Point );
                
                    return true;
                }
                
                bool get_Anchor_Point( mt::vec3f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Anchor_Point) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Anchor_Point;
                
                    return true;
                }
                
                bool has_Bounds_Box() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Bounds_Box) != 0;
                }
                
                template<class C, class M>
                bool getm_Bounds_Box( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Bounds_Box) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Bounds_Box );
                
                    return true;
                }
                
                bool get_Bounds_Box( mt::box2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Bounds_Box) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_KeyFramesPackPath_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_KeyFramesPackPath_Codec );
                
                    return true;
                }
                
                bool get_KeyFramesPackPath_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Codec) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_KeyFramesPackPath_Codec;
                
                    return true;
                }
                
                bool has_KeyFramesPackPath_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_KeyFramesPackPath_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_KeyFramesPackPath_Converter );
                
                    return true;
                }
                
                bool get_KeyFramesPackPath_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Converter) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Loop_Segment) != 0;
                }
                
                template<class C, class M>
                bool getm_Loop_Segment( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Loop_Segment) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Loop_Segment );
                
                    return true;
                }
                
                bool get_Loop_Segment( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Loop_Segment) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Loop_Segment;
                
                    return true;
                }
                
                bool has_Offset_Point() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Offset_Point) != 0;
                }
                
                template<class C, class M>
                bool getm_Offset_Point( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Offset_Point) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Offset_Point );
                
                    return true;
                }
                
                bool get_Offset_Point( mt::vec3f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Offset_Point) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_MovieCamera3D
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    float m_CameraAspect;
                    float m_CameraFOV;
                    mt::vec3f m_CameraInterest;
                    mt::vec3f m_CameraPosition;
                    float m_Height;
                    float m_Width;
                };
                
                class Meta_MovieLayer2D
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_MovieLayer2D();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_AnchorPoint = (1 <<10),
                        EMETA_BlendingMode = (1 <<2),
                        EMETA_Loop = (1 <<9),
                        EMETA_Params = (1 <<15),
                        EMETA_Parent = (1 <<0),
                        EMETA_PlayCount = (1 <<6),
                        EMETA_Polygon = (1 <<5),
                        EMETA_Position = (1 <<11),
                        EMETA_Rotation = (1 <<13),
                        EMETA_Scale = (1 <<12),
                        EMETA_Shape = (1 <<4),
                        EMETA_StartInterval = (1 <<1),
                        EMETA_Stretch = (1 <<7),
                        EMETA_Switch = (1 <<8),
                        EMETA_TimeRemap = (1 <<3),
                        EMETA_Viewport = (1 <<14),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
                public:
                    bool has_AnchorPoint() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_BlendingMode() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_BlendingMode) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_BlendingMode( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_BlendingMode );
                    
                        return true;
                    }
                    
                    bool get_BlendingMode( Mengine::ConstString * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_BlendingMode;
                    
                        return true;
                    }
                    
                    bool getd_BlendingMode( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_Loop) != 0;
                    }
                    
                    bool getd_Loop( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Loop;
                    }
                    
                    bool get_Loop( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Loop;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Loop( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Loop );
                    
                        return true;
                    }
                    
                    bool getd_Loop( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_Params) != 0;
                    }
                    
                    uint32_t getd_Params( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Params;
                    }
                    
                    bool get_Params( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Params( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Params );
                    
                        return true;
                    }
                    
                    bool getd_Params( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    bool has_Parent() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Parent) != 0;
                    }
                    
                    uint32_t getd_Parent( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Parent;
                    }
                    
                    bool get_Parent( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Parent( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Parent );
                    
                        return true;
                    }
                    
                    bool getd_Parent( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    bool has_PlayCount() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_PlayCount) != 0;
                    }
                    
                    uint32_t getd_PlayCount( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_PlayCount;
                    }
                    
                    bool get_PlayCount( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_PlayCount( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_PlayCount );
                    
                        return true;
                    }
                    
                    bool getd_PlayCount( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    bool has_Polygon() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Polygon) != 0;
                    }
                    
                    bool getd_Polygon( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Polygon;
                    }
                    
                    bool get_Polygon( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Polygon( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Polygon );
                    
                        return true;
                    }
                    
                    bool getd_Polygon( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Position) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Rotation) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool get_Rotation( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    bool getd_Rotation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Scale) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Shape() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Shape) != 0;
                    }
                    
                    bool getd_Shape( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Shape;
                    }
                    
                    bool get_Shape( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Shape;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Shape( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Shape );
                    
                        return true;
                    }
                    
                    bool getd_Shape( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_StartInterval) != 0;
                    }
                    
                    float getd_StartInterval( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_StartInterval;
                    }
                    
                    bool get_StartInterval( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_StartInterval( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_StartInterval );
                    
                        return true;
                    }
                    
                    bool getd_StartInterval( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    bool has_Stretch() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Stretch) != 0;
                    }
                    
                    float getd_Stretch( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Stretch;
                    }
                    
                    bool get_Stretch( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Stretch( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Stretch );
                    
                        return true;
                    }
                    
                    bool getd_Stretch( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    bool has_Switch() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Switch) != 0;
                    }
                    
                    bool getd_Switch( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Switch;
                    }
                    
                    bool get_Switch( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Switch( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Switch );
                    
                        return true;
                    }
                    
                    bool getd_Switch( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    bool has_TimeRemap() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_TimeRemap) != 0;
                    }
                    
                    bool getd_TimeRemap( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_TimeRemap;
                    }
                    
                    bool get_TimeRemap( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_TimeRemap;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_TimeRemap( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_TimeRemap );
                    
                        return true;
                    }
                    
                    bool getd_TimeRemap( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_Viewport) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Viewport( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Viewport) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Viewport );
                    
                        return true;
                    }
                    
                    bool get_Viewport( Mengine::Viewport * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Viewport) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Viewport;
                    
                        return true;
                    }
                    
                    bool getd_Viewport( Mengine::Viewport * _value, const Mengine::Viewport & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Viewport) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Viewport;
                    
                        return true;
                    }
                public:
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    mt::vec3f m_AnchorPoint;
                    Mengine::ConstString m_BlendingMode;
                    float m_In;
                    uint32_t m_Index;
                    bool m_Loop;
                    Mengine::ConstString m_Name;
                    float m_Out;
                    uint32_t m_Params;
                    uint32_t m_Parent;
                    uint32_t m_PlayCount;
                    bool m_Polygon;
                    mt::vec3f m_Position;
                    mt::vec3f m_Rotation;
                    mt::vec3f m_Scale;
                    bool m_Shape;
                    Mengine::ConstString m_Source;
                    float m_StartInterval;
                    float m_Stretch;
                    bool m_Switch;
                    bool m_TimeRemap;
                    Mengine::ConstString m_Type;
                    Mengine::Viewport m_Viewport;
                };
                
                class Meta_MovieLayer3D
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_MovieLayer3D();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_AnchorPoint = (1 <<10),
                        EMETA_BlendingMode = (1 <<2),
                        EMETA_Loop = (1 <<9),
                        EMETA_Params = (1 <<14),
                        EMETA_Parent = (1 <<0),
                        EMETA_PlayCount = (1 <<6),
                        EMETA_Polygon = (1 <<5),
                        EMETA_Position = (1 <<11),
                        EMETA_Rotation = (1 <<13),
                        EMETA_Scale = (1 <<12),
                        EMETA_Shape = (1 <<4),
                        EMETA_StartInterval = (1 <<1),
                        EMETA_Stretch = (1 <<7),
                        EMETA_Switch = (1 <<8),
                        EMETA_TimeRemap = (1 <<3),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
                public:
                    bool has_AnchorPoint() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_BlendingMode() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_BlendingMode) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_BlendingMode( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_BlendingMode );
                    
                        return true;
                    }
                    
                    bool get_BlendingMode( Mengine::ConstString * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_BlendingMode;
                    
                        return true;
                    }
                    
                    bool getd_BlendingMode( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_Loop) != 0;
                    }
                    
                    bool getd_Loop( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Loop;
                    }
                    
                    bool get_Loop( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Loop;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Loop( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Loop );
                    
                        return true;
                    }
                    
                    bool getd_Loop( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Loop) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_Params) != 0;
                    }
                    
                    uint32_t getd_Params( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Params;
                    }
                    
                    bool get_Params( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Params( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Params );
                    
                        return true;
                    }
                    
                    bool getd_Params( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Params) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Params;
                    
                        return true;
                    }
                    bool has_Parent() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Parent) != 0;
                    }
                    
                    uint32_t getd_Parent( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Parent;
                    }
                    
                    bool get_Parent( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Parent( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Parent );
                    
                        return true;
                    }
                    
                    bool getd_Parent( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Parent) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Parent;
                    
                        return true;
                    }
                    bool has_PlayCount() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_PlayCount) != 0;
                    }
                    
                    uint32_t getd_PlayCount( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_PlayCount;
                    }
                    
                    bool get_PlayCount( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_PlayCount( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_PlayCount );
                    
                        return true;
                    }
                    
                    bool getd_PlayCount( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_PlayCount) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_PlayCount;
                    
                        return true;
                    }
                    bool has_Polygon() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Polygon) != 0;
                    }
                    
                    bool getd_Polygon( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Polygon;
                    }
                    
                    bool get_Polygon( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Polygon( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Polygon );
                    
                        return true;
                    }
                    
                    bool getd_Polygon( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Polygon) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Polygon;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Position) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Rotation) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool get_Rotation( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    bool getd_Rotation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Scale) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Shape() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Shape) != 0;
                    }
                    
                    bool getd_Shape( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Shape;
                    }
                    
                    bool get_Shape( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Shape;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Shape( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Shape );
                    
                        return true;
                    }
                    
                    bool getd_Shape( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Shape) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_StartInterval) != 0;
                    }
                    
                    float getd_StartInterval( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_StartInterval;
                    }
                    
                    bool get_StartInterval( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_StartInterval( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_StartInterval );
                    
                        return true;
                    }
                    
                    bool getd_StartInterval( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_StartInterval) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_StartInterval;
                    
                        return true;
                    }
                    bool has_Stretch() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Stretch) != 0;
                    }
                    
                    float getd_Stretch( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Stretch;
                    }
                    
                    bool get_Stretch( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Stretch( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Stretch );
                    
                        return true;
                    }
                    
                    bool getd_Stretch( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Stretch) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Stretch;
                    
                        return true;
                    }
                    bool has_Switch() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Switch) != 0;
                    }
                    
                    bool getd_Switch( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Switch;
                    }
                    
                    bool get_Switch( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Switch( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Switch );
                    
                        return true;
                    }
                    
                    bool getd_Switch( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Switch) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Switch;
                    
                        return true;
                    }
                    bool has_TimeRemap() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_TimeRemap) != 0;
                    }
                    
                    bool getd_TimeRemap( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_TimeRemap;
                    }
                    
                    bool get_TimeRemap( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_TimeRemap;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_TimeRemap( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_TimeRemap );
                    
                        return true;
                    }
                    
                    bool getd_TimeRemap( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) == 0 )
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    mt::vec3f m_AnchorPoint;
                    Mengine::ConstString m_BlendingMode;
                    float m_In;
                    uint32_t m_Index;
                    bool m_Loop;
                    Mengine::ConstString m_Name;
                    float m_Out;
                    uint32_t m_Params;
                    uint32_t m_Parent;
                    uint32_t m_PlayCount;
                    bool m_Polygon;
                    mt::vec3f m_Position;
                    mt::vec3f m_Rotation;
                    mt::vec3f m_Scale;
                    bool m_Shape;
                    Mengine::ConstString m_Source;
                    float m_StartInterval;
                    float m_Stretch;
                    bool m_Switch;
                    bool m_TimeRemap;
                    Mengine::ConstString m_Type;
                };
                
            protected:
                mt::vec3f m_Anchor_Point;
                mt::box2f m_Bounds_Box;
                float m_Duration_Value;
                float m_FrameDuration_Value;
                float m_Height_Value;
                Mengine::ConstString m_KeyFramesPackPath_Codec;
                Mengine::ConstString m_KeyFramesPackPath_Converter;
                Mengine::FilePath m_KeyFramesPackPath_Path;
                mt::vec2f m_Loop_Segment;
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
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Dataflow = (1 <<2),
                    EMETA_File_NoExist = (1 <<3),
                };
                
            public:
                bool has_File_Dataflow() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Dataflow) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Dataflow( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Dataflow) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Dataflow );
                
                    return true;
                }
                
                bool get_File_Dataflow( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Dataflow) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Dataflow;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Composition
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Composition();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_Bounds = (1 <<0),
                        EMETA_Master = (1 <<1),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
                public:
                    bool has_Bounds() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Bounds) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Bounds( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Bounds) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Bounds );
                    
                        return true;
                    }
                    
                    bool get_Bounds( mt::box2f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Bounds) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Bounds;
                    
                        return true;
                    }
                    
                    bool getd_Bounds( mt::box2f * _value, const mt::box2f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Bounds) == 0 )
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
                        return (m_flagNoRequiredAttribute & EMETA_Master) != 0;
                    }
                    
                    bool getd_Master( bool _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Master) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Master;
                    }
                    
                    bool get_Master( bool * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Master) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Master;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Master( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Master) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Master );
                    
                        return true;
                    }
                    
                    bool getd_Master( bool * _value, const bool & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Master) == 0 )
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                    class Meta_Layer
                        : public Metabuf::Metaparse
                    { 
                    public:
                        Meta_Layer();
                    
                    protected:
                        enum NoRequiredAttribute
                        {
                            EMETA_Color = (1 <<1),
                            EMETA_Dimension = (1 <<3),
                            EMETA_Matrix = (1 <<0),
                            EMETA_Options = (1 <<2),
                        };
                        
                        uint32_t m_flagNoRequiredAttribute;
                        
                    public:
                        bool has_Color() const
                        {
                            return (m_flagNoRequiredAttribute & EMETA_Color) != 0;
                        }
                        
                        template<class C, class M>
                        bool getm_Color( C _self, M _method ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Color) == 0 )
                            {
                                return false;
                            }
                        
                            (_self->*_method)( this->m_Color );
                        
                            return true;
                        }
                        
                        bool get_Color( Mengine::Color * _value ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Color) == 0 )
                            {
                                return false;
                            }
                        
                            *_value = this->m_Color;
                        
                            return true;
                        }
                        
                        bool getd_Color( Mengine::Color * _value, const Mengine::Color & _default ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Color) == 0 )
                            {
                                *_value = _default;
                        
                                return false;
                            }
                        
                            *_value = this->m_Color;
                        
                            return true;
                        }
                        bool has_Dimension() const
                        {
                            return (m_flagNoRequiredAttribute & EMETA_Dimension) != 0;
                        }
                        
                        template<class C, class M>
                        bool getm_Dimension( C _self, M _method ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Dimension) == 0 )
                            {
                                return false;
                            }
                        
                            (_self->*_method)( this->m_Dimension );
                        
                            return true;
                        }
                        
                        bool get_Dimension( mt::box2f * _value ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Dimension) == 0 )
                            {
                                return false;
                            }
                        
                            *_value = this->m_Dimension;
                        
                            return true;
                        }
                        
                        bool getd_Dimension( mt::box2f * _value, const mt::box2f & _default ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Dimension) == 0 )
                            {
                                *_value = _default;
                        
                                return false;
                            }
                        
                            *_value = this->m_Dimension;
                        
                            return true;
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
                        
                        bool has_Matrix() const
                        {
                            return (m_flagNoRequiredAttribute & EMETA_Matrix) != 0;
                        }
                        
                        template<class C, class M>
                        bool getm_Matrix( C _self, M _method ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Matrix) == 0 )
                            {
                                return false;
                            }
                        
                            (_self->*_method)( this->m_Matrix );
                        
                            return true;
                        }
                        
                        bool get_Matrix( mt::mat4f * _value ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Matrix) == 0 )
                            {
                                return false;
                            }
                        
                            *_value = this->m_Matrix;
                        
                            return true;
                        }
                        
                        bool getd_Matrix( mt::mat4f * _value, const mt::mat4f & _default ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Matrix) == 0 )
                            {
                                *_value = _default;
                        
                                return false;
                            }
                        
                            *_value = this->m_Matrix;
                        
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
                        
                        bool has_Options() const
                        {
                            return (m_flagNoRequiredAttribute & EMETA_Options) != 0;
                        }
                        
                        template<class C, class M>
                        bool getm_Options( C _self, M _method ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Options) == 0 )
                            {
                                return false;
                            }
                        
                            (_self->*_method)( this->m_Options );
                        
                            return true;
                        }
                        
                        bool get_Options( Mengine::UInt32s * _value ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Options) == 0 )
                            {
                                return false;
                            }
                        
                            *_value = this->m_Options;
                        
                            return true;
                        }
                        
                        bool getd_Options( Mengine::UInt32s * _value, const Mengine::UInt32s & _default ) const
                        {
                            if( (m_flagNoRequiredAttribute & EMETA_Options) == 0 )
                            {
                                *_value = _default;
                        
                                return false;
                            }
                        
                            *_value = this->m_Options;
                        
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
                        // cppcheck-suppress duplInheritedMember
                        bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                    
                    protected:
                        // cppcheck-suppress duplInheritedMember
                        void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                        // cppcheck-suppress duplInheritedMember
                        void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                        // cppcheck-suppress duplInheritedMember
                        void _preparationIncludes( uint32_t _id, uint32_t _count );
                        // cppcheck-suppress duplInheritedMember
                        void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    
                    public:
                    protected:
                        Mengine::Color m_Color;
                        mt::box2f m_Dimension;
                        uint32_t m_Index;
                        mt::mat4f m_Matrix;
                        Mengine::ConstString m_Name;
                        Mengine::UInt32s m_Options;
                        Mengine::ConstString m_Type;
                    };
                    
                    class Meta_SubComposition
                        : public Metabuf::Metaparse
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
                        // cppcheck-suppress duplInheritedMember
                        bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                    
                    protected:
                        // cppcheck-suppress duplInheritedMember
                        void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                        // cppcheck-suppress duplInheritedMember
                        void _preparationIncludes( uint32_t _id, uint32_t _count );
                        // cppcheck-suppress duplInheritedMember
                        void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                    
                    public:
                    protected:
                        float m_Duration;
                        float m_FrameDuration;
                        uint32_t m_Index;
                        Mengine::ConstString m_Name;
                    };
                    
                protected:
                    mt::box2f m_Bounds;
                    float m_Duration;
                    float m_FrameDuration;
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
                Mengine::ConstString m_File_Dataflow;
                bool m_File_NoExist;
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
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_DefaultVolume_Value = (1 <<6),
                    EMETA_File_Codec = (1 <<2),
                    EMETA_File_Converter = (1 <<3),
                    EMETA_File_External = (1 <<4),
                    EMETA_File_NoExist = (1 <<5),
                };
                
            public:
                bool has_DefaultVolume_Value() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) != 0;
                }
                
                template<class C, class M>
                bool getm_DefaultVolume_Value( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_DefaultVolume_Value );
                
                    return true;
                }
                
                bool get_DefaultVolume_Value( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_DefaultVolume_Value;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_External() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_External) != 0;
                }
                
                template<class C, class M>
                bool getm_File_External( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_External) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_External );
                
                    return true;
                }
                
                bool get_File_External( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_External) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_External;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                float m_DefaultVolume_Value;
                Mengine::ConstString m_File_Codec;
                Mengine::ConstString m_File_Converter;
                bool m_File_External;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceParticle
                : public Meta_Resource
            { 
            public:
                Meta_ResourceParticle();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Converter = (1 <<2),
                    EMETA_File_NoExist = (1 <<3),
                };
                
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
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Atlas
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    uint32_t m_Index;
                    Mengine::ConstString m_ResourceName;
                };
                
            protected:
                uint32_t m_AtlasCount_Value;
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::Polygon m_Polygon_Value;
            };
            
            class Meta_ResourceSound
                : public Meta_Resource
            { 
            public:
                Meta_ResourceSound();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_DefaultVolume_Value = (1 <<5),
                    EMETA_File_Codec = (1 <<2),
                    EMETA_File_Converter = (1 <<3),
                    EMETA_File_NoExist = (1 <<4),
                    EMETA_IsStreamable_Value = (1 <<6),
                };
                
            public:
                bool has_DefaultVolume_Value() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) != 0;
                }
                
                template<class C, class M>
                bool getm_DefaultVolume_Value( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_DefaultVolume_Value );
                
                    return true;
                }
                
                bool get_DefaultVolume_Value( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_DefaultVolume_Value;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                    return (m_flagNoRequiredAttribute & EMETA_IsStreamable_Value) != 0;
                }
                
                template<class C, class M>
                bool getm_IsStreamable_Value( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_IsStreamable_Value) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_IsStreamable_Value );
                
                    return true;
                }
                
                bool get_IsStreamable_Value( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_IsStreamable_Value) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_IsStreamable_Value;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                float m_DefaultVolume_Value;
                Mengine::ConstString m_File_Codec;
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
                bool m_IsStreamable_Value;
            };
            
            class Meta_ResourceSpineAtlas
                : public Meta_Resource
            { 
            public:
                Meta_ResourceSpineAtlas();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Converter = (1 <<2),
                    EMETA_File_NoExist = (1 <<3),
                };
                
            public:
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Image
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::ConstString m_Name;
                    Mengine::ConstString m_Resource;
                };
                
            protected:
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            public:
                typedef Metabuf::Vector<Meta_Image> VectorMeta_Image;
            
                const VectorMeta_Image & get_Includes_Image() const
                {
                    return this->includes_Meta_Image;
                }
            
            protected:
                VectorMeta_Image includes_Meta_Image;
            };
            
            class Meta_ResourceSpineAtlasTexturepacker
                : public Meta_Resource
            { 
            public:
                Meta_ResourceSpineAtlasTexturepacker();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Texturepacker_Name = (1 <<2),
                };
                
            public:
                bool has_Texturepacker_Name() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Texturepacker_Name) != 0;
                }
                
                template<class C, class M>
                bool getm_Texturepacker_Name( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Texturepacker_Name) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Texturepacker_Name );
                
                    return true;
                }
                
                bool get_Texturepacker_Name( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Texturepacker_Name) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Texturepacker_Name;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Texturepacker_Name;
            };
            
            class Meta_ResourceSpineSkeleton
                : public Meta_Resource
            { 
            public:
                Meta_ResourceSpineSkeleton();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Atlas_Name = (1 <<4),
                    EMETA_File_Converter = (1 <<2),
                    EMETA_File_NoExist = (1 <<3),
                };
                
            public:
                bool has_Atlas_Name() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Atlas_Name) != 0;
                }
                
                template<class C, class M>
                bool getm_Atlas_Name( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Atlas_Name) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Atlas_Name );
                
                    return true;
                }
                
                bool get_Atlas_Name( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Atlas_Name) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Atlas_Name;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Atlas_Name;
                Mengine::ConstString m_File_Converter;
                bool m_File_NoExist;
                Mengine::FilePath m_File_Path;
            };
            
            class Meta_ResourceTexturepacker
                : public Meta_Resource
            { 
            public:
                Meta_ResourceTexturepacker();
            
            public:
                template<class C, class M>
                void getm_Image_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_Image_Name );
                }
                
                const Mengine::ConstString & get_Image_Name() const
                {
                    return this->m_Image_Name;
                }
                
                template<class C, class M>
                void getm_JSON_Name( C _self, M _method ) const
                {
                    (_self->*_method)( this->m_JSON_Name );
                }
                
                const Mengine::ConstString & get_JSON_Name() const
                {
                    return this->m_JSON_Name;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                Mengine::ConstString m_Image_Name;
                Mengine::ConstString m_JSON_Name;
            };
            
            class Meta_ResourceVideo
                : public Meta_Resource
            { 
            public:
                Meta_ResourceVideo();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_File_Alpha = (1 <<2),
                    EMETA_File_Codec = (1 <<4),
                    EMETA_File_Converter = (1 <<5),
                    EMETA_File_Duration = (1 <<8),
                    EMETA_File_FrameRate = (1 <<7),
                    EMETA_File_NoExist = (1 <<10),
                    EMETA_File_NoSeek = (1 <<6),
                    EMETA_File_Premultiply = (1 <<3),
                    EMETA_File_Resize = (1 <<9),
                };
                
            public:
                bool has_File_Alpha() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Alpha) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Alpha( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Alpha );
                
                    return true;
                }
                
                bool get_File_Alpha( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Alpha;
                
                    return true;
                }
                
                bool has_File_Codec() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Codec( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Codec );
                
                    return true;
                }
                
                bool get_File_Codec( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Codec) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Codec;
                
                    return true;
                }
                
                bool has_File_Converter() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Converter( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Converter );
                
                    return true;
                }
                
                bool get_File_Converter( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Converter) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Converter;
                
                    return true;
                }
                
                bool has_File_Duration() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Duration) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Duration( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Duration) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Duration );
                
                    return true;
                }
                
                bool get_File_Duration( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Duration) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Duration;
                
                    return true;
                }
                
                bool has_File_FrameRate() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_FrameRate) != 0;
                }
                
                template<class C, class M>
                bool getm_File_FrameRate( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_FrameRate) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_FrameRate );
                
                    return true;
                }
                
                bool get_File_FrameRate( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_FrameRate) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_FrameRate;
                
                    return true;
                }
                
                bool has_File_NoExist() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoExist( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoExist );
                
                    return true;
                }
                
                bool get_File_NoExist( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_NoExist;
                
                    return true;
                }
                
                bool has_File_NoSeek() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_NoSeek) != 0;
                }
                
                template<class C, class M>
                bool getm_File_NoSeek( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoSeek) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_NoSeek );
                
                    return true;
                }
                
                bool get_File_NoSeek( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_NoSeek) == 0 )
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
                
                bool has_File_Premultiply() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Premultiply) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Premultiply( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Premultiply );
                
                    return true;
                }
                
                bool get_File_Premultiply( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Premultiply;
                
                    return true;
                }
                
                bool has_File_Resize() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_File_Resize) != 0;
                }
                
                template<class C, class M>
                bool getm_File_Resize( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Resize) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_File_Resize );
                
                    return true;
                }
                
                bool get_File_Resize( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_File_Resize) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_File_Resize;
                
                    return true;
                }
                
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                bool m_File_Alpha;
                Mengine::ConstString m_File_Codec;
                Mengine::ConstString m_File_Converter;
                float m_File_Duration;
                float m_File_FrameRate;
                bool m_File_NoExist;
                bool m_File_NoSeek;
                Mengine::FilePath m_File_Path;
                bool m_File_Premultiply;
                float m_File_Resize;
            };
            
            class Meta_ResourceWindow
                : public Meta_Resource
            { 
            public:
                Meta_ResourceWindow();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_WindowBackground_ResourceImageName = (1 <<2),
                };
                
            public:
                bool has_WindowBackground_ResourceImageName() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_WindowBackground_ResourceImageName) != 0;
                }
                
                template<class C, class M>
                bool getm_WindowBackground_ResourceImageName( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_WindowBackground_ResourceImageName) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_WindowBackground_ResourceImageName );
                
                    return true;
                }
                
                bool get_WindowBackground_ResourceImageName( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_WindowBackground_ResourceImageName) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
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
            // cppcheck-suppress duplInheritedMember
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
        
        protected:
            // cppcheck-suppress duplInheritedMember
            void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
            // cppcheck-suppress duplInheritedMember
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            // cppcheck-suppress duplInheritedMember
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_ImageShape
                : public Metabuf::Metaparse
            { 
            public:
                Meta_ImageShape();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Count = (1 <<4),
                    EMETA_ImageOffset = (1 <<1),
                    EMETA_ImageSize = (1 <<0),
                    EMETA_Immutable = (1 <<2),
                    EMETA_Subtract = (1 <<3),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Count() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Count) != 0;
                }
                
                uint32_t getd_Count( uint32_t _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Count;
                }
                
                bool get_Count( uint32_t * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Count( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Count );
                
                    return true;
                }
                
                bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_ImageOffset) != 0;
                }
                
                template<class C, class M>
                bool getm_ImageOffset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_ImageOffset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_ImageOffset );
                
                    return true;
                }
                
                bool get_ImageOffset( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_ImageOffset) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_ImageOffset;
                
                    return true;
                }
                
                bool getd_ImageOffset( mt::vec2f * _value, const mt::vec2f & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_ImageOffset) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_ImageOffset;
                
                    return true;
                }
                bool has_ImageSize() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_ImageSize) != 0;
                }
                
                template<class C, class M>
                bool getm_ImageSize( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_ImageSize) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_ImageSize );
                
                    return true;
                }
                
                bool get_ImageSize( mt::vec2f * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_ImageSize) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_ImageSize;
                
                    return true;
                }
                
                bool getd_ImageSize( mt::vec2f * _value, const mt::vec2f & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_ImageSize) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_ImageSize;
                
                    return true;
                }
                bool has_Immutable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Immutable) != 0;
                }
                
                bool getd_Immutable( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Immutable;
                }
                
                bool get_Immutable( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Immutable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Immutable );
                
                    return true;
                }
                
                bool getd_Immutable( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_Subtract) != 0;
                }
                
                bool getd_Subtract( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Subtract) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Subtract;
                }
                
                bool get_Subtract( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Subtract) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Subtract;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Subtract( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Subtract) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Subtract );
                
                    return true;
                }
                
                bool getd_Subtract( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Subtract) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Subtract;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Shape
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::Polygon m_Polygon;
                };
                
            protected:
                uint32_t m_Count;
                mt::vec2f m_ImageMaxSize;
                mt::vec2f m_ImageOffset;
                mt::vec2f m_ImageSize;
                bool m_Immutable;
                uint32_t m_LayerIndex;
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_KeyFrames2D();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Count = (1 <<1),
                    EMETA_Immutable = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Count() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Count) != 0;
                }
                
                uint32_t getd_Count( uint32_t _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Count;
                }
                
                bool get_Count( uint32_t * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Count( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Count );
                
                    return true;
                }
                
                bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                bool has_Immutable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Immutable) != 0;
                }
                
                bool getd_Immutable( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Immutable;
                }
                
                bool get_Immutable( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Immutable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Immutable );
                
                    return true;
                }
                
                bool getd_Immutable( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_KeyFrame2D
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_KeyFrame2D();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_AnchorPoint = (1 <<0),
                        EMETA_Count = (1 <<6),
                        EMETA_Opacity = (1 <<4),
                        EMETA_Position = (1 <<1),
                        EMETA_Rotation = (1 <<3),
                        EMETA_Scale = (1 <<2),
                        EMETA_Volume = (1 <<5),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
                public:
                    bool has_AnchorPoint() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec2f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec2f * _value, const mt::vec2f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_Count() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Count) != 0;
                    }
                    
                    uint32_t getd_Count( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Count;
                    }
                    
                    bool get_Count( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Count( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Count );
                    
                        return true;
                    }
                    
                    bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    bool has_Opacity() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Opacity) != 0;
                    }
                    
                    float getd_Opacity( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Opacity;
                    }
                    
                    bool get_Opacity( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Opacity( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Opacity );
                    
                        return true;
                    }
                    
                    bool getd_Opacity( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Position) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec2f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec2f * _value, const mt::vec2f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Rotation) != 0;
                    }
                    
                    float getd_Rotation( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Rotation;
                    }
                    
                    bool get_Rotation( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool getd_Rotation( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Scale) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec2f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec2f * _value, const mt::vec2f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Volume() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Volume) != 0;
                    }
                    
                    float getd_Volume( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Volume;
                    }
                    
                    bool get_Volume( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Volume( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Volume );
                    
                        return true;
                    }
                    
                    bool getd_Volume( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                public:
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    mt::vec2f m_AnchorPoint;
                    uint32_t m_Count;
                    float m_Opacity;
                    mt::vec2f m_Position;
                    float m_Rotation;
                    mt::vec2f m_Scale;
                    float m_Volume;
                };
                
            protected:
                uint32_t m_Count;
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_KeyFrames3D();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Count = (1 <<1),
                    EMETA_Immutable = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Count() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Count) != 0;
                }
                
                uint32_t getd_Count( uint32_t _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Count;
                }
                
                bool get_Count( uint32_t * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Count( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Count );
                
                    return true;
                }
                
                bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Count;
                
                    return true;
                }
                bool has_Immutable() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Immutable) != 0;
                }
                
                bool getd_Immutable( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Immutable;
                }
                
                bool get_Immutable( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Immutable;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Immutable( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Immutable );
                
                    return true;
                }
                
                bool getd_Immutable( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Immutable) == 0 )
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_KeyFrame3D
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_KeyFrame3D();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_AnchorPoint = (1 <<0),
                        EMETA_Count = (1 <<7),
                        EMETA_Opacity = (1 <<5),
                        EMETA_Orientation = (1 <<4),
                        EMETA_Position = (1 <<1),
                        EMETA_Rotation = (1 <<3),
                        EMETA_Scale = (1 <<2),
                        EMETA_Volume = (1 <<6),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
                public:
                    bool has_AnchorPoint() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_AnchorPoint( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_AnchorPoint );
                    
                        return true;
                    }
                    
                    bool get_AnchorPoint( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    
                    bool getd_AnchorPoint( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_AnchorPoint;
                    
                        return true;
                    }
                    bool has_Count() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Count) != 0;
                    }
                    
                    uint32_t getd_Count( uint32_t _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Count;
                    }
                    
                    bool get_Count( uint32_t * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Count( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Count );
                    
                        return true;
                    }
                    
                    bool getd_Count( uint32_t * _value, const uint32_t & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Count) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Count;
                    
                        return true;
                    }
                    bool has_Opacity() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Opacity) != 0;
                    }
                    
                    float getd_Opacity( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Opacity;
                    }
                    
                    bool get_Opacity( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Opacity( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Opacity );
                    
                        return true;
                    }
                    
                    bool getd_Opacity( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Opacity) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Opacity;
                    
                        return true;
                    }
                    bool has_Orientation() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Orientation) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Orientation( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Orientation) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Orientation );
                    
                        return true;
                    }
                    
                    bool get_Orientation( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Orientation) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Orientation;
                    
                        return true;
                    }
                    
                    bool getd_Orientation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Orientation) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Orientation;
                    
                        return true;
                    }
                    bool has_Position() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Position) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Position( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Position );
                    
                        return true;
                    }
                    
                    bool get_Position( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    
                    bool getd_Position( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Position) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Position;
                    
                        return true;
                    }
                    bool has_Rotation() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Rotation) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Rotation( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Rotation );
                    
                        return true;
                    }
                    
                    bool get_Rotation( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    
                    bool getd_Rotation( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Rotation) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Rotation;
                    
                        return true;
                    }
                    bool has_Scale() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Scale) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Scale( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Scale );
                    
                        return true;
                    }
                    
                    bool get_Scale( mt::vec3f * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    
                    bool getd_Scale( mt::vec3f * _value, const mt::vec3f & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Scale) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Scale;
                    
                        return true;
                    }
                    bool has_Volume() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Volume) != 0;
                    }
                    
                    float getd_Volume( float _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            return _default;
                        }
                    
                        return this->m_Volume;
                    }
                    
                    bool get_Volume( float * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                    
                    template<class C, class M>
                    bool getm_Volume( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Volume );
                    
                        return true;
                    }
                    
                    bool getd_Volume( float * _value, const float & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Volume) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Volume;
                    
                        return true;
                    }
                public:
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    mt::vec3f m_AnchorPoint;
                    uint32_t m_Count;
                    float m_Opacity;
                    mt::vec3f m_Orientation;
                    mt::vec3f m_Position;
                    mt::vec3f m_Rotation;
                    mt::vec3f m_Scale;
                    float m_Volume;
                };
                
            protected:
                uint32_t m_Count;
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
                : public Metabuf::Metaparse
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                uint32_t m_LayerIndex;
                Mengine::Polygon m_Value;
            };
            
            class Meta_TimeRemap
                : public Metabuf::Metaparse
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
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                uint32_t m_LayerIndex;
                Mengine::Floats m_Time;
            };
            
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
            // cppcheck-suppress duplInheritedMember
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
        
        protected:
            // cppcheck-suppress duplInheritedMember
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            // cppcheck-suppress duplInheritedMember
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_Datas
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Datas();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Data
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::ConstString m_Name;
                    Mengine::FilePath m_Path;
                };
                
            protected:
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Fonts();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Font
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Font();
                
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::FilePath m_Path;
                };
                
            protected:
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Font> VectorMeta_Font;
            
                const VectorMeta_Font & get_Includes_Font() const
                {
                    return this->includes_Meta_Font;
                }
            
            protected:
                VectorMeta_Font includes_Meta_Font;
            };
            
            class Meta_Glyphs
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Glyphs();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Glyph
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Glyph();
                
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::FilePath m_Path;
                };
                
            protected:
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Glyph> VectorMeta_Glyph;
            
                const VectorMeta_Glyph & get_Includes_Glyph() const
                {
                    return this->includes_Meta_Glyph;
                }
            
            protected:
                VectorMeta_Glyph includes_Meta_Glyph;
            };
            
            class Meta_Materials
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Materials();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Material
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::FilePath m_Path;
                };
                
            protected:
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Resources();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Demand = (1 <<1),
                    EMETA_Ignored = (1 <<0),
                    EMETA_Platform = (1 <<2),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Demand() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Demand) != 0;
                }
                
                bool getd_Demand( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Demand) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Demand;
                }
                
                bool get_Demand( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Demand) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Demand;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Demand( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Demand) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Demand );
                
                    return true;
                }
                
                bool getd_Demand( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Demand) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Demand;
                
                    return true;
                }
                bool has_Ignored() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Ignored) != 0;
                }
                
                bool getd_Ignored( bool _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Ignored) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_Ignored;
                }
                
                bool get_Ignored( bool * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Ignored) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Ignored;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_Ignored( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Ignored) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Ignored );
                
                    return true;
                }
                
                bool getd_Ignored( bool * _value, const bool & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Ignored) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Ignored;
                
                    return true;
                }
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Resource
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Resource();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_Tags = (1 <<0),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
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
                        return (m_flagNoRequiredAttribute & EMETA_Tags) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Tags( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Tags) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Tags );
                    
                        return true;
                    }
                    
                    bool get_Tags( Mengine::Tags * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Tags) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Tags;
                    
                        return true;
                    }
                    
                    bool getd_Tags( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Tags) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Tags;
                    
                        return true;
                    }
                public:
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::FilePath m_Path;
                    Mengine::Tags m_Tags;
                };
                
            protected:
                bool m_Demand;
                bool m_Ignored;
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
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Scripts();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Script
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Script();
                
                protected:
                    enum NoRequiredAttribute
                    {
                        EMETA_Finalizer = (1 <<2),
                        EMETA_Initializer = (1 <<1),
                        EMETA_Module = (1 <<0),
                    };
                    
                    uint32_t m_flagNoRequiredAttribute;
                    
                public:
                    bool has_Finalizer() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Finalizer) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Finalizer( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Finalizer) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Finalizer );
                    
                        return true;
                    }
                    
                    bool get_Finalizer( Mengine::ConstString * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Finalizer) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Finalizer;
                    
                        return true;
                    }
                    
                    bool getd_Finalizer( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Finalizer) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Finalizer;
                    
                        return true;
                    }
                    bool has_Initializer() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Initializer) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Initializer( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Initializer) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Initializer );
                    
                        return true;
                    }
                    
                    bool get_Initializer( Mengine::ConstString * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Initializer) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Initializer;
                    
                        return true;
                    }
                    
                    bool getd_Initializer( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Initializer) == 0 )
                        {
                            *_value = _default;
                    
                            return false;
                        }
                    
                        *_value = this->m_Initializer;
                    
                        return true;
                    }
                    bool has_Module() const
                    {
                        return (m_flagNoRequiredAttribute & EMETA_Module) != 0;
                    }
                    
                    template<class C, class M>
                    bool getm_Module( C _self, M _method ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Module) == 0 )
                        {
                            return false;
                        }
                    
                        (_self->*_method)( this->m_Module );
                    
                        return true;
                    }
                    
                    bool get_Module( Mengine::ConstString * _value ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Module) == 0 )
                        {
                            return false;
                        }
                    
                        *_value = this->m_Module;
                    
                        return true;
                    }
                    
                    bool getd_Module( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                    {
                        if( (m_flagNoRequiredAttribute & EMETA_Module) == 0 )
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
                    
                public:
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::ConstString m_Finalizer;
                    Mengine::ConstString m_Initializer;
                    Mengine::ConstString m_Module;
                    Mengine::FilePath m_Path;
                };
                
            protected:
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Script> VectorMeta_Script;
            
                const VectorMeta_Script & get_Includes_Script() const
                {
                    return this->includes_Meta_Script;
                }
            
            protected:
                VectorMeta_Script includes_Meta_Script;
            };
            
            class Meta_Settings
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Settings();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Setting
                    : public Metabuf::Metaparse
                { 
                public:
                    Meta_Setting();
                
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::ConstString m_Name;
                    Mengine::FilePath m_Path;
                };
                
            protected:
                Mengine::Tags m_Platform;
            public:
                typedef Metabuf::Vector<Meta_Setting> VectorMeta_Setting;
            
                const VectorMeta_Setting & get_Includes_Setting() const
                {
                    return this->includes_Meta_Setting;
                }
            
            protected:
                VectorMeta_Setting includes_Meta_Setting;
            };
            
            class Meta_Texts
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Texts();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_Platform = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_Platform() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Platform) != 0;
                }
                
                template<class C, class M>
                bool getm_Platform( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Platform );
                
                    return true;
                }
                
                bool get_Platform( Mengine::Tags * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
                
                bool getd_Platform( Mengine::Tags * _value, const Mengine::Tags & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Platform) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Platform;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
                class Meta_Text
                    : public Metabuf::Metaparse
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
                    // cppcheck-suppress duplInheritedMember
                    bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
                
                protected:
                    // cppcheck-suppress duplInheritedMember
                    void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                    // cppcheck-suppress duplInheritedMember
                    void _preparationIncludes( uint32_t _id, uint32_t _count );
                    // cppcheck-suppress duplInheritedMember
                    void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
                
                public:
                protected:
                    Mengine::FilePath m_Path;
                };
                
            protected:
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
            typedef Metabuf::Vector<Meta_Glyphs> VectorMeta_Glyphs;
        
            const VectorMeta_Glyphs & get_Includes_Glyphs() const
            {
                return this->includes_Meta_Glyphs;
            }
        
        protected:
            VectorMeta_Glyphs includes_Meta_Glyphs;
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
            typedef Metabuf::Vector<Meta_Settings> VectorMeta_Settings;
        
            const VectorMeta_Settings & get_Includes_Settings() const
            {
                return this->includes_Meta_Settings;
            }
        
        protected:
            VectorMeta_Settings includes_Meta_Settings;
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
            // cppcheck-suppress duplInheritedMember
            bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
        
        protected:
            // cppcheck-suppress duplInheritedMember
            void _preparationIncludes( uint32_t _id, uint32_t _count );
            // cppcheck-suppress duplInheritedMember
            void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
        
        public:
            class Meta_Text
                : public Metabuf::Metaparse
            { 
            public:
                Meta_Text();
            
            protected:
                enum NoRequiredAttribute
                {
                    EMETA_CharOffset = (1 <<2),
                    EMETA_Font = (1 <<1),
                    EMETA_LineOffset = (1 <<3),
                    EMETA_Value = (1 <<0),
                };
                
                uint32_t m_flagNoRequiredAttribute;
                
            public:
                bool has_CharOffset() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_CharOffset) != 0;
                }
                
                float getd_CharOffset( float _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_CharOffset) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_CharOffset;
                }
                
                bool get_CharOffset( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_CharOffset) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_CharOffset;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_CharOffset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_CharOffset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_CharOffset );
                
                    return true;
                }
                
                bool getd_CharOffset( float * _value, const float & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_CharOffset) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_CharOffset;
                
                    return true;
                }
                bool has_Font() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Font) != 0;
                }
                
                template<class C, class M>
                bool getm_Font( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Font) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Font );
                
                    return true;
                }
                
                bool get_Font( Mengine::ConstString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Font) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Font;
                
                    return true;
                }
                
                bool getd_Font( Mengine::ConstString * _value, const Mengine::ConstString & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Font) == 0 )
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
                    return (m_flagNoRequiredAttribute & EMETA_LineOffset) != 0;
                }
                
                float getd_LineOffset( float _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_LineOffset) == 0 )
                    {
                        return _default;
                    }
                
                    return this->m_LineOffset;
                }
                
                bool get_LineOffset( float * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_LineOffset) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_LineOffset;
                
                    return true;
                }
                
                template<class C, class M>
                bool getm_LineOffset( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_LineOffset) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_LineOffset );
                
                    return true;
                }
                
                bool getd_LineOffset( float * _value, const float & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_LineOffset) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_LineOffset;
                
                    return true;
                }
                bool has_Value() const
                {
                    return (m_flagNoRequiredAttribute & EMETA_Value) != 0;
                }
                
                template<class C, class M>
                bool getm_Value( C _self, M _method ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Value) == 0 )
                    {
                        return false;
                    }
                
                    (_self->*_method)( this->m_Value );
                
                    return true;
                }
                
                bool get_Value( Mengine::WString * _value ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Value) == 0 )
                    {
                        return false;
                    }
                
                    *_value = this->m_Value;
                
                    return true;
                }
                
                bool getd_Value( Mengine::WString * _value, const Mengine::WString & _default ) const
                {
                    if( (m_flagNoRequiredAttribute & EMETA_Value) == 0 )
                    {
                        *_value = _default;
                
                        return false;
                    }
                
                    *_value = this->m_Value;
                
                    return true;
                }
            public:
                // cppcheck-suppress duplInheritedMember
                bool parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData ) override;
            
            protected:
                // cppcheck-suppress duplInheritedMember
                void _parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData );
                // cppcheck-suppress duplInheritedMember
                void _preparationIncludes( uint32_t _id, uint32_t _count );
                // cppcheck-suppress duplInheritedMember
                void _parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData );
            
            public:
            protected:
                float m_CharOffset;
                Mengine::ConstString m_Font;
                Mengine::ConstString m_Key;
                float m_LineOffset;
                Mengine::WString m_Value;
            };
            
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
