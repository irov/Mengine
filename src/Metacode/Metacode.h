#   pragma once

#   include "Metatype.h"

#   include <metabuf/Metabuf.hpp>

namespace Metacode
{
    uint32_t get_metacode_magic();
    uint32_t get_metacode_version();
    bool readHeader( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion );
    bool readStrings( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringCount );
    const char * readString( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringSize );

    class Meta_DataBlock
        : public Metabuf::Metadata
    { 
    public:
        Meta_DataBlock();
        ~Meta_DataBlock();
    
    public:
        uint32_t getId() const override;
    
    public:
        const Menge::ConstString & get_Name() const
        {
            return this->Name;
        }
        
        void swap_Name( Menge::ConstString & _value ) const
        {
            std::swap( _value, this->Name);
        }
        
    protected:
        void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
        void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_FragmentShader
            : public Metabuf::Metadata
        { 
        public:
            Meta_FragmentShader();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Name() const
            {
                return this->Name;
            }
            
            void swap_Name( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Name);
            }
            
            const Menge::ConstString & get_Platform() const
            {
                return this->Platform;
            }
            
            void swap_Platform( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Platform);
            }
            
            bool has_File_Compile() const
            {
                return File_Compile_successful;
            }
            
            bool get_File_Compile( bool & _value ) const
            {
                if( File_Compile_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Compile;
            
                return true;
            }
            
            bool swap_File_Compile( bool & _value ) const
            {
                if( File_Compile_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Compile);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Name;
            mutable Menge::ConstString Platform;
            bool File_Compile_successful;
            mutable bool File_Compile;
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_Include
            : public Metabuf::Metadata
        { 
        public:
            Meta_Include();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::FilePath & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath Path;
        };
        
        class Meta_Material
            : public Metabuf::Metadata
        { 
        public:
            Meta_Material();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Debug() const
            {
                return Debug_successful;
            }
            
            bool get_Debug( bool & _value ) const
            {
                if( Debug_successful == false )
                {
                    return false;
                }
            
                _value = this->Debug;
            
                return true;
            }
            
            bool swap_Debug( bool & _value ) const
            {
                if( Debug_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Debug);
            
                return true;
            }
            
            const Menge::ConstString & get_Name() const
            {
                return this->Name;
            }
            
            void swap_Name( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Name);
            }
            
            bool has_AlphaBlend_Enable() const
            {
                return AlphaBlend_Enable_successful;
            }
            
            bool get_AlphaBlend_Enable( bool & _value ) const
            {
                if( AlphaBlend_Enable_successful == false )
                {
                    return false;
                }
            
                _value = this->AlphaBlend_Enable;
            
                return true;
            }
            
            bool swap_AlphaBlend_Enable( bool & _value ) const
            {
                if( AlphaBlend_Enable_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->AlphaBlend_Enable);
            
                return true;
            }
            
            bool has_BlendFactor_Dest() const
            {
                return BlendFactor_Dest_successful;
            }
            
            bool get_BlendFactor_Dest( Menge::EBlendFactor & _value ) const
            {
                if( BlendFactor_Dest_successful == false )
                {
                    return false;
                }
            
                _value = this->BlendFactor_Dest;
            
                return true;
            }
            
            bool swap_BlendFactor_Dest( Menge::EBlendFactor & _value ) const
            {
                if( BlendFactor_Dest_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->BlendFactor_Dest);
            
                return true;
            }
            
            bool has_BlendFactor_Op() const
            {
                return BlendFactor_Op_successful;
            }
            
            bool get_BlendFactor_Op( Menge::EBlendOp & _value ) const
            {
                if( BlendFactor_Op_successful == false )
                {
                    return false;
                }
            
                _value = this->BlendFactor_Op;
            
                return true;
            }
            
            bool swap_BlendFactor_Op( Menge::EBlendOp & _value ) const
            {
                if( BlendFactor_Op_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->BlendFactor_Op);
            
                return true;
            }
            
            bool has_BlendFactor_Source() const
            {
                return BlendFactor_Source_successful;
            }
            
            bool get_BlendFactor_Source( Menge::EBlendFactor & _value ) const
            {
                if( BlendFactor_Source_successful == false )
                {
                    return false;
                }
            
                _value = this->BlendFactor_Source;
            
                return true;
            }
            
            bool swap_BlendFactor_Source( Menge::EBlendFactor & _value ) const
            {
                if( BlendFactor_Source_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->BlendFactor_Source);
            
                return true;
            }
            
            bool has_Program_Name() const
            {
                return Program_Name_successful;
            }
            
            bool get_Program_Name( Menge::ConstString & _value ) const
            {
                if( Program_Name_successful == false )
                {
                    return false;
                }
            
                _value = this->Program_Name;
            
                return true;
            }
            
            bool swap_Program_Name( Menge::ConstString & _value ) const
            {
                if( Program_Name_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Program_Name);
            
                return true;
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_TextureStages
                : public Metabuf::Metadata
            { 
            public:
                Meta_TextureStages();
            
            public:
                uint32_t getId() const override;
            
            public:
                uint32_t get_Stage() const
                {
                    return this->Stage;
                }
                
                void swap_Stage( uint32_t & _value ) const
                {
                    std::swap( _value, this->Stage);
                }
                
                bool has_AddressMode_U() const
                {
                    return AddressMode_U_successful;
                }
                
                bool get_AddressMode_U( Menge::ETextureAddressMode & _value ) const
                {
                    if( AddressMode_U_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->AddressMode_U;
                
                    return true;
                }
                
                bool swap_AddressMode_U( Menge::ETextureAddressMode & _value ) const
                {
                    if( AddressMode_U_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->AddressMode_U);
                
                    return true;
                }
                
                bool has_AddressMode_V() const
                {
                    return AddressMode_V_successful;
                }
                
                bool get_AddressMode_V( Menge::ETextureAddressMode & _value ) const
                {
                    if( AddressMode_V_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->AddressMode_V;
                
                    return true;
                }
                
                bool swap_AddressMode_V( Menge::ETextureAddressMode & _value ) const
                {
                    if( AddressMode_V_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->AddressMode_V);
                
                    return true;
                }
                
                bool has_Alpha_Arg1() const
                {
                    return Alpha_Arg1_successful;
                }
                
                bool get_Alpha_Arg1( Menge::ETextureArgument & _value ) const
                {
                    if( Alpha_Arg1_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Alpha_Arg1;
                
                    return true;
                }
                
                bool swap_Alpha_Arg1( Menge::ETextureArgument & _value ) const
                {
                    if( Alpha_Arg1_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->Alpha_Arg1);
                
                    return true;
                }
                
                bool has_Alpha_Arg2() const
                {
                    return Alpha_Arg2_successful;
                }
                
                bool get_Alpha_Arg2( Menge::ETextureArgument & _value ) const
                {
                    if( Alpha_Arg2_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Alpha_Arg2;
                
                    return true;
                }
                
                bool swap_Alpha_Arg2( Menge::ETextureArgument & _value ) const
                {
                    if( Alpha_Arg2_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->Alpha_Arg2);
                
                    return true;
                }
                
                Menge::ETextureOp get_Alpha_Operator() const
                {
                    return this->Alpha_Operator;
                }
                
                void swap_Alpha_Operator( Menge::ETextureOp & _value ) const
                {
                    std::swap(_value, this->Alpha_Operator);
                }
                
                bool has_Color_Arg1() const
                {
                    return Color_Arg1_successful;
                }
                
                bool get_Color_Arg1( Menge::ETextureArgument & _value ) const
                {
                    if( Color_Arg1_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Color_Arg1;
                
                    return true;
                }
                
                bool swap_Color_Arg1( Menge::ETextureArgument & _value ) const
                {
                    if( Color_Arg1_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->Color_Arg1);
                
                    return true;
                }
                
                bool has_Color_Arg2() const
                {
                    return Color_Arg2_successful;
                }
                
                bool get_Color_Arg2( Menge::ETextureArgument & _value ) const
                {
                    if( Color_Arg2_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Color_Arg2;
                
                    return true;
                }
                
                bool swap_Color_Arg2( Menge::ETextureArgument & _value ) const
                {
                    if( Color_Arg2_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->Color_Arg2);
                
                    return true;
                }
                
                Menge::ETextureOp get_Color_Operator() const
                {
                    return this->Color_Operator;
                }
                
                void swap_Color_Operator( Menge::ETextureOp & _value ) const
                {
                    std::swap(_value, this->Color_Operator);
                }
                
                bool has_TextureCoord_Index() const
                {
                    return TextureCoord_Index_successful;
                }
                
                bool get_TextureCoord_Index( uint32_t & _value ) const
                {
                    if( TextureCoord_Index_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->TextureCoord_Index;
                
                    return true;
                }
                
                bool swap_TextureCoord_Index( uint32_t & _value ) const
                {
                    if( TextureCoord_Index_successful == false )
                    {
                        return false;
                    }
                
                    std::swap(_value, this->TextureCoord_Index);
                
                    return true;
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable uint32_t Stage;
                bool AddressMode_U_successful;
                mutable Menge::ETextureAddressMode AddressMode_U;
                bool AddressMode_V_successful;
                mutable Menge::ETextureAddressMode AddressMode_V;
                bool Alpha_Arg1_successful;
                mutable Menge::ETextureArgument Alpha_Arg1;
                bool Alpha_Arg2_successful;
                mutable Menge::ETextureArgument Alpha_Arg2;
                mutable Menge::ETextureOp Alpha_Operator;
                bool Color_Arg1_successful;
                mutable Menge::ETextureArgument Color_Arg1;
                bool Color_Arg2_successful;
                mutable Menge::ETextureArgument Color_Arg2;
                mutable Menge::ETextureOp Color_Operator;
                bool TextureCoord_Index_successful;
                mutable uint32_t TextureCoord_Index;
            };
            
        protected:
        protected:
            bool Debug_successful;
            mutable bool Debug;
            mutable Menge::ConstString Name;
            bool AlphaBlend_Enable_successful;
            mutable bool AlphaBlend_Enable;
            bool BlendFactor_Dest_successful;
            mutable Menge::EBlendFactor BlendFactor_Dest;
            bool BlendFactor_Op_successful;
            mutable Menge::EBlendOp BlendFactor_Op;
            bool BlendFactor_Source_successful;
            mutable Menge::EBlendFactor BlendFactor_Source;
            bool Program_Name_successful;
            mutable Menge::ConstString Program_Name;
        public:
            typedef stdex::auto_array<Meta_TextureStages> TVectorMeta_TextureStages;
        
            const TVectorMeta_TextureStages & get_IncludesTextureStages() const
            {
                return this->includes_Meta_TextureStages;
            }
        
        protected:
            TVectorMeta_TextureStages includes_Meta_TextureStages;
        };
        
        class Meta_Program
            : public Metabuf::Metadata
        { 
        public:
            Meta_Program();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Name() const
            {
                return this->Name;
            }
            
            void swap_Name( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Name);
            }
            
            const Menge::ConstString & get_Platform() const
            {
                return this->Platform;
            }
            
            void swap_Platform( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Platform);
            }
            
            const Menge::ConstString & get_FragmentShader_Name() const
            {
                return this->FragmentShader_Name;
            }
            
            void swap_FragmentShader_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->FragmentShader_Name);
            }
            
            uint32_t get_Sampler_Count() const
            {
                return this->Sampler_Count;
            }
            
            void swap_Sampler_Count( uint32_t & _value ) const
            {
                std::swap(_value, this->Sampler_Count);
            }
            
            const Menge::ConstString & get_VertexShader_Name() const
            {
                return this->VertexShader_Name;
            }
            
            void swap_VertexShader_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->VertexShader_Name);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Name;
            mutable Menge::ConstString Platform;
            mutable Menge::ConstString FragmentShader_Name;
            mutable uint32_t Sampler_Count;
            mutable Menge::ConstString VertexShader_Name;
        };
        
        class Meta_VertexShader
            : public Metabuf::Metadata
        { 
        public:
            Meta_VertexShader();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Name() const
            {
                return this->Name;
            }
            
            void swap_Name( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Name);
            }
            
            const Menge::ConstString & get_Platform() const
            {
                return this->Platform;
            }
            
            void swap_Platform( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Platform);
            }
            
            bool has_File_Compile() const
            {
                return File_Compile_successful;
            }
            
            bool get_File_Compile( bool & _value ) const
            {
                if( File_Compile_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Compile;
            
                return true;
            }
            
            bool swap_File_Compile( bool & _value ) const
            {
                if( File_Compile_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Compile);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Name;
            mutable Menge::ConstString Platform;
            bool File_Compile_successful;
            mutable bool File_Compile;
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_Resource
            : public Metabuf::Metadata
        { 
        public:
            Meta_Resource();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Name() const
            {
                return this->Name;
            }
            
            void swap_Name( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Name);
            }
            
            const Menge::ConstString & get_Type() const
            {
                return this->Type;
            }
            
            void swap_Type( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Type);
            }
            
            bool has_Unique() const
            {
                return Unique_successful;
            }
            
            bool get_Unique( bool & _value ) const
            {
                if( Unique_successful == false )
                {
                    return false;
                }
            
                _value = this->Unique;
            
                return true;
            }
            
            bool swap_Unique( bool & _value ) const
            {
                if( Unique_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Unique);
            
                return true;
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Name;
            mutable Menge::ConstString Type;
            bool Unique_successful;
            mutable bool Unique;
        };
        
        class Meta_ResourceAnimation
            : public Meta_Resource
        { 
        public:
            Meta_ResourceAnimation();
        
        public:
            uint32_t getId() const override;
        
        public:
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Sequence
                : public Metabuf::Metadata
            { 
            public:
                Meta_Sequence();
            
            public:
                uint32_t getId() const override;
            
            public:
                float get_Delay() const
                {
                    return this->Delay;
                }
                
                void swap_Delay( float & _value ) const
                {
                    std::swap( _value, this->Delay);
                }
                
                const Menge::ConstString & get_ResourceImageName() const
                {
                    return this->ResourceImageName;
                }
                
                void swap_ResourceImageName( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->ResourceImageName);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable float Delay;
                mutable Menge::ConstString ResourceImageName;
            };
            
        protected:
        protected:
        public:
            typedef stdex::auto_array<Meta_Sequence> TVectorMeta_Sequence;
        
            const TVectorMeta_Sequence & get_IncludesSequence() const
            {
                return this->includes_Meta_Sequence;
            }
        
        protected:
            TVectorMeta_Sequence includes_Meta_Sequence;
        };
        
        class Meta_ResourceCal3dAnimation
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCal3dAnimation();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceCal3dMesh
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCal3dMesh();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceCal3dSkeleton
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCal3dSkeleton();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceCursorICO
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCursorICO();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceCursorSystem
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCursorSystem();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceExternal
            : public Meta_Resource
        { 
        public:
            Meta_ResourceExternal();
        
        public:
            uint32_t getId() const override;
        
        public:
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceHIT
            : public Meta_Resource
        { 
        public:
            Meta_ResourceHIT();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_File_Codec() const
            {
                return File_Codec_successful;
            }
            
            bool get_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Codec;
            
                return true;
            }
            
            bool swap_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Codec);
            
                return true;
            }
            
            bool has_File_Converter() const
            {
                return File_Converter_successful;
            }
            
            bool get_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Converter;
            
                return true;
            }
            
            bool swap_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Converter);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceImageData
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageData();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_File_Codec() const
            {
                return File_Codec_successful;
            }
            
            bool get_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Codec;
            
                return true;
            }
            
            bool swap_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Codec);
            
                return true;
            }
            
            const mt::vec2f & get_File_MaxSize() const
            {
                return this->File_MaxSize;
            }
            
            void swap_File_MaxSize( mt::vec2f & _value ) const
            {
                std::swap(_value, this->File_MaxSize);
            }
            
            bool has_File_NoExist() const
            {
                return File_NoExist_successful;
            }
            
            bool get_File_NoExist( bool & _value ) const
            {
                if( File_NoExist_successful == false )
                {
                    return false;
                }
            
                _value = this->File_NoExist;
            
                return true;
            }
            
            bool swap_File_NoExist( bool & _value ) const
            {
                if( File_NoExist_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_NoExist);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            mutable mt::vec2f File_MaxSize;
            bool File_NoExist_successful;
            mutable bool File_NoExist;
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceImageDefault
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageDefault();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_File_Alpha() const
            {
                return File_Alpha_successful;
            }
            
            bool get_File_Alpha( bool & _value ) const
            {
                if( File_Alpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Alpha;
            
                return true;
            }
            
            bool swap_File_Alpha( bool & _value ) const
            {
                if( File_Alpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Alpha);
            
                return true;
            }
            
            bool has_File_Codec() const
            {
                return File_Codec_successful;
            }
            
            bool get_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Codec;
            
                return true;
            }
            
            bool swap_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Codec);
            
                return true;
            }
            
            bool has_File_Converter() const
            {
                return File_Converter_successful;
            }
            
            bool get_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Converter;
            
                return true;
            }
            
            bool swap_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Converter);
            
                return true;
            }
            
            const mt::vec2f & get_File_MaxSize() const
            {
                return this->File_MaxSize;
            }
            
            void swap_File_MaxSize( mt::vec2f & _value ) const
            {
                std::swap(_value, this->File_MaxSize);
            }
            
            bool has_File_NoExist() const
            {
                return File_NoExist_successful;
            }
            
            bool get_File_NoExist( bool & _value ) const
            {
                if( File_NoExist_successful == false )
                {
                    return false;
                }
            
                _value = this->File_NoExist;
            
                return true;
            }
            
            bool swap_File_NoExist( bool & _value ) const
            {
                if( File_NoExist_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_NoExist);
            
                return true;
            }
            
            bool has_File_Offset() const
            {
                return File_Offset_successful;
            }
            
            bool get_File_Offset( mt::vec2f & _value ) const
            {
                if( File_Offset_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Offset;
            
                return true;
            }
            
            bool swap_File_Offset( mt::vec2f & _value ) const
            {
                if( File_Offset_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Offset);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            bool has_File_Premultiply() const
            {
                return File_Premultiply_successful;
            }
            
            bool get_File_Premultiply( bool & _value ) const
            {
                if( File_Premultiply_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Premultiply;
            
                return true;
            }
            
            bool swap_File_Premultiply( bool & _value ) const
            {
                if( File_Premultiply_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Premultiply);
            
                return true;
            }
            
            bool has_File_Size() const
            {
                return File_Size_successful;
            }
            
            bool get_File_Size( mt::vec2f & _value ) const
            {
                if( File_Size_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Size;
            
                return true;
            }
            
            bool swap_File_Size( mt::vec2f & _value ) const
            {
                if( File_Size_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Size);
            
                return true;
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable mt::vec2f File_MaxSize;
            bool File_NoExist_successful;
            mutable bool File_NoExist;
            bool File_Offset_successful;
            mutable mt::vec2f File_Offset;
            mutable Menge::FilePath File_Path;
            bool File_Premultiply_successful;
            mutable bool File_Premultiply;
            bool File_Size_successful;
            mutable mt::vec2f File_Size;
        };
        
        class Meta_ResourceImageSolid
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageSolid();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ColourValue & get_Color_Value() const
            {
                return this->Color_Value;
            }
            
            void swap_Color_Value( Menge::ColourValue & _value ) const
            {
                std::swap(_value, this->Color_Value);
            }
            
            const mt::vec2f & get_Size_Value() const
            {
                return this->Size_Value;
            }
            
            void swap_Size_Value( mt::vec2f & _value ) const
            {
                std::swap(_value, this->Size_Value);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ColourValue Color_Value;
            mutable mt::vec2f Size_Value;
        };
        
        class Meta_ResourceImageSubstract
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageSubstract();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Image_Alpha() const
            {
                return Image_Alpha_successful;
            }
            
            bool get_Image_Alpha( bool & _value ) const
            {
                if( Image_Alpha_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Alpha;
            
                return true;
            }
            
            bool swap_Image_Alpha( bool & _value ) const
            {
                if( Image_Alpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Alpha);
            
                return true;
            }
            
            const mt::vec2f & get_Image_MaxSize() const
            {
                return this->Image_MaxSize;
            }
            
            void swap_Image_MaxSize( mt::vec2f & _value ) const
            {
                std::swap(_value, this->Image_MaxSize);
            }
            
            const Menge::ConstString & get_Image_Name() const
            {
                return this->Image_Name;
            }
            
            void swap_Image_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Image_Name);
            }
            
            bool has_Image_Offset() const
            {
                return Image_Offset_successful;
            }
            
            bool get_Image_Offset( mt::vec2f & _value ) const
            {
                if( Image_Offset_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Offset;
            
                return true;
            }
            
            bool swap_Image_Offset( mt::vec2f & _value ) const
            {
                if( Image_Offset_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Offset);
            
                return true;
            }
            
            bool has_Image_Size() const
            {
                return Image_Size_successful;
            }
            
            bool get_Image_Size( mt::vec2f & _value ) const
            {
                if( Image_Size_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Size;
            
                return true;
            }
            
            bool swap_Image_Size( mt::vec2f & _value ) const
            {
                if( Image_Size_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Size);
            
                return true;
            }
            
            const mt::uv4f & get_Image_UV() const
            {
                return this->Image_UV;
            }
            
            void swap_Image_UV( mt::uv4f & _value ) const
            {
                std::swap(_value, this->Image_UV);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool Image_Alpha_successful;
            mutable bool Image_Alpha;
            mutable mt::vec2f Image_MaxSize;
            mutable Menge::ConstString Image_Name;
            bool Image_Offset_successful;
            mutable mt::vec2f Image_Offset;
            bool Image_Size_successful;
            mutable mt::vec2f Image_Size;
            mutable mt::uv4f Image_UV;
        };
        
        class Meta_ResourceImageSubstractRGBAndAlpha
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageSubstractRGBAndAlpha();
        
        public:
            uint32_t getId() const override;
        
        public:
            const mt::vec2f & get_Image_MaxSize() const
            {
                return this->Image_MaxSize;
            }
            
            void swap_Image_MaxSize( mt::vec2f & _value ) const
            {
                std::swap(_value, this->Image_MaxSize);
            }
            
            const Menge::ConstString & get_Image_NameAlpha() const
            {
                return this->Image_NameAlpha;
            }
            
            void swap_Image_NameAlpha( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Image_NameAlpha);
            }
            
            const Menge::ConstString & get_Image_NameRGB() const
            {
                return this->Image_NameRGB;
            }
            
            void swap_Image_NameRGB( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Image_NameRGB);
            }
            
            bool has_Image_Offset() const
            {
                return Image_Offset_successful;
            }
            
            bool get_Image_Offset( mt::vec2f & _value ) const
            {
                if( Image_Offset_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Offset;
            
                return true;
            }
            
            bool swap_Image_Offset( mt::vec2f & _value ) const
            {
                if( Image_Offset_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Offset);
            
                return true;
            }
            
            bool has_Image_Size() const
            {
                return Image_Size_successful;
            }
            
            bool get_Image_Size( mt::vec2f & _value ) const
            {
                if( Image_Size_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Size;
            
                return true;
            }
            
            bool swap_Image_Size( mt::vec2f & _value ) const
            {
                if( Image_Size_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Size);
            
                return true;
            }
            
            const mt::uv4f & get_Image_UVAlpha() const
            {
                return this->Image_UVAlpha;
            }
            
            void swap_Image_UVAlpha( mt::uv4f & _value ) const
            {
                std::swap(_value, this->Image_UVAlpha);
            }
            
            const mt::uv4f & get_Image_UVRGB() const
            {
                return this->Image_UVRGB;
            }
            
            void swap_Image_UVRGB( mt::uv4f & _value ) const
            {
                std::swap(_value, this->Image_UVRGB);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable mt::vec2f Image_MaxSize;
            mutable Menge::ConstString Image_NameAlpha;
            mutable Menge::ConstString Image_NameRGB;
            bool Image_Offset_successful;
            mutable mt::vec2f Image_Offset;
            bool Image_Size_successful;
            mutable mt::vec2f Image_Size;
            mutable mt::uv4f Image_UVAlpha;
            mutable mt::uv4f Image_UVRGB;
        };
        
        class Meta_ResourceInternalObject
            : public Meta_Resource
        { 
        public:
            Meta_ResourceInternalObject();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Internal_Group() const
            {
                return this->Internal_Group;
            }
            
            void swap_Internal_Group( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Internal_Group);
            }
            
            const Menge::ConstString & get_Internal_Name() const
            {
                return this->Internal_Name;
            }
            
            void swap_Internal_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Internal_Name);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Internal_Group;
            mutable Menge::ConstString Internal_Name;
        };
        
        class Meta_ResourceModel3D
            : public Meta_Resource
        { 
        public:
            Meta_ResourceModel3D();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_File_Converter() const
            {
                return File_Converter_successful;
            }
            
            bool get_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Converter;
            
                return true;
            }
            
            bool swap_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Converter);
            
                return true;
            }
            
            bool has_File_Dataflow() const
            {
                return File_Dataflow_successful;
            }
            
            bool get_File_Dataflow( Menge::ConstString & _value ) const
            {
                if( File_Dataflow_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Dataflow;
            
                return true;
            }
            
            bool swap_File_Dataflow( Menge::ConstString & _value ) const
            {
                if( File_Dataflow_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Dataflow);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            const Menge::ConstString & get_Image_Resource() const
            {
                return this->Image_Resource;
            }
            
            void swap_Image_Resource( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Image_Resource);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            bool File_Dataflow_successful;
            mutable Menge::ConstString File_Dataflow;
            mutable Menge::FilePath File_Path;
            mutable Menge::ConstString Image_Resource;
        };
        
        class Meta_ResourceMovie
            : public Meta_Resource
        { 
        public:
            Meta_ResourceMovie();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Anchor_Point() const
            {
                return Anchor_Point_successful;
            }
            
            bool get_Anchor_Point( mt::vec3f & _value ) const
            {
                if( Anchor_Point_successful == false )
                {
                    return false;
                }
            
                _value = this->Anchor_Point;
            
                return true;
            }
            
            bool swap_Anchor_Point( mt::vec3f & _value ) const
            {
                if( Anchor_Point_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Anchor_Point);
            
                return true;
            }
            
            bool has_Bounds_Box() const
            {
                return Bounds_Box_successful;
            }
            
            bool get_Bounds_Box( mt::box2f & _value ) const
            {
                if( Bounds_Box_successful == false )
                {
                    return false;
                }
            
                _value = this->Bounds_Box;
            
                return true;
            }
            
            bool swap_Bounds_Box( mt::box2f & _value ) const
            {
                if( Bounds_Box_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Bounds_Box);
            
                return true;
            }
            
            float get_Duration_Value() const
            {
                return this->Duration_Value;
            }
            
            void swap_Duration_Value( float & _value ) const
            {
                std::swap(_value, this->Duration_Value);
            }
            
            float get_FrameDuration_Value() const
            {
                return this->FrameDuration_Value;
            }
            
            void swap_FrameDuration_Value( float & _value ) const
            {
                std::swap(_value, this->FrameDuration_Value);
            }
            
            float get_Height_Value() const
            {
                return this->Height_Value;
            }
            
            void swap_Height_Value( float & _value ) const
            {
                std::swap(_value, this->Height_Value);
            }
            
            bool has_KeyFramesPackPath_Codec() const
            {
                return KeyFramesPackPath_Codec_successful;
            }
            
            bool get_KeyFramesPackPath_Codec( Menge::ConstString & _value ) const
            {
                if( KeyFramesPackPath_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->KeyFramesPackPath_Codec;
            
                return true;
            }
            
            bool swap_KeyFramesPackPath_Codec( Menge::ConstString & _value ) const
            {
                if( KeyFramesPackPath_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->KeyFramesPackPath_Codec);
            
                return true;
            }
            
            bool has_KeyFramesPackPath_Converter() const
            {
                return KeyFramesPackPath_Converter_successful;
            }
            
            bool get_KeyFramesPackPath_Converter( Menge::ConstString & _value ) const
            {
                if( KeyFramesPackPath_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->KeyFramesPackPath_Converter;
            
                return true;
            }
            
            bool swap_KeyFramesPackPath_Converter( Menge::ConstString & _value ) const
            {
                if( KeyFramesPackPath_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->KeyFramesPackPath_Converter);
            
                return true;
            }
            
            const Menge::ConstString & get_KeyFramesPackPath_Path() const
            {
                return this->KeyFramesPackPath_Path;
            }
            
            void swap_KeyFramesPackPath_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->KeyFramesPackPath_Path);
            }
            
            bool has_Loop_Segment() const
            {
                return Loop_Segment_successful;
            }
            
            bool get_Loop_Segment( mt::vec2f & _value ) const
            {
                if( Loop_Segment_successful == false )
                {
                    return false;
                }
            
                _value = this->Loop_Segment;
            
                return true;
            }
            
            bool swap_Loop_Segment( mt::vec2f & _value ) const
            {
                if( Loop_Segment_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Loop_Segment);
            
                return true;
            }
            
            bool has_Offset_Point() const
            {
                return Offset_Point_successful;
            }
            
            bool get_Offset_Point( mt::vec3f & _value ) const
            {
                if( Offset_Point_successful == false )
                {
                    return false;
                }
            
                _value = this->Offset_Point;
            
                return true;
            }
            
            bool swap_Offset_Point( mt::vec3f & _value ) const
            {
                if( Offset_Point_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Offset_Point);
            
                return true;
            }
            
            float get_Width_Value() const
            {
                return this->Width_Value;
            }
            
            void swap_Width_Value( float & _value ) const
            {
                std::swap(_value, this->Width_Value);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_MovieCamera3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_MovieCamera3D();
            
            public:
                uint32_t getId() const override;
            
            public:
                float get_CameraAspect() const
                {
                    return this->CameraAspect;
                }
                
                void swap_CameraAspect( float & _value ) const
                {
                    std::swap( _value, this->CameraAspect);
                }
                
                float get_CameraFOV() const
                {
                    return this->CameraFOV;
                }
                
                void swap_CameraFOV( float & _value ) const
                {
                    std::swap( _value, this->CameraFOV);
                }
                
                const mt::vec3f & get_CameraInterest() const
                {
                    return this->CameraInterest;
                }
                
                void swap_CameraInterest( mt::vec3f & _value ) const
                {
                    std::swap( _value, this->CameraInterest);
                }
                
                const mt::vec3f & get_CameraPosition() const
                {
                    return this->CameraPosition;
                }
                
                void swap_CameraPosition( mt::vec3f & _value ) const
                {
                    std::swap( _value, this->CameraPosition);
                }
                
                float get_Height() const
                {
                    return this->Height;
                }
                
                void swap_Height( float & _value ) const
                {
                    std::swap( _value, this->Height);
                }
                
                float get_Width() const
                {
                    return this->Width;
                }
                
                void swap_Width( float & _value ) const
                {
                    std::swap( _value, this->Width);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable float CameraAspect;
                mutable float CameraFOV;
                mutable mt::vec3f CameraInterest;
                mutable mt::vec3f CameraPosition;
                mutable float Height;
                mutable float Width;
            };
            
            class Meta_MovieLayer2D
                : public Metabuf::Metadata
            { 
            public:
                Meta_MovieLayer2D();
            
            public:
                uint32_t getId() const override;
            
            public:
                bool has_BlendingMode() const
                {
                    return BlendingMode_successful;
                }
                
                bool get_BlendingMode( Menge::ConstString & _value ) const
                {
                    if( BlendingMode_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->BlendingMode;
                
                    return true;
                }
                
                bool swap_BlendingMode( Menge::ConstString & _value ) const
                {
                    if( BlendingMode_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->BlendingMode);
                
                    return true;
                }
                
                float get_In() const
                {
                    return this->In;
                }
                
                void swap_In( float & _value ) const
                {
                    std::swap( _value, this->In);
                }
                
                uint32_t get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( uint32_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                bool has_Loop() const
                {
                    return Loop_successful;
                }
                
                bool get_Loop( bool & _value ) const
                {
                    if( Loop_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Loop;
                
                    return true;
                }
                
                bool swap_Loop( bool & _value ) const
                {
                    if( Loop_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Loop);
                
                    return true;
                }
                
                const Menge::ConstString & get_Name() const
                {
                    return this->Name;
                }
                
                void swap_Name( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Name);
                }
                
                float get_Out() const
                {
                    return this->Out;
                }
                
                void swap_Out( float & _value ) const
                {
                    std::swap( _value, this->Out);
                }
                
                bool has_Params() const
                {
                    return Params_successful;
                }
                
                bool get_Params( uint32_t & _value ) const
                {
                    if( Params_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Params;
                
                    return true;
                }
                
                bool swap_Params( uint32_t & _value ) const
                {
                    if( Params_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Params);
                
                    return true;
                }
                
                bool has_Parent() const
                {
                    return Parent_successful;
                }
                
                bool get_Parent( uint32_t & _value ) const
                {
                    if( Parent_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Parent;
                
                    return true;
                }
                
                bool swap_Parent( uint32_t & _value ) const
                {
                    if( Parent_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Parent);
                
                    return true;
                }
                
                bool has_PlayCount() const
                {
                    return PlayCount_successful;
                }
                
                bool get_PlayCount( uint32_t & _value ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->PlayCount;
                
                    return true;
                }
                
                bool swap_PlayCount( uint32_t & _value ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->PlayCount);
                
                    return true;
                }
                
                bool has_Polygon() const
                {
                    return Polygon_successful;
                }
                
                bool get_Polygon( bool & _value ) const
                {
                    if( Polygon_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Polygon;
                
                    return true;
                }
                
                bool swap_Polygon( bool & _value ) const
                {
                    if( Polygon_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Polygon);
                
                    return true;
                }
                
                bool has_Position() const
                {
                    return Position_successful;
                }
                
                bool get_Position( mt::vec3f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Position;
                
                    return true;
                }
                
                bool swap_Position( mt::vec3f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Position);
                
                    return true;
                }
                
                bool has_Shape() const
                {
                    return Shape_successful;
                }
                
                bool get_Shape( bool & _value ) const
                {
                    if( Shape_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Shape;
                
                    return true;
                }
                
                bool swap_Shape( bool & _value ) const
                {
                    if( Shape_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Shape);
                
                    return true;
                }
                
                const Menge::ConstString & get_Source() const
                {
                    return this->Source;
                }
                
                void swap_Source( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Source);
                }
                
                bool has_StartInterval() const
                {
                    return StartInterval_successful;
                }
                
                bool get_StartInterval( float & _value ) const
                {
                    if( StartInterval_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->StartInterval;
                
                    return true;
                }
                
                bool swap_StartInterval( float & _value ) const
                {
                    if( StartInterval_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->StartInterval);
                
                    return true;
                }
                
                bool has_Stretch() const
                {
                    return Stretch_successful;
                }
                
                bool get_Stretch( float & _value ) const
                {
                    if( Stretch_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Stretch;
                
                    return true;
                }
                
                bool swap_Stretch( float & _value ) const
                {
                    if( Stretch_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Stretch);
                
                    return true;
                }
                
                bool has_Switch() const
                {
                    return Switch_successful;
                }
                
                bool get_Switch( bool & _value ) const
                {
                    if( Switch_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Switch;
                
                    return true;
                }
                
                bool swap_Switch( bool & _value ) const
                {
                    if( Switch_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Switch);
                
                    return true;
                }
                
                bool has_TimeRemap() const
                {
                    return TimeRemap_successful;
                }
                
                bool get_TimeRemap( bool & _value ) const
                {
                    if( TimeRemap_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->TimeRemap;
                
                    return true;
                }
                
                bool swap_TimeRemap( bool & _value ) const
                {
                    if( TimeRemap_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->TimeRemap);
                
                    return true;
                }
                
                const Menge::ConstString & get_Type() const
                {
                    return this->Type;
                }
                
                void swap_Type( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Type);
                }
                
                bool has_Viewport() const
                {
                    return Viewport_successful;
                }
                
                bool get_Viewport( Menge::Viewport & _value ) const
                {
                    if( Viewport_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Viewport;
                
                    return true;
                }
                
                bool swap_Viewport( Menge::Viewport & _value ) const
                {
                    if( Viewport_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Viewport);
                
                    return true;
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool BlendingMode_successful;
                mutable Menge::ConstString BlendingMode;
                mutable float In;
                mutable uint32_t Index;
                bool Loop_successful;
                mutable bool Loop;
                mutable Menge::ConstString Name;
                mutable float Out;
                bool Params_successful;
                mutable uint32_t Params;
                bool Parent_successful;
                mutable uint32_t Parent;
                bool PlayCount_successful;
                mutable uint32_t PlayCount;
                bool Polygon_successful;
                mutable bool Polygon;
                bool Position_successful;
                mutable mt::vec3f Position;
                bool Shape_successful;
                mutable bool Shape;
                mutable Menge::ConstString Source;
                bool StartInterval_successful;
                mutable float StartInterval;
                bool Stretch_successful;
                mutable float Stretch;
                bool Switch_successful;
                mutable bool Switch;
                bool TimeRemap_successful;
                mutable bool TimeRemap;
                mutable Menge::ConstString Type;
                bool Viewport_successful;
                mutable Menge::Viewport Viewport;
            };
            
            class Meta_MovieLayer3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_MovieLayer3D();
            
            public:
                uint32_t getId() const override;
            
            public:
                bool has_BlendingMode() const
                {
                    return BlendingMode_successful;
                }
                
                bool get_BlendingMode( Menge::ConstString & _value ) const
                {
                    if( BlendingMode_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->BlendingMode;
                
                    return true;
                }
                
                bool swap_BlendingMode( Menge::ConstString & _value ) const
                {
                    if( BlendingMode_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->BlendingMode);
                
                    return true;
                }
                
                float get_In() const
                {
                    return this->In;
                }
                
                void swap_In( float & _value ) const
                {
                    std::swap( _value, this->In);
                }
                
                uint32_t get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( uint32_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                bool has_Loop() const
                {
                    return Loop_successful;
                }
                
                bool get_Loop( bool & _value ) const
                {
                    if( Loop_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Loop;
                
                    return true;
                }
                
                bool swap_Loop( bool & _value ) const
                {
                    if( Loop_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Loop);
                
                    return true;
                }
                
                const Menge::ConstString & get_Name() const
                {
                    return this->Name;
                }
                
                void swap_Name( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Name);
                }
                
                float get_Out() const
                {
                    return this->Out;
                }
                
                void swap_Out( float & _value ) const
                {
                    std::swap( _value, this->Out);
                }
                
                bool has_Params() const
                {
                    return Params_successful;
                }
                
                bool get_Params( uint32_t & _value ) const
                {
                    if( Params_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Params;
                
                    return true;
                }
                
                bool swap_Params( uint32_t & _value ) const
                {
                    if( Params_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Params);
                
                    return true;
                }
                
                bool has_Parent() const
                {
                    return Parent_successful;
                }
                
                bool get_Parent( uint32_t & _value ) const
                {
                    if( Parent_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Parent;
                
                    return true;
                }
                
                bool swap_Parent( uint32_t & _value ) const
                {
                    if( Parent_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Parent);
                
                    return true;
                }
                
                bool has_PlayCount() const
                {
                    return PlayCount_successful;
                }
                
                bool get_PlayCount( uint32_t & _value ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->PlayCount;
                
                    return true;
                }
                
                bool swap_PlayCount( uint32_t & _value ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->PlayCount);
                
                    return true;
                }
                
                bool has_Polygon() const
                {
                    return Polygon_successful;
                }
                
                bool get_Polygon( bool & _value ) const
                {
                    if( Polygon_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Polygon;
                
                    return true;
                }
                
                bool swap_Polygon( bool & _value ) const
                {
                    if( Polygon_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Polygon);
                
                    return true;
                }
                
                bool has_Position() const
                {
                    return Position_successful;
                }
                
                bool get_Position( mt::vec3f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Position;
                
                    return true;
                }
                
                bool swap_Position( mt::vec3f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Position);
                
                    return true;
                }
                
                bool has_Shape() const
                {
                    return Shape_successful;
                }
                
                bool get_Shape( bool & _value ) const
                {
                    if( Shape_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Shape;
                
                    return true;
                }
                
                bool swap_Shape( bool & _value ) const
                {
                    if( Shape_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Shape);
                
                    return true;
                }
                
                const Menge::ConstString & get_Source() const
                {
                    return this->Source;
                }
                
                void swap_Source( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Source);
                }
                
                bool has_StartInterval() const
                {
                    return StartInterval_successful;
                }
                
                bool get_StartInterval( float & _value ) const
                {
                    if( StartInterval_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->StartInterval;
                
                    return true;
                }
                
                bool swap_StartInterval( float & _value ) const
                {
                    if( StartInterval_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->StartInterval);
                
                    return true;
                }
                
                bool has_Stretch() const
                {
                    return Stretch_successful;
                }
                
                bool get_Stretch( float & _value ) const
                {
                    if( Stretch_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Stretch;
                
                    return true;
                }
                
                bool swap_Stretch( float & _value ) const
                {
                    if( Stretch_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Stretch);
                
                    return true;
                }
                
                bool has_Switch() const
                {
                    return Switch_successful;
                }
                
                bool get_Switch( bool & _value ) const
                {
                    if( Switch_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Switch;
                
                    return true;
                }
                
                bool swap_Switch( bool & _value ) const
                {
                    if( Switch_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Switch);
                
                    return true;
                }
                
                bool has_TimeRemap() const
                {
                    return TimeRemap_successful;
                }
                
                bool get_TimeRemap( bool & _value ) const
                {
                    if( TimeRemap_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->TimeRemap;
                
                    return true;
                }
                
                bool swap_TimeRemap( bool & _value ) const
                {
                    if( TimeRemap_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->TimeRemap);
                
                    return true;
                }
                
                const Menge::ConstString & get_Type() const
                {
                    return this->Type;
                }
                
                void swap_Type( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Type);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool BlendingMode_successful;
                mutable Menge::ConstString BlendingMode;
                mutable float In;
                mutable uint32_t Index;
                bool Loop_successful;
                mutable bool Loop;
                mutable Menge::ConstString Name;
                mutable float Out;
                bool Params_successful;
                mutable uint32_t Params;
                bool Parent_successful;
                mutable uint32_t Parent;
                bool PlayCount_successful;
                mutable uint32_t PlayCount;
                bool Polygon_successful;
                mutable bool Polygon;
                bool Position_successful;
                mutable mt::vec3f Position;
                bool Shape_successful;
                mutable bool Shape;
                mutable Menge::ConstString Source;
                bool StartInterval_successful;
                mutable float StartInterval;
                bool Stretch_successful;
                mutable float Stretch;
                bool Switch_successful;
                mutable bool Switch;
                bool TimeRemap_successful;
                mutable bool TimeRemap;
                mutable Menge::ConstString Type;
            };
            
        protected:
        protected:
            bool Anchor_Point_successful;
            mutable mt::vec3f Anchor_Point;
            bool Bounds_Box_successful;
            mutable mt::box2f Bounds_Box;
            mutable float Duration_Value;
            mutable float FrameDuration_Value;
            mutable float Height_Value;
            bool KeyFramesPackPath_Codec_successful;
            mutable Menge::ConstString KeyFramesPackPath_Codec;
            bool KeyFramesPackPath_Converter_successful;
            mutable Menge::ConstString KeyFramesPackPath_Converter;
            mutable Menge::ConstString KeyFramesPackPath_Path;
            bool Loop_Segment_successful;
            mutable mt::vec2f Loop_Segment;
            bool Offset_Point_successful;
            mutable mt::vec3f Offset_Point;
            mutable float Width_Value;
        public:
            typedef stdex::auto_array<Meta_MovieCamera3D> TVectorMeta_MovieCamera3D;
        
            const TVectorMeta_MovieCamera3D & get_IncludesMovieCamera3D() const
            {
                return this->includes_Meta_MovieCamera3D;
            }
        
        protected:
            TVectorMeta_MovieCamera3D includes_Meta_MovieCamera3D;
        public:
            typedef stdex::auto_array<Meta_MovieLayer2D> TVectorMeta_MovieLayer2D;
        
            const TVectorMeta_MovieLayer2D & get_IncludesMovieLayer2D() const
            {
                return this->includes_Meta_MovieLayer2D;
            }
        
        protected:
            TVectorMeta_MovieLayer2D includes_Meta_MovieLayer2D;
        public:
            typedef stdex::auto_array<Meta_MovieLayer3D> TVectorMeta_MovieLayer3D;
        
            const TVectorMeta_MovieLayer3D & get_IncludesMovieLayer3D() const
            {
                return this->includes_Meta_MovieLayer3D;
            }
        
        protected:
            TVectorMeta_MovieLayer3D includes_Meta_MovieLayer3D;
        };
        
        class Meta_ResourceParticle
            : public Meta_Resource
        { 
        public:
            Meta_ResourceParticle();
        
        public:
            uint32_t getId() const override;
        
        public:
            uint32_t get_AtlasCount_Value() const
            {
                return this->AtlasCount_Value;
            }
            
            void swap_AtlasCount_Value( uint32_t & _value ) const
            {
                std::swap(_value, this->AtlasCount_Value);
            }
            
            bool has_File_Converter() const
            {
                return File_Converter_successful;
            }
            
            bool get_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Converter;
            
                return true;
            }
            
            bool swap_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Converter);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Atlas
                : public Metabuf::Metadata
            { 
            public:
                Meta_Atlas();
            
            public:
                uint32_t getId() const override;
            
            public:
                uint32_t get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( uint32_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                const Menge::ConstString & get_ResourceName() const
                {
                    return this->ResourceName;
                }
                
                void swap_ResourceName( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->ResourceName);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable uint32_t Index;
                mutable Menge::ConstString ResourceName;
            };
            
        protected:
        protected:
            mutable uint32_t AtlasCount_Value;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable Menge::FilePath File_Path;
        public:
            typedef stdex::auto_array<Meta_Atlas> TVectorMeta_Atlas;
        
            const TVectorMeta_Atlas & get_IncludesAtlas() const
            {
                return this->includes_Meta_Atlas;
            }
        
        protected:
            TVectorMeta_Atlas includes_Meta_Atlas;
        };
        
        class Meta_ResourcePlaylist
            : public Meta_Resource
        { 
        public:
            Meta_ResourcePlaylist();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Loop_Value() const
            {
                return Loop_Value_successful;
            }
            
            bool get_Loop_Value( bool & _value ) const
            {
                if( Loop_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Loop_Value;
            
                return true;
            }
            
            bool swap_Loop_Value( bool & _value ) const
            {
                if( Loop_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Loop_Value);
            
                return true;
            }
            
            bool has_Shuffle_Value() const
            {
                return Shuffle_Value_successful;
            }
            
            bool get_Shuffle_Value( bool & _value ) const
            {
                if( Shuffle_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Shuffle_Value;
            
                return true;
            }
            
            bool swap_Shuffle_Value( bool & _value ) const
            {
                if( Shuffle_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Shuffle_Value);
            
                return true;
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Tracks
                : public Metabuf::Metadata
            { 
            public:
                Meta_Tracks();
            
            public:
                uint32_t getId() const override;
            
            public:
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
                class Meta_Track
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Track();
                
                public:
                    uint32_t getId() const override;
                
                public:
                    bool has_Codec() const
                    {
                        return Codec_successful;
                    }
                    
                    bool get_Codec( Menge::ConstString & _value ) const
                    {
                        if( Codec_successful == false )
                        {
                            return false;
                        }
                    
                        _value = this->Codec;
                    
                        return true;
                    }
                    
                    bool swap_Codec( Menge::ConstString & _value ) const
                    {
                        if( Codec_successful == false )
                        {
                            return false;
                        }
                    
                        std::swap( _value, this->Codec);
                    
                        return true;
                    }
                    
                    bool has_External() const
                    {
                        return External_successful;
                    }
                    
                    bool get_External( bool & _value ) const
                    {
                        if( External_successful == false )
                        {
                            return false;
                        }
                    
                        _value = this->External;
                    
                        return true;
                    }
                    
                    bool swap_External( bool & _value ) const
                    {
                        if( External_successful == false )
                        {
                            return false;
                        }
                    
                        std::swap( _value, this->External);
                    
                        return true;
                    }
                    
                    const Menge::FilePath & get_File() const
                    {
                        return this->File;
                    }
                    
                    void swap_File( Menge::FilePath & _value ) const
                    {
                        std::swap( _value, this->File);
                    }
                    
                protected:
                    void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                    void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                    void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                    void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                    void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
                public:
                protected:
                protected:
                    bool Codec_successful;
                    mutable Menge::ConstString Codec;
                    bool External_successful;
                    mutable bool External;
                    mutable Menge::FilePath File;
                };
                
            protected:
            protected:
            public:
                typedef stdex::auto_array<Meta_Track> TVectorMeta_Track;
            
                const TVectorMeta_Track & get_IncludesTrack() const
                {
                    return this->includes_Meta_Track;
                }
            
            protected:
                TVectorMeta_Track includes_Meta_Track;
            };
            
        protected:
        protected:
            bool Loop_Value_successful;
            mutable bool Loop_Value;
            bool Shuffle_Value_successful;
            mutable bool Shuffle_Value;
        public:
            typedef stdex::auto_array<Meta_Tracks> TVectorMeta_Tracks;
        
            const TVectorMeta_Tracks & get_IncludesTracks() const
            {
                return this->includes_Meta_Tracks;
            }
        
        protected:
            TVectorMeta_Tracks includes_Meta_Tracks;
        };
        
        class Meta_ResourceShape
            : public Meta_Resource
        { 
        public:
            Meta_ResourceShape();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::Polygon & get_Polygon_Value() const
            {
                return this->Polygon_Value;
            }
            
            void swap_Polygon_Value( Menge::Polygon & _value ) const
            {
                std::swap(_value, this->Polygon_Value);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::Polygon Polygon_Value;
        };
        
        class Meta_ResourceSound
            : public Meta_Resource
        { 
        public:
            Meta_ResourceSound();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_DefaultVolume_Value() const
            {
                return DefaultVolume_Value_successful;
            }
            
            bool get_DefaultVolume_Value( float & _value ) const
            {
                if( DefaultVolume_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->DefaultVolume_Value;
            
                return true;
            }
            
            bool swap_DefaultVolume_Value( float & _value ) const
            {
                if( DefaultVolume_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->DefaultVolume_Value);
            
                return true;
            }
            
            bool has_File_Codec() const
            {
                return File_Codec_successful;
            }
            
            bool get_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Codec;
            
                return true;
            }
            
            bool swap_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Codec);
            
                return true;
            }
            
            bool has_File_Converter() const
            {
                return File_Converter_successful;
            }
            
            bool get_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Converter;
            
                return true;
            }
            
            bool swap_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Converter);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            bool has_IsStreamable_Value() const
            {
                return IsStreamable_Value_successful;
            }
            
            bool get_IsStreamable_Value( bool & _value ) const
            {
                if( IsStreamable_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->IsStreamable_Value;
            
                return true;
            }
            
            bool swap_IsStreamable_Value( bool & _value ) const
            {
                if( IsStreamable_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->IsStreamable_Value);
            
                return true;
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool DefaultVolume_Value_successful;
            mutable float DefaultVolume_Value;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable Menge::FilePath File_Path;
            bool IsStreamable_Value_successful;
            mutable bool IsStreamable_Value;
        };
        
        class Meta_ResourceSpine
            : public Meta_Resource
        { 
        public:
            Meta_ResourceSpine();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_Atlas_Path() const
            {
                return this->Atlas_Path;
            }
            
            void swap_Atlas_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->Atlas_Path);
            }
            
            const Menge::FilePath & get_Skeleton_Path() const
            {
                return this->Skeleton_Path;
            }
            
            void swap_Skeleton_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->Skeleton_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Image
                : public Metabuf::Metadata
            { 
            public:
                Meta_Image();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::ConstString & get_Name() const
                {
                    return this->Name;
                }
                
                void swap_Name( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Name);
                }
                
                const Menge::ConstString & get_Resource() const
                {
                    return this->Resource;
                }
                
                void swap_Resource( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Resource);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::ConstString Name;
                mutable Menge::ConstString Resource;
            };
            
        protected:
        protected:
            mutable Menge::FilePath Atlas_Path;
            mutable Menge::FilePath Skeleton_Path;
        public:
            typedef stdex::auto_array<Meta_Image> TVectorMeta_Image;
        
            const TVectorMeta_Image & get_IncludesImage() const
            {
                return this->includes_Meta_Image;
            }
        
        protected:
            TVectorMeta_Image includes_Meta_Image;
        };
        
        class Meta_ResourceVideo
            : public Meta_Resource
        { 
        public:
            Meta_ResourceVideo();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_File_Alpha() const
            {
                return File_Alpha_successful;
            }
            
            bool get_File_Alpha( bool & _value ) const
            {
                if( File_Alpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Alpha;
            
                return true;
            }
            
            bool swap_File_Alpha( bool & _value ) const
            {
                if( File_Alpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Alpha);
            
                return true;
            }
            
            bool has_File_Codec() const
            {
                return File_Codec_successful;
            }
            
            bool get_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Codec;
            
                return true;
            }
            
            bool swap_File_Codec( Menge::ConstString & _value ) const
            {
                if( File_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Codec);
            
                return true;
            }
            
            bool has_File_Converter() const
            {
                return File_Converter_successful;
            }
            
            bool get_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Converter;
            
                return true;
            }
            
            bool swap_File_Converter( Menge::ConstString & _value ) const
            {
                if( File_Converter_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Converter);
            
                return true;
            }
            
            bool has_File_Duration() const
            {
                return File_Duration_successful;
            }
            
            bool get_File_Duration( float & _value ) const
            {
                if( File_Duration_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Duration;
            
                return true;
            }
            
            bool swap_File_Duration( float & _value ) const
            {
                if( File_Duration_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Duration);
            
                return true;
            }
            
            bool has_File_FrameRate() const
            {
                return File_FrameRate_successful;
            }
            
            bool get_File_FrameRate( uint32_t & _value ) const
            {
                if( File_FrameRate_successful == false )
                {
                    return false;
                }
            
                _value = this->File_FrameRate;
            
                return true;
            }
            
            bool swap_File_FrameRate( uint32_t & _value ) const
            {
                if( File_FrameRate_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_FrameRate);
            
                return true;
            }
            
            bool has_File_NoSeek() const
            {
                return File_NoSeek_successful;
            }
            
            bool get_File_NoSeek( bool & _value ) const
            {
                if( File_NoSeek_successful == false )
                {
                    return false;
                }
            
                _value = this->File_NoSeek;
            
                return true;
            }
            
            bool swap_File_NoSeek( bool & _value ) const
            {
                if( File_NoSeek_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_NoSeek);
            
                return true;
            }
            
            const Menge::FilePath & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::FilePath & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            bool File_Duration_successful;
            mutable float File_Duration;
            bool File_FrameRate_successful;
            mutable uint32_t File_FrameRate;
            bool File_NoSeek_successful;
            mutable bool File_NoSeek;
            mutable Menge::FilePath File_Path;
        };
        
        class Meta_ResourceWindow
            : public Meta_Resource
        { 
        public:
            Meta_ResourceWindow();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_WindowBackground_ResourceImageName() const
            {
                return WindowBackground_ResourceImageName_successful;
            }
            
            bool get_WindowBackground_ResourceImageName( Menge::ConstString & _value ) const
            {
                if( WindowBackground_ResourceImageName_successful == false )
                {
                    return false;
                }
            
                _value = this->WindowBackground_ResourceImageName;
            
                return true;
            }
            
            bool swap_WindowBackground_ResourceImageName( Menge::ConstString & _value ) const
            {
                if( WindowBackground_ResourceImageName_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->WindowBackground_ResourceImageName);
            
                return true;
            }
            
            const mt::vec2f & get_WindowBottom_Offset() const
            {
                return this->WindowBottom_Offset;
            }
            
            void swap_WindowBottom_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowBottom_Offset);
            }
            
            const Menge::ConstString & get_WindowBottom_ResourceImageName() const
            {
                return this->WindowBottom_ResourceImageName;
            }
            
            void swap_WindowBottom_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowBottom_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowLeft_Offset() const
            {
                return this->WindowLeft_Offset;
            }
            
            void swap_WindowLeft_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowLeft_Offset);
            }
            
            const Menge::ConstString & get_WindowLeft_ResourceImageName() const
            {
                return this->WindowLeft_ResourceImageName;
            }
            
            void swap_WindowLeft_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowLeft_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowLeftBottom_Offset() const
            {
                return this->WindowLeftBottom_Offset;
            }
            
            void swap_WindowLeftBottom_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowLeftBottom_Offset);
            }
            
            const Menge::ConstString & get_WindowLeftBottom_ResourceImageName() const
            {
                return this->WindowLeftBottom_ResourceImageName;
            }
            
            void swap_WindowLeftBottom_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowLeftBottom_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowLeftTop_Offset() const
            {
                return this->WindowLeftTop_Offset;
            }
            
            void swap_WindowLeftTop_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowLeftTop_Offset);
            }
            
            const Menge::ConstString & get_WindowLeftTop_ResourceImageName() const
            {
                return this->WindowLeftTop_ResourceImageName;
            }
            
            void swap_WindowLeftTop_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowLeftTop_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowRight_Offset() const
            {
                return this->WindowRight_Offset;
            }
            
            void swap_WindowRight_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowRight_Offset);
            }
            
            const Menge::ConstString & get_WindowRight_ResourceImageName() const
            {
                return this->WindowRight_ResourceImageName;
            }
            
            void swap_WindowRight_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowRight_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowRightBottom_Offset() const
            {
                return this->WindowRightBottom_Offset;
            }
            
            void swap_WindowRightBottom_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowRightBottom_Offset);
            }
            
            const Menge::ConstString & get_WindowRightBottom_ResourceImageName() const
            {
                return this->WindowRightBottom_ResourceImageName;
            }
            
            void swap_WindowRightBottom_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowRightBottom_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowRightTop_Offset() const
            {
                return this->WindowRightTop_Offset;
            }
            
            void swap_WindowRightTop_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowRightTop_Offset);
            }
            
            const Menge::ConstString & get_WindowRightTop_ResourceImageName() const
            {
                return this->WindowRightTop_ResourceImageName;
            }
            
            void swap_WindowRightTop_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowRightTop_ResourceImageName);
            }
            
            const mt::vec2f & get_WindowTop_Offset() const
            {
                return this->WindowTop_Offset;
            }
            
            void swap_WindowTop_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowTop_Offset);
            }
            
            const Menge::ConstString & get_WindowTop_ResourceImageName() const
            {
                return this->WindowTop_ResourceImageName;
            }
            
            void swap_WindowTop_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowTop_ResourceImageName);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool WindowBackground_ResourceImageName_successful;
            mutable Menge::ConstString WindowBackground_ResourceImageName;
            mutable mt::vec2f WindowBottom_Offset;
            mutable Menge::ConstString WindowBottom_ResourceImageName;
            mutable mt::vec2f WindowLeft_Offset;
            mutable Menge::ConstString WindowLeft_ResourceImageName;
            mutable mt::vec2f WindowLeftBottom_Offset;
            mutable Menge::ConstString WindowLeftBottom_ResourceImageName;
            mutable mt::vec2f WindowLeftTop_Offset;
            mutable Menge::ConstString WindowLeftTop_ResourceImageName;
            mutable mt::vec2f WindowRight_Offset;
            mutable Menge::ConstString WindowRight_ResourceImageName;
            mutable mt::vec2f WindowRightBottom_Offset;
            mutable Menge::ConstString WindowRightBottom_ResourceImageName;
            mutable mt::vec2f WindowRightTop_Offset;
            mutable Menge::ConstString WindowRightTop_ResourceImageName;
            mutable mt::vec2f WindowTop_Offset;
            mutable Menge::ConstString WindowTop_ResourceImageName;
        };
        
    protected:
    protected:
        mutable Menge::ConstString Name;
    public:
        typedef stdex::auto_array<Meta_FragmentShader> TVectorMeta_FragmentShader;
    
        const TVectorMeta_FragmentShader & get_IncludesFragmentShader() const
        {
            return this->includes_Meta_FragmentShader;
        }
    
    protected:
        TVectorMeta_FragmentShader includes_Meta_FragmentShader;
    public:
        typedef stdex::auto_array<Meta_Include> TVectorMeta_Include;
    
        const TVectorMeta_Include & get_IncludesInclude() const
        {
            return this->includes_Meta_Include;
        }
    
    protected:
        TVectorMeta_Include includes_Meta_Include;
    public:
        typedef stdex::auto_array<Meta_Material> TVectorMeta_Material;
    
        const TVectorMeta_Material & get_IncludesMaterial() const
        {
            return this->includes_Meta_Material;
        }
    
    protected:
        TVectorMeta_Material includes_Meta_Material;
    public:
        typedef stdex::auto_array<Meta_Program> TVectorMeta_Program;
    
        const TVectorMeta_Program & get_IncludesProgram() const
        {
            return this->includes_Meta_Program;
        }
    
    protected:
        TVectorMeta_Program includes_Meta_Program;
    public:
        typedef stdex::auto_array<Meta_VertexShader> TVectorMeta_VertexShader;
    
        const TVectorMeta_VertexShader & get_IncludesVertexShader() const
        {
            return this->includes_Meta_VertexShader;
        }
    
    protected:
        TVectorMeta_VertexShader includes_Meta_VertexShader;
    public:
        typedef stdex::auto_array<Meta_Resource *> TVectorMeta_Resource;
    
        const TVectorMeta_Resource & get_IncludesResource() const
        {
            return this->includes_Meta_Resource;
        }
    
    protected:
        TVectorMeta_Resource includes_Meta_Resource;
    };
    
    class Meta_KeyFramesPack
        : public Metabuf::Metadata
    { 
    public:
        Meta_KeyFramesPack();
    
    public:
        uint32_t getId() const override;
    
    public:
        uint32_t get_MaxIndex() const
        {
            return this->MaxIndex;
        }
        
        void swap_MaxIndex( uint32_t & _value ) const
        {
            std::swap( _value, this->MaxIndex);
        }
        
    protected:
        void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
        void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_ImageShape
            : public Metabuf::Metadata
        { 
        public:
            Meta_ImageShape();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Count() const
            {
                return Count_successful;
            }
            
            bool get_Count( uint32_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                _value = this->Count;
            
                return true;
            }
            
            bool swap_Count( uint32_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Count);
            
                return true;
            }
            
            float get_Height() const
            {
                return this->Height;
            }
            
            void swap_Height( float & _value ) const
            {
                std::swap( _value, this->Height);
            }
            
            bool has_Immutable() const
            {
                return Immutable_successful;
            }
            
            bool get_Immutable( bool & _value ) const
            {
                if( Immutable_successful == false )
                {
                    return false;
                }
            
                _value = this->Immutable;
            
                return true;
            }
            
            bool swap_Immutable( bool & _value ) const
            {
                if( Immutable_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Immutable);
            
                return true;
            }
            
            uint32_t get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            float get_Width() const
            {
                return this->Width;
            }
            
            void swap_Width( float & _value ) const
            {
                std::swap( _value, this->Width);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Shape
                : public Metabuf::Metadata
            { 
            public:
                Meta_Shape();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::Polygon & get_Polygon() const
                {
                    return this->Polygon;
                }
                
                void swap_Polygon( Menge::Polygon & _value ) const
                {
                    std::swap( _value, this->Polygon);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::Polygon Polygon;
            };
            
        protected:
        protected:
            bool Count_successful;
            mutable uint32_t Count;
            mutable float Height;
            bool Immutable_successful;
            mutable bool Immutable;
            mutable uint32_t LayerIndex;
            mutable float Width;
        public:
            typedef stdex::auto_array<Meta_Shape> TVectorMeta_Shape;
        
            const TVectorMeta_Shape & get_IncludesShape() const
            {
                return this->includes_Meta_Shape;
            }
        
        protected:
            TVectorMeta_Shape includes_Meta_Shape;
        };
        
        class Meta_KeyFrames2D
            : public Metabuf::Metadata
        { 
        public:
            Meta_KeyFrames2D();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Count() const
            {
                return Count_successful;
            }
            
            bool get_Count( uint32_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                _value = this->Count;
            
                return true;
            }
            
            bool swap_Count( uint32_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Count);
            
                return true;
            }
            
            bool has_Immutable() const
            {
                return Immutable_successful;
            }
            
            bool get_Immutable( bool & _value ) const
            {
                if( Immutable_successful == false )
                {
                    return false;
                }
            
                _value = this->Immutable;
            
                return true;
            }
            
            bool swap_Immutable( bool & _value ) const
            {
                if( Immutable_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Immutable);
            
                return true;
            }
            
            uint32_t get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_KeyFrame2D
                : public Metabuf::Metadata
            { 
            public:
                Meta_KeyFrame2D();
            
            public:
                uint32_t getId() const override;
            
            public:
                bool has_AnchorPoint() const
                {
                    return AnchorPoint_successful;
                }
                
                bool get_AnchorPoint( mt::vec2f & _value ) const
                {
                    if( AnchorPoint_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->AnchorPoint;
                
                    return true;
                }
                
                bool swap_AnchorPoint( mt::vec2f & _value ) const
                {
                    if( AnchorPoint_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->AnchorPoint);
                
                    return true;
                }
                
                bool has_Count() const
                {
                    return Count_successful;
                }
                
                bool get_Count( uint32_t & _value ) const
                {
                    if( Count_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Count;
                
                    return true;
                }
                
                bool swap_Count( uint32_t & _value ) const
                {
                    if( Count_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Count);
                
                    return true;
                }
                
                bool has_Opacity() const
                {
                    return Opacity_successful;
                }
                
                bool get_Opacity( float & _value ) const
                {
                    if( Opacity_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Opacity;
                
                    return true;
                }
                
                bool swap_Opacity( float & _value ) const
                {
                    if( Opacity_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Opacity);
                
                    return true;
                }
                
                bool has_Position() const
                {
                    return Position_successful;
                }
                
                bool get_Position( mt::vec2f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Position;
                
                    return true;
                }
                
                bool swap_Position( mt::vec2f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Position);
                
                    return true;
                }
                
                bool has_Rotation() const
                {
                    return Rotation_successful;
                }
                
                bool get_Rotation( float & _value ) const
                {
                    if( Rotation_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Rotation;
                
                    return true;
                }
                
                bool swap_Rotation( float & _value ) const
                {
                    if( Rotation_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Rotation);
                
                    return true;
                }
                
                bool has_Scale() const
                {
                    return Scale_successful;
                }
                
                bool get_Scale( mt::vec2f & _value ) const
                {
                    if( Scale_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Scale;
                
                    return true;
                }
                
                bool swap_Scale( mt::vec2f & _value ) const
                {
                    if( Scale_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Scale);
                
                    return true;
                }
                
                bool has_Volume() const
                {
                    return Volume_successful;
                }
                
                bool get_Volume( float & _value ) const
                {
                    if( Volume_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Volume;
                
                    return true;
                }
                
                bool swap_Volume( float & _value ) const
                {
                    if( Volume_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Volume);
                
                    return true;
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool AnchorPoint_successful;
                mutable mt::vec2f AnchorPoint;
                bool Count_successful;
                mutable uint32_t Count;
                bool Opacity_successful;
                mutable float Opacity;
                bool Position_successful;
                mutable mt::vec2f Position;
                bool Rotation_successful;
                mutable float Rotation;
                bool Scale_successful;
                mutable mt::vec2f Scale;
                bool Volume_successful;
                mutable float Volume;
            };
            
        protected:
        protected:
            bool Count_successful;
            mutable uint32_t Count;
            bool Immutable_successful;
            mutable bool Immutable;
            mutable uint32_t LayerIndex;
        public:
            typedef stdex::auto_array<Meta_KeyFrame2D> TVectorMeta_KeyFrame2D;
        
            const TVectorMeta_KeyFrame2D & get_IncludesKeyFrame2D() const
            {
                return this->includes_Meta_KeyFrame2D;
            }
        
        protected:
            TVectorMeta_KeyFrame2D includes_Meta_KeyFrame2D;
        };
        
        class Meta_KeyFrames3D
            : public Metabuf::Metadata
        { 
        public:
            Meta_KeyFrames3D();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Count() const
            {
                return Count_successful;
            }
            
            bool get_Count( uint32_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                _value = this->Count;
            
                return true;
            }
            
            bool swap_Count( uint32_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Count);
            
                return true;
            }
            
            bool has_Immutable() const
            {
                return Immutable_successful;
            }
            
            bool get_Immutable( bool & _value ) const
            {
                if( Immutable_successful == false )
                {
                    return false;
                }
            
                _value = this->Immutable;
            
                return true;
            }
            
            bool swap_Immutable( bool & _value ) const
            {
                if( Immutable_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Immutable);
            
                return true;
            }
            
            uint32_t get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_KeyFrame3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_KeyFrame3D();
            
            public:
                uint32_t getId() const override;
            
            public:
                bool has_AnchorPoint() const
                {
                    return AnchorPoint_successful;
                }
                
                bool get_AnchorPoint( mt::vec3f & _value ) const
                {
                    if( AnchorPoint_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->AnchorPoint;
                
                    return true;
                }
                
                bool swap_AnchorPoint( mt::vec3f & _value ) const
                {
                    if( AnchorPoint_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->AnchorPoint);
                
                    return true;
                }
                
                bool has_Count() const
                {
                    return Count_successful;
                }
                
                bool get_Count( uint32_t & _value ) const
                {
                    if( Count_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Count;
                
                    return true;
                }
                
                bool swap_Count( uint32_t & _value ) const
                {
                    if( Count_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Count);
                
                    return true;
                }
                
                bool has_Opacity() const
                {
                    return Opacity_successful;
                }
                
                bool get_Opacity( float & _value ) const
                {
                    if( Opacity_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Opacity;
                
                    return true;
                }
                
                bool swap_Opacity( float & _value ) const
                {
                    if( Opacity_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Opacity);
                
                    return true;
                }
                
                bool has_Orientation() const
                {
                    return Orientation_successful;
                }
                
                bool get_Orientation( mt::vec3f & _value ) const
                {
                    if( Orientation_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Orientation;
                
                    return true;
                }
                
                bool swap_Orientation( mt::vec3f & _value ) const
                {
                    if( Orientation_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Orientation);
                
                    return true;
                }
                
                bool has_Position() const
                {
                    return Position_successful;
                }
                
                bool get_Position( mt::vec3f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Position;
                
                    return true;
                }
                
                bool swap_Position( mt::vec3f & _value ) const
                {
                    if( Position_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Position);
                
                    return true;
                }
                
                bool has_Rotation() const
                {
                    return Rotation_successful;
                }
                
                bool get_Rotation( mt::vec3f & _value ) const
                {
                    if( Rotation_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Rotation;
                
                    return true;
                }
                
                bool swap_Rotation( mt::vec3f & _value ) const
                {
                    if( Rotation_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Rotation);
                
                    return true;
                }
                
                bool has_Scale() const
                {
                    return Scale_successful;
                }
                
                bool get_Scale( mt::vec3f & _value ) const
                {
                    if( Scale_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Scale;
                
                    return true;
                }
                
                bool swap_Scale( mt::vec3f & _value ) const
                {
                    if( Scale_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Scale);
                
                    return true;
                }
                
                bool has_Volume() const
                {
                    return Volume_successful;
                }
                
                bool get_Volume( float & _value ) const
                {
                    if( Volume_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Volume;
                
                    return true;
                }
                
                bool swap_Volume( float & _value ) const
                {
                    if( Volume_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Volume);
                
                    return true;
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool AnchorPoint_successful;
                mutable mt::vec3f AnchorPoint;
                bool Count_successful;
                mutable uint32_t Count;
                bool Opacity_successful;
                mutable float Opacity;
                bool Orientation_successful;
                mutable mt::vec3f Orientation;
                bool Position_successful;
                mutable mt::vec3f Position;
                bool Rotation_successful;
                mutable mt::vec3f Rotation;
                bool Scale_successful;
                mutable mt::vec3f Scale;
                bool Volume_successful;
                mutable float Volume;
            };
            
        protected:
        protected:
            bool Count_successful;
            mutable uint32_t Count;
            bool Immutable_successful;
            mutable bool Immutable;
            mutable uint32_t LayerIndex;
        public:
            typedef stdex::auto_array<Meta_KeyFrame3D> TVectorMeta_KeyFrame3D;
        
            const TVectorMeta_KeyFrame3D & get_IncludesKeyFrame3D() const
            {
                return this->includes_Meta_KeyFrame3D;
            }
        
        protected:
            TVectorMeta_KeyFrame3D includes_Meta_KeyFrame3D;
        };
        
        class Meta_Polygon
            : public Metabuf::Metadata
        { 
        public:
            Meta_Polygon();
        
        public:
            uint32_t getId() const override;
        
        public:
            uint32_t get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            const Menge::Polygon & get_Value() const
            {
                return this->Value;
            }
            
            void swap_Value( Menge::Polygon & _value ) const
            {
                std::swap( _value, this->Value);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable uint32_t LayerIndex;
            mutable Menge::Polygon Value;
        };
        
        class Meta_TimeRemap
            : public Metabuf::Metadata
        { 
        public:
            Meta_TimeRemap();
        
        public:
            uint32_t getId() const override;
        
        public:
            uint32_t get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            const Menge::Floats & get_Time() const
            {
                return this->Time;
            }
            
            void swap_Time( Menge::Floats & _value ) const
            {
                std::swap( _value, this->Time);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable uint32_t LayerIndex;
            mutable Menge::Floats Time;
        };
        
    protected:
    protected:
        mutable uint32_t MaxIndex;
    public:
        typedef stdex::auto_array<Meta_ImageShape> TVectorMeta_ImageShape;
    
        const TVectorMeta_ImageShape & get_IncludesImageShape() const
        {
            return this->includes_Meta_ImageShape;
        }
    
    protected:
        TVectorMeta_ImageShape includes_Meta_ImageShape;
    public:
        typedef stdex::auto_array<Meta_KeyFrames2D> TVectorMeta_KeyFrames2D;
    
        const TVectorMeta_KeyFrames2D & get_IncludesKeyFrames2D() const
        {
            return this->includes_Meta_KeyFrames2D;
        }
    
    protected:
        TVectorMeta_KeyFrames2D includes_Meta_KeyFrames2D;
    public:
        typedef stdex::auto_array<Meta_KeyFrames3D> TVectorMeta_KeyFrames3D;
    
        const TVectorMeta_KeyFrames3D & get_IncludesKeyFrames3D() const
        {
            return this->includes_Meta_KeyFrames3D;
        }
    
    protected:
        TVectorMeta_KeyFrames3D includes_Meta_KeyFrames3D;
    public:
        typedef stdex::auto_array<Meta_Polygon> TVectorMeta_Polygon;
    
        const TVectorMeta_Polygon & get_IncludesPolygon() const
        {
            return this->includes_Meta_Polygon;
        }
    
    protected:
        TVectorMeta_Polygon includes_Meta_Polygon;
    public:
        typedef stdex::auto_array<Meta_TimeRemap> TVectorMeta_TimeRemap;
    
        const TVectorMeta_TimeRemap & get_IncludesTimeRemap() const
        {
            return this->includes_Meta_TimeRemap;
        }
    
    protected:
        TVectorMeta_TimeRemap includes_Meta_TimeRemap;
    };
    
    class Meta_Pak
        : public Metabuf::Metadata
    { 
    public:
        Meta_Pak();
    
    public:
        uint32_t getId() const override;
    
    public:
    protected:
        void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
        void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_Datas
            : public Metabuf::Metadata
        { 
        public:
            Meta_Datas();
        
        public:
            uint32_t getId() const override;
        
        public:
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Data
                : public Metabuf::Metadata
            { 
            public:
                Meta_Data();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::ConstString & get_Name() const
                {
                    return this->Name;
                }
                
                void swap_Name( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Name);
                }
                
                const Menge::FilePath & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::FilePath & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::ConstString Name;
                mutable Menge::FilePath Path;
            };
            
        protected:
        protected:
        public:
            typedef stdex::auto_array<Meta_Data> TVectorMeta_Data;
        
            const TVectorMeta_Data & get_IncludesData() const
            {
                return this->includes_Meta_Data;
            }
        
        protected:
            TVectorMeta_Data includes_Meta_Data;
        };
        
        class Meta_Fonts
            : public Metabuf::Metadata
        { 
        public:
            Meta_Fonts();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::FilePath & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::FilePath & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::FilePath Path;
        };
        
        class Meta_Materials
            : public Metabuf::Metadata
        { 
        public:
            Meta_Materials();
        
        public:
            uint32_t getId() const override;
        
        public:
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Material
                : public Metabuf::Metadata
            { 
            public:
                Meta_Material();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::FilePath & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::FilePath & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::FilePath Path;
            };
            
        protected:
        protected:
        public:
            typedef stdex::auto_array<Meta_Material> TVectorMeta_Material;
        
            const TVectorMeta_Material & get_IncludesMaterial() const
            {
                return this->includes_Meta_Material;
            }
        
        protected:
            TVectorMeta_Material includes_Meta_Material;
        };
        
        class Meta_Resources
            : public Metabuf::Metadata
        { 
        public:
            Meta_Resources();
        
        public:
            uint32_t getId() const override;
        
        public:
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Resource
                : public Metabuf::Metadata
            { 
            public:
                Meta_Resource();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::FilePath & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::FilePath & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::FilePath Path;
            };
            
        protected:
        protected:
        public:
            typedef stdex::auto_array<Meta_Resource> TVectorMeta_Resource;
        
            const TVectorMeta_Resource & get_IncludesResource() const
            {
                return this->includes_Meta_Resource;
            }
        
        protected:
            TVectorMeta_Resource includes_Meta_Resource;
        };
        
        class Meta_Scripts
            : public Metabuf::Metadata
        { 
        public:
            Meta_Scripts();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_Initializer() const
            {
                return Initializer_successful;
            }
            
            bool get_Initializer( Menge::ConstString & _value ) const
            {
                if( Initializer_successful == false )
                {
                    return false;
                }
            
                _value = this->Initializer;
            
                return true;
            }
            
            bool swap_Initializer( Menge::ConstString & _value ) const
            {
                if( Initializer_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Initializer);
            
                return true;
            }
            
            bool has_Module() const
            {
                return Module_successful;
            }
            
            bool get_Module( Menge::ConstString & _value ) const
            {
                if( Module_successful == false )
                {
                    return false;
                }
            
                _value = this->Module;
            
                return true;
            }
            
            bool swap_Module( Menge::ConstString & _value ) const
            {
                if( Module_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Module);
            
                return true;
            }
            
            const Menge::FilePath & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::FilePath & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool Initializer_successful;
            mutable Menge::ConstString Initializer;
            bool Module_successful;
            mutable Menge::ConstString Module;
            mutable Menge::FilePath Path;
        };
        
        class Meta_Texts
            : public Metabuf::Metadata
        { 
        public:
            Meta_Texts();
        
        public:
            uint32_t getId() const override;
        
        public:
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Text
                : public Metabuf::Metadata
            { 
            public:
                Meta_Text();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::FilePath & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::FilePath & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
            protected:
                void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
                void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::FilePath Path;
            };
            
        protected:
        protected:
        public:
            typedef stdex::auto_array<Meta_Text> TVectorMeta_Text;
        
            const TVectorMeta_Text & get_IncludesText() const
            {
                return this->includes_Meta_Text;
            }
        
        protected:
            TVectorMeta_Text includes_Meta_Text;
        };
        
    protected:
    protected:
    public:
        typedef stdex::auto_array<Meta_Datas> TVectorMeta_Datas;
    
        const TVectorMeta_Datas & get_IncludesDatas() const
        {
            return this->includes_Meta_Datas;
        }
    
    protected:
        TVectorMeta_Datas includes_Meta_Datas;
    public:
        typedef stdex::auto_array<Meta_Fonts> TVectorMeta_Fonts;
    
        const TVectorMeta_Fonts & get_IncludesFonts() const
        {
            return this->includes_Meta_Fonts;
        }
    
    protected:
        TVectorMeta_Fonts includes_Meta_Fonts;
    public:
        typedef stdex::auto_array<Meta_Materials> TVectorMeta_Materials;
    
        const TVectorMeta_Materials & get_IncludesMaterials() const
        {
            return this->includes_Meta_Materials;
        }
    
    protected:
        TVectorMeta_Materials includes_Meta_Materials;
    public:
        typedef stdex::auto_array<Meta_Resources> TVectorMeta_Resources;
    
        const TVectorMeta_Resources & get_IncludesResources() const
        {
            return this->includes_Meta_Resources;
        }
    
    protected:
        TVectorMeta_Resources includes_Meta_Resources;
    public:
        typedef stdex::auto_array<Meta_Scripts> TVectorMeta_Scripts;
    
        const TVectorMeta_Scripts & get_IncludesScripts() const
        {
            return this->includes_Meta_Scripts;
        }
    
    protected:
        TVectorMeta_Scripts includes_Meta_Scripts;
    public:
        typedef stdex::auto_array<Meta_Texts> TVectorMeta_Texts;
    
        const TVectorMeta_Texts & get_IncludesTexts() const
        {
            return this->includes_Meta_Texts;
        }
    
    protected:
        TVectorMeta_Texts includes_Meta_Texts;
    };
    
    class Meta_Texts
        : public Metabuf::Metadata
    { 
    public:
        Meta_Texts();
    
    public:
        uint32_t getId() const override;
    
    public:
    protected:
        void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
        void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_Text
            : public Metabuf::Metadata
        { 
        public:
            Meta_Text();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_CharOffset() const
            {
                return CharOffset_successful;
            }
            
            bool get_CharOffset( float & _value ) const
            {
                if( CharOffset_successful == false )
                {
                    return false;
                }
            
                _value = this->CharOffset;
            
                return true;
            }
            
            bool swap_CharOffset( float & _value ) const
            {
                if( CharOffset_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->CharOffset);
            
                return true;
            }
            
            bool has_Font() const
            {
                return Font_successful;
            }
            
            bool get_Font( Menge::ConstString & _value ) const
            {
                if( Font_successful == false )
                {
                    return false;
                }
            
                _value = this->Font;
            
                return true;
            }
            
            bool swap_Font( Menge::ConstString & _value ) const
            {
                if( Font_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Font);
            
                return true;
            }
            
            const Menge::ConstString & get_Key() const
            {
                return this->Key;
            }
            
            void swap_Key( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Key);
            }
            
            bool has_LineOffset() const
            {
                return LineOffset_successful;
            }
            
            bool get_LineOffset( float & _value ) const
            {
                if( LineOffset_successful == false )
                {
                    return false;
                }
            
                _value = this->LineOffset;
            
                return true;
            }
            
            bool swap_LineOffset( float & _value ) const
            {
                if( LineOffset_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->LineOffset);
            
                return true;
            }
            
            bool has_Value() const
            {
                return Value_successful;
            }
            
            bool get_Value( Menge::WString & _value ) const
            {
                if( Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Value;
            
                return true;
            }
            
            bool swap_Value( Menge::WString & _value ) const
            {
                if( Value_successful == false )
                {
                    return false;
                }
            
                std::swap( _value, this->Value);
            
                return true;
            }
            
        protected:
            void _parseData( const unsigned char * _buff, size_t _size, size_t & _read ) override;
            void _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            void _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            void _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            void _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool CharOffset_successful;
            mutable float CharOffset;
            bool Font_successful;
            mutable Menge::ConstString Font;
            mutable Menge::ConstString Key;
            bool LineOffset_successful;
            mutable float LineOffset;
            bool Value_successful;
            mutable Menge::WString Value;
        };
        
    protected:
    protected:
    public:
        typedef stdex::auto_array<Meta_Text> TVectorMeta_Text;
    
        const TVectorMeta_Text & get_IncludesText() const
        {
            return this->includes_Meta_Text;
        }
    
    protected:
        TVectorMeta_Text includes_Meta_Text;
    };
    
}
