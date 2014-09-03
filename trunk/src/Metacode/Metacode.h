#   pragma once

#   include "Metatype.h"

#   include <metabuf/Metabuf.hpp>

namespace Metacode
{
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
        
        template<class C, class M>
        void method_Name( C * _self, M _method ) const
        {
            (_self->*_method)( this->Name );
        }
        
    protected:
        bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_Include
            : public Metabuf::Metadata
        { 
        public:
            Meta_Include();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
            template<class C, class M>
            void method_Path( C * _self, M _method ) const
            {
                (_self->*_method)( this->Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Path;
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
            
            template<class C, class M>
            void method_Name( C * _self, M _method ) const
            {
                (_self->*_method)( this->Name );
            }
            
            const Menge::ConstString & get_Type() const
            {
                return this->Type;
            }
            
            void swap_Type( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Type);
            }
            
            template<class C, class M>
            void method_Type( C * _self, M _method ) const
            {
                (_self->*_method)( this->Type );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Name;
            mutable Menge::ConstString Type;
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
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Sequence
                : public Metabuf::Metadata
            { 
            public:
                Meta_Sequence();
            
            public:
                uint32_t getId() const override;
            
            public:
                const float & get_Delay() const
                {
                    return this->Delay;
                }
                
                void swap_Delay( float & _value ) const
                {
                    std::swap( _value, this->Delay);
                }
                
                template<class C, class M>
                void method_Delay( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Delay );
                }
                
                const Menge::ConstString & get_ResourceImageName() const
                {
                    return this->ResourceImageName;
                }
                
                void swap_ResourceImageName( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->ResourceImageName);
                }
                
                template<class C, class M>
                void method_ResourceImageName( C * _self, M _method ) const
                {
                    (_self->*_method)( this->ResourceImageName );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
        };
        
        class Meta_ResourceCal3dMesh
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCal3dMesh();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
        };
        
        class Meta_ResourceCal3dSkeleton
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCal3dSkeleton();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
        };
        
        class Meta_ResourceCursorICO
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCursorICO();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
        };
        
        class Meta_ResourceCursorSystem
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCursorSystem();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
        };
        
        class Meta_ResourceEmitter
            : public Meta_Resource
        { 
        public:
            Meta_ResourceEmitter();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Container_Name() const
            {
                return this->Container_Name;
            }
            
            void swap_Container_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Container_Name);
            }
            
            template<class C, class M>
            void method_Container_Name( C * _self, M _method )
            {
                (_self->*_method)( this->Container_Name );
            }
            
            const Menge::ConstString & get_Emitter_Name() const
            {
                return this->Emitter_Name;
            }
            
            void swap_Emitter_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Emitter_Name);
            }
            
            template<class C, class M>
            void method_Emitter_Name( C * _self, M _method )
            {
                (_self->*_method)( this->Emitter_Name );
            }
            
            const bool & get_EmitterRelative_Value() const
            {
                return this->EmitterRelative_Value;
            }
            
            void swap_EmitterRelative_Value( bool & _value ) const
            {
                std::swap(_value, this->EmitterRelative_Value);
            }
            
            template<class C, class M>
            void method_EmitterRelative_Value( C * _self, M _method )
            {
                (_self->*_method)( this->EmitterRelative_Value );
            }
            
            bool has_Offset_Value() const
            {
                return Offset_Value_successful;
            }
            
            bool get_Offset_Value( mt::vec2f & _value ) const
            {
                if( Offset_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Offset_Value;
            
                return true;
            }
            
            bool swap_Offset_Value( mt::vec2f & _value ) const
            {
                if( Offset_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Offset_Value);
            
                return true;
            }
            
            template<class C, class M>
            void method_Offset_Value( C * _self, M _method )
            {
                if( Offset_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Offset_Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Container_Name;
            mutable Menge::ConstString Emitter_Name;
            mutable bool EmitterRelative_Value;
            bool Offset_Value_successful;
            mutable mt::vec2f Offset_Value;
        };
        
        class Meta_ResourceEmitterContainer
            : public Meta_Resource
        { 
        public:
            Meta_ResourceEmitterContainer();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
            const Menge::ConstString & get_Folder_Path() const
            {
                return this->Folder_Path;
            }
            
            void swap_Folder_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Folder_Path);
            }
            
            template<class C, class M>
            void method_Folder_Path( C * _self, M _method )
            {
                (_self->*_method)( this->Folder_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
            mutable Menge::ConstString Folder_Path;
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
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString File_Path;
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
            
            template<class C, class M>
            void method_File_Codec( C * _self, M _method )
            {
                if( File_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Codec );
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
            
            template<class C, class M>
            void method_File_Converter( C * _self, M _method )
            {
                if( File_Converter_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Converter );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable Menge::ConstString File_Path;
        };
        
        class Meta_ResourceImageCombineRGBAndAlpha
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageCombineRGBAndAlpha();
        
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
            
            template<class C, class M>
            void method_File_Alpha( C * _self, M _method )
            {
                if( File_Alpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Alpha );
            }
            
            bool has_File_CodecAlpha() const
            {
                return File_CodecAlpha_successful;
            }
            
            bool get_File_CodecAlpha( Menge::ConstString & _value ) const
            {
                if( File_CodecAlpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_CodecAlpha;
            
                return true;
            }
            
            bool swap_File_CodecAlpha( Menge::ConstString & _value ) const
            {
                if( File_CodecAlpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_CodecAlpha);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_CodecAlpha( C * _self, M _method )
            {
                if( File_CodecAlpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_CodecAlpha );
            }
            
            bool has_File_CodecRGB() const
            {
                return File_CodecRGB_successful;
            }
            
            bool get_File_CodecRGB( Menge::ConstString & _value ) const
            {
                if( File_CodecRGB_successful == false )
                {
                    return false;
                }
            
                _value = this->File_CodecRGB;
            
                return true;
            }
            
            bool swap_File_CodecRGB( Menge::ConstString & _value ) const
            {
                if( File_CodecRGB_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_CodecRGB);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_CodecRGB( C * _self, M _method )
            {
                if( File_CodecRGB_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_CodecRGB );
            }
            
            bool has_File_MaxSize() const
            {
                return File_MaxSize_successful;
            }
            
            bool get_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                _value = this->File_MaxSize;
            
                return true;
            }
            
            bool swap_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_MaxSize);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_MaxSize( C * _self, M _method )
            {
                if( File_MaxSize_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_MaxSize );
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
            
            template<class C, class M>
            void method_File_Offset( C * _self, M _method )
            {
                if( File_Offset_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Offset );
            }
            
            bool has_File_PathAlpha() const
            {
                return File_PathAlpha_successful;
            }
            
            bool get_File_PathAlpha( Menge::ConstString & _value ) const
            {
                if( File_PathAlpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_PathAlpha;
            
                return true;
            }
            
            bool swap_File_PathAlpha( Menge::ConstString & _value ) const
            {
                if( File_PathAlpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_PathAlpha);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_PathAlpha( C * _self, M _method )
            {
                if( File_PathAlpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_PathAlpha );
            }
            
            bool has_File_PathRGB() const
            {
                return File_PathRGB_successful;
            }
            
            bool get_File_PathRGB( Menge::ConstString & _value ) const
            {
                if( File_PathRGB_successful == false )
                {
                    return false;
                }
            
                _value = this->File_PathRGB;
            
                return true;
            }
            
            bool swap_File_PathRGB( Menge::ConstString & _value ) const
            {
                if( File_PathRGB_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_PathRGB);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_PathRGB( C * _self, M _method )
            {
                if( File_PathRGB_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_PathRGB );
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
            
            template<class C, class M>
            void method_File_Size( C * _self, M _method )
            {
                if( File_Size_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Size );
            }
            
            bool has_File_UV() const
            {
                return File_UV_successful;
            }
            
            bool get_File_UV( mt::vec4f & _value ) const
            {
                if( File_UV_successful == false )
                {
                    return false;
                }
            
                _value = this->File_UV;
            
                return true;
            }
            
            bool swap_File_UV( mt::vec4f & _value ) const
            {
                if( File_UV_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_UV);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_UV( C * _self, M _method )
            {
                if( File_UV_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_UV );
            }
            
            bool has_File_WrapX() const
            {
                return File_WrapX_successful;
            }
            
            bool get_File_WrapX( bool & _value ) const
            {
                if( File_WrapX_successful == false )
                {
                    return false;
                }
            
                _value = this->File_WrapX;
            
                return true;
            }
            
            bool swap_File_WrapX( bool & _value ) const
            {
                if( File_WrapX_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_WrapX);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_WrapX( C * _self, M _method )
            {
                if( File_WrapX_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_WrapX );
            }
            
            bool has_File_WrapY() const
            {
                return File_WrapY_successful;
            }
            
            bool get_File_WrapY( bool & _value ) const
            {
                if( File_WrapY_successful == false )
                {
                    return false;
                }
            
                _value = this->File_WrapY;
            
                return true;
            }
            
            bool swap_File_WrapY( bool & _value ) const
            {
                if( File_WrapY_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_WrapY);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_WrapY( C * _self, M _method )
            {
                if( File_WrapY_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_WrapY );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_CodecAlpha_successful;
            mutable Menge::ConstString File_CodecAlpha;
            bool File_CodecRGB_successful;
            mutable Menge::ConstString File_CodecRGB;
            bool File_MaxSize_successful;
            mutable mt::vec2f File_MaxSize;
            bool File_Offset_successful;
            mutable mt::vec2f File_Offset;
            bool File_PathAlpha_successful;
            mutable Menge::ConstString File_PathAlpha;
            bool File_PathRGB_successful;
            mutable Menge::ConstString File_PathRGB;
            bool File_Size_successful;
            mutable mt::vec2f File_Size;
            bool File_UV_successful;
            mutable mt::vec4f File_UV;
            bool File_WrapX_successful;
            mutable bool File_WrapX;
            bool File_WrapY_successful;
            mutable bool File_WrapY;
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
            
            template<class C, class M>
            void method_File_Codec( C * _self, M _method )
            {
                if( File_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Codec );
            }
            
            bool has_File_MaxSize() const
            {
                return File_MaxSize_successful;
            }
            
            bool get_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                _value = this->File_MaxSize;
            
                return true;
            }
            
            bool swap_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_MaxSize);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_MaxSize( C * _self, M _method )
            {
                if( File_MaxSize_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_MaxSize );
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
            
            template<class C, class M>
            void method_File_NoExist( C * _self, M _method )
            {
                if( File_NoExist_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_NoExist );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_MaxSize_successful;
            mutable mt::vec2f File_MaxSize;
            bool File_NoExist_successful;
            mutable bool File_NoExist;
            mutable Menge::ConstString File_Path;
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
            
            template<class C, class M>
            void method_File_Alpha( C * _self, M _method )
            {
                if( File_Alpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Alpha );
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
            
            template<class C, class M>
            void method_File_Codec( C * _self, M _method )
            {
                if( File_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Codec );
            }
            
            bool has_File_MaxSize() const
            {
                return File_MaxSize_successful;
            }
            
            bool get_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                _value = this->File_MaxSize;
            
                return true;
            }
            
            bool swap_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_MaxSize);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_MaxSize( C * _self, M _method )
            {
                if( File_MaxSize_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_MaxSize );
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
            
            template<class C, class M>
            void method_File_NoExist( C * _self, M _method )
            {
                if( File_NoExist_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_NoExist );
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
            
            template<class C, class M>
            void method_File_Offset( C * _self, M _method )
            {
                if( File_Offset_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Offset );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
            bool has_File_Rotate() const
            {
                return File_Rotate_successful;
            }
            
            bool get_File_Rotate( bool & _value ) const
            {
                if( File_Rotate_successful == false )
                {
                    return false;
                }
            
                _value = this->File_Rotate;
            
                return true;
            }
            
            bool swap_File_Rotate( bool & _value ) const
            {
                if( File_Rotate_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_Rotate);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_Rotate( C * _self, M _method )
            {
                if( File_Rotate_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Rotate );
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
            
            template<class C, class M>
            void method_File_Size( C * _self, M _method )
            {
                if( File_Size_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Size );
            }
            
            bool has_File_UV() const
            {
                return File_UV_successful;
            }
            
            bool get_File_UV( mt::vec4f & _value ) const
            {
                if( File_UV_successful == false )
                {
                    return false;
                }
            
                _value = this->File_UV;
            
                return true;
            }
            
            bool swap_File_UV( mt::vec4f & _value ) const
            {
                if( File_UV_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_UV);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_UV( C * _self, M _method )
            {
                if( File_UV_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_UV );
            }
            
            bool has_File_WrapX() const
            {
                return File_WrapX_successful;
            }
            
            bool get_File_WrapX( bool & _value ) const
            {
                if( File_WrapX_successful == false )
                {
                    return false;
                }
            
                _value = this->File_WrapX;
            
                return true;
            }
            
            bool swap_File_WrapX( bool & _value ) const
            {
                if( File_WrapX_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_WrapX);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_WrapX( C * _self, M _method )
            {
                if( File_WrapX_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_WrapX );
            }
            
            bool has_File_WrapY() const
            {
                return File_WrapY_successful;
            }
            
            bool get_File_WrapY( bool & _value ) const
            {
                if( File_WrapY_successful == false )
                {
                    return false;
                }
            
                _value = this->File_WrapY;
            
                return true;
            }
            
            bool swap_File_WrapY( bool & _value ) const
            {
                if( File_WrapY_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_WrapY);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_WrapY( C * _self, M _method )
            {
                if( File_WrapY_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_WrapY );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_MaxSize_successful;
            mutable mt::vec2f File_MaxSize;
            bool File_NoExist_successful;
            mutable bool File_NoExist;
            bool File_Offset_successful;
            mutable mt::vec2f File_Offset;
            mutable Menge::ConstString File_Path;
            bool File_Rotate_successful;
            mutable bool File_Rotate;
            bool File_Size_successful;
            mutable mt::vec2f File_Size;
            bool File_UV_successful;
            mutable mt::vec4f File_UV;
            bool File_WrapX_successful;
            mutable bool File_WrapX;
            bool File_WrapY_successful;
            mutable bool File_WrapY;
        };
        
        class Meta_ResourceImageMultiplyRGBAndAlpha
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageMultiplyRGBAndAlpha();
        
        public:
            uint32_t getId() const override;
        
        public:
            bool has_File_CodecAlpha() const
            {
                return File_CodecAlpha_successful;
            }
            
            bool get_File_CodecAlpha( Menge::ConstString & _value ) const
            {
                if( File_CodecAlpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_CodecAlpha;
            
                return true;
            }
            
            bool swap_File_CodecAlpha( Menge::ConstString & _value ) const
            {
                if( File_CodecAlpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_CodecAlpha);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_CodecAlpha( C * _self, M _method )
            {
                if( File_CodecAlpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_CodecAlpha );
            }
            
            bool has_File_CodecRGB() const
            {
                return File_CodecRGB_successful;
            }
            
            bool get_File_CodecRGB( Menge::ConstString & _value ) const
            {
                if( File_CodecRGB_successful == false )
                {
                    return false;
                }
            
                _value = this->File_CodecRGB;
            
                return true;
            }
            
            bool swap_File_CodecRGB( Menge::ConstString & _value ) const
            {
                if( File_CodecRGB_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_CodecRGB);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_CodecRGB( C * _self, M _method )
            {
                if( File_CodecRGB_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_CodecRGB );
            }
            
            bool has_File_MaxSize() const
            {
                return File_MaxSize_successful;
            }
            
            bool get_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                _value = this->File_MaxSize;
            
                return true;
            }
            
            bool swap_File_MaxSize( mt::vec2f & _value ) const
            {
                if( File_MaxSize_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_MaxSize);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_MaxSize( C * _self, M _method )
            {
                if( File_MaxSize_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_MaxSize );
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
            
            template<class C, class M>
            void method_File_Offset( C * _self, M _method )
            {
                if( File_Offset_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Offset );
            }
            
            bool has_File_PathAlpha() const
            {
                return File_PathAlpha_successful;
            }
            
            bool get_File_PathAlpha( Menge::ConstString & _value ) const
            {
                if( File_PathAlpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_PathAlpha;
            
                return true;
            }
            
            bool swap_File_PathAlpha( Menge::ConstString & _value ) const
            {
                if( File_PathAlpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_PathAlpha);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_PathAlpha( C * _self, M _method )
            {
                if( File_PathAlpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_PathAlpha );
            }
            
            bool has_File_PathRGB() const
            {
                return File_PathRGB_successful;
            }
            
            bool get_File_PathRGB( Menge::ConstString & _value ) const
            {
                if( File_PathRGB_successful == false )
                {
                    return false;
                }
            
                _value = this->File_PathRGB;
            
                return true;
            }
            
            bool swap_File_PathRGB( Menge::ConstString & _value ) const
            {
                if( File_PathRGB_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_PathRGB);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_PathRGB( C * _self, M _method )
            {
                if( File_PathRGB_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_PathRGB );
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
            
            template<class C, class M>
            void method_File_Size( C * _self, M _method )
            {
                if( File_Size_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Size );
            }
            
            bool has_File_UVAlpha() const
            {
                return File_UVAlpha_successful;
            }
            
            bool get_File_UVAlpha( mt::vec4f & _value ) const
            {
                if( File_UVAlpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_UVAlpha;
            
                return true;
            }
            
            bool swap_File_UVAlpha( mt::vec4f & _value ) const
            {
                if( File_UVAlpha_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_UVAlpha);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_UVAlpha( C * _self, M _method )
            {
                if( File_UVAlpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_UVAlpha );
            }
            
            bool has_File_UVRGB() const
            {
                return File_UVRGB_successful;
            }
            
            bool get_File_UVRGB( mt::vec4f & _value ) const
            {
                if( File_UVRGB_successful == false )
                {
                    return false;
                }
            
                _value = this->File_UVRGB;
            
                return true;
            }
            
            bool swap_File_UVRGB( mt::vec4f & _value ) const
            {
                if( File_UVRGB_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_UVRGB);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_UVRGB( C * _self, M _method )
            {
                if( File_UVRGB_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_UVRGB );
            }
            
            bool has_File_WrapX() const
            {
                return File_WrapX_successful;
            }
            
            bool get_File_WrapX( bool & _value ) const
            {
                if( File_WrapX_successful == false )
                {
                    return false;
                }
            
                _value = this->File_WrapX;
            
                return true;
            }
            
            bool swap_File_WrapX( bool & _value ) const
            {
                if( File_WrapX_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_WrapX);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_WrapX( C * _self, M _method )
            {
                if( File_WrapX_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_WrapX );
            }
            
            bool has_File_WrapY() const
            {
                return File_WrapY_successful;
            }
            
            bool get_File_WrapY( bool & _value ) const
            {
                if( File_WrapY_successful == false )
                {
                    return false;
                }
            
                _value = this->File_WrapY;
            
                return true;
            }
            
            bool swap_File_WrapY( bool & _value ) const
            {
                if( File_WrapY_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_WrapY);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_WrapY( C * _self, M _method )
            {
                if( File_WrapY_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_WrapY );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_CodecAlpha_successful;
            mutable Menge::ConstString File_CodecAlpha;
            bool File_CodecRGB_successful;
            mutable Menge::ConstString File_CodecRGB;
            bool File_MaxSize_successful;
            mutable mt::vec2f File_MaxSize;
            bool File_Offset_successful;
            mutable mt::vec2f File_Offset;
            bool File_PathAlpha_successful;
            mutable Menge::ConstString File_PathAlpha;
            bool File_PathRGB_successful;
            mutable Menge::ConstString File_PathRGB;
            bool File_Size_successful;
            mutable mt::vec2f File_Size;
            bool File_UVAlpha_successful;
            mutable mt::vec4f File_UVAlpha;
            bool File_UVRGB_successful;
            mutable mt::vec4f File_UVRGB;
            bool File_WrapX_successful;
            mutable bool File_WrapX;
            bool File_WrapY_successful;
            mutable bool File_WrapY;
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
            
            template<class C, class M>
            void method_Color_Value( C * _self, M _method )
            {
                (_self->*_method)( this->Color_Value );
            }
            
            const mt::vec2f & get_Size_Value() const
            {
                return this->Size_Value;
            }
            
            void swap_Size_Value( mt::vec2f & _value ) const
            {
                std::swap(_value, this->Size_Value);
            }
            
            template<class C, class M>
            void method_Size_Value( C * _self, M _method )
            {
                (_self->*_method)( this->Size_Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_Image_Alpha( C * _self, M _method )
            {
                if( Image_Alpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Image_Alpha );
            }
            
            bool has_Image_MaxSize() const
            {
                return Image_MaxSize_successful;
            }
            
            bool get_Image_MaxSize( mt::vec2f & _value ) const
            {
                if( Image_MaxSize_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_MaxSize;
            
                return true;
            }
            
            bool swap_Image_MaxSize( mt::vec2f & _value ) const
            {
                if( Image_MaxSize_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_MaxSize);
            
                return true;
            }
            
            template<class C, class M>
            void method_Image_MaxSize( C * _self, M _method )
            {
                if( Image_MaxSize_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Image_MaxSize );
            }
            
            const Menge::ConstString & get_Image_Name() const
            {
                return this->Image_Name;
            }
            
            void swap_Image_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Image_Name);
            }
            
            template<class C, class M>
            void method_Image_Name( C * _self, M _method )
            {
                (_self->*_method)( this->Image_Name );
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
            
            template<class C, class M>
            void method_Image_Offset( C * _self, M _method )
            {
                if( Image_Offset_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Image_Offset );
            }
            
            bool has_Image_Rotate() const
            {
                return Image_Rotate_successful;
            }
            
            bool get_Image_Rotate( bool & _value ) const
            {
                if( Image_Rotate_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Rotate;
            
                return true;
            }
            
            bool swap_Image_Rotate( bool & _value ) const
            {
                if( Image_Rotate_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Rotate);
            
                return true;
            }
            
            template<class C, class M>
            void method_Image_Rotate( C * _self, M _method )
            {
                if( Image_Rotate_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Image_Rotate );
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
            
            template<class C, class M>
            void method_Image_Size( C * _self, M _method )
            {
                if( Image_Size_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Image_Size );
            }
            
            const mt::vec4f & get_Image_UV() const
            {
                return this->Image_UV;
            }
            
            void swap_Image_UV( mt::vec4f & _value ) const
            {
                std::swap(_value, this->Image_UV);
            }
            
            template<class C, class M>
            void method_Image_UV( C * _self, M _method )
            {
                (_self->*_method)( this->Image_UV );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool Image_Alpha_successful;
            mutable bool Image_Alpha;
            bool Image_MaxSize_successful;
            mutable mt::vec2f Image_MaxSize;
            mutable Menge::ConstString Image_Name;
            bool Image_Offset_successful;
            mutable mt::vec2f Image_Offset;
            bool Image_Rotate_successful;
            mutable bool Image_Rotate;
            bool Image_Size_successful;
            mutable mt::vec2f Image_Size;
            mutable mt::vec4f Image_UV;
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
            
            template<class C, class M>
            void method_Internal_Group( C * _self, M _method )
            {
                (_self->*_method)( this->Internal_Group );
            }
            
            const Menge::ConstString & get_Internal_Name() const
            {
                return this->Internal_Name;
            }
            
            void swap_Internal_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Internal_Name);
            }
            
            template<class C, class M>
            void method_Internal_Name( C * _self, M _method )
            {
                (_self->*_method)( this->Internal_Name );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_File_Converter( C * _self, M _method )
            {
                if( File_Converter_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Converter );
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
            
            template<class C, class M>
            void method_File_Dataflow( C * _self, M _method )
            {
                if( File_Dataflow_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Dataflow );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
            const Menge::ConstString & get_Image_Resource() const
            {
                return this->Image_Resource;
            }
            
            void swap_Image_Resource( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->Image_Resource);
            }
            
            template<class C, class M>
            void method_Image_Resource( C * _self, M _method )
            {
                (_self->*_method)( this->Image_Resource );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            bool File_Dataflow_successful;
            mutable Menge::ConstString File_Dataflow;
            mutable Menge::ConstString File_Path;
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
            
            template<class C, class M>
            void method_Anchor_Point( C * _self, M _method )
            {
                if( Anchor_Point_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Anchor_Point );
            }
            
            bool has_Duration_Value() const
            {
                return Duration_Value_successful;
            }
            
            bool get_Duration_Value( float & _value ) const
            {
                if( Duration_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Duration_Value;
            
                return true;
            }
            
            bool swap_Duration_Value( float & _value ) const
            {
                if( Duration_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Duration_Value);
            
                return true;
            }
            
            template<class C, class M>
            void method_Duration_Value( C * _self, M _method )
            {
                if( Duration_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Duration_Value );
            }
            
            bool has_FrameDuration_Value() const
            {
                return FrameDuration_Value_successful;
            }
            
            bool get_FrameDuration_Value( float & _value ) const
            {
                if( FrameDuration_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->FrameDuration_Value;
            
                return true;
            }
            
            bool swap_FrameDuration_Value( float & _value ) const
            {
                if( FrameDuration_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->FrameDuration_Value);
            
                return true;
            }
            
            template<class C, class M>
            void method_FrameDuration_Value( C * _self, M _method )
            {
                if( FrameDuration_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->FrameDuration_Value );
            }
            
            bool has_Height_Value() const
            {
                return Height_Value_successful;
            }
            
            bool get_Height_Value( float & _value ) const
            {
                if( Height_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Height_Value;
            
                return true;
            }
            
            bool swap_Height_Value( float & _value ) const
            {
                if( Height_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Height_Value);
            
                return true;
            }
            
            template<class C, class M>
            void method_Height_Value( C * _self, M _method )
            {
                if( Height_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Height_Value );
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
            
            template<class C, class M>
            void method_KeyFramesPackPath_Codec( C * _self, M _method )
            {
                if( KeyFramesPackPath_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->KeyFramesPackPath_Codec );
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
            
            template<class C, class M>
            void method_KeyFramesPackPath_Converter( C * _self, M _method )
            {
                if( KeyFramesPackPath_Converter_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->KeyFramesPackPath_Converter );
            }
            
            bool has_KeyFramesPackPath_Path() const
            {
                return KeyFramesPackPath_Path_successful;
            }
            
            bool get_KeyFramesPackPath_Path( Menge::ConstString & _value ) const
            {
                if( KeyFramesPackPath_Path_successful == false )
                {
                    return false;
                }
            
                _value = this->KeyFramesPackPath_Path;
            
                return true;
            }
            
            bool swap_KeyFramesPackPath_Path( Menge::ConstString & _value ) const
            {
                if( KeyFramesPackPath_Path_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->KeyFramesPackPath_Path);
            
                return true;
            }
            
            template<class C, class M>
            void method_KeyFramesPackPath_Path( C * _self, M _method )
            {
                if( KeyFramesPackPath_Path_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->KeyFramesPackPath_Path );
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
            
            template<class C, class M>
            void method_Loop_Segment( C * _self, M _method )
            {
                if( Loop_Segment_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Loop_Segment );
            }
            
            bool has_Width_Value() const
            {
                return Width_Value_successful;
            }
            
            bool get_Width_Value( float & _value ) const
            {
                if( Width_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Width_Value;
            
                return true;
            }
            
            bool swap_Width_Value( float & _value ) const
            {
                if( Width_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Width_Value);
            
                return true;
            }
            
            template<class C, class M>
            void method_Width_Value( C * _self, M _method )
            {
                if( Width_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Width_Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_MovieCamera3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_MovieCamera3D();
            
            public:
                uint32_t getId() const override;
            
            public:
                const float & get_CameraAspect() const
                {
                    return this->CameraAspect;
                }
                
                void swap_CameraAspect( float & _value ) const
                {
                    std::swap( _value, this->CameraAspect);
                }
                
                template<class C, class M>
                void method_CameraAspect( C * _self, M _method ) const
                {
                    (_self->*_method)( this->CameraAspect );
                }
                
                const float & get_CameraFOV() const
                {
                    return this->CameraFOV;
                }
                
                void swap_CameraFOV( float & _value ) const
                {
                    std::swap( _value, this->CameraFOV);
                }
                
                template<class C, class M>
                void method_CameraFOV( C * _self, M _method ) const
                {
                    (_self->*_method)( this->CameraFOV );
                }
                
                const mt::vec3f & get_CameraInterest() const
                {
                    return this->CameraInterest;
                }
                
                void swap_CameraInterest( mt::vec3f & _value ) const
                {
                    std::swap( _value, this->CameraInterest);
                }
                
                template<class C, class M>
                void method_CameraInterest( C * _self, M _method ) const
                {
                    (_self->*_method)( this->CameraInterest );
                }
                
                const mt::vec3f & get_CameraPosition() const
                {
                    return this->CameraPosition;
                }
                
                void swap_CameraPosition( mt::vec3f & _value ) const
                {
                    std::swap( _value, this->CameraPosition);
                }
                
                template<class C, class M>
                void method_CameraPosition( C * _self, M _method ) const
                {
                    (_self->*_method)( this->CameraPosition );
                }
                
                const float & get_Height() const
                {
                    return this->Height;
                }
                
                void swap_Height( float & _value ) const
                {
                    std::swap( _value, this->Height);
                }
                
                template<class C, class M>
                void method_Height( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Height );
                }
                
                const float & get_Width() const
                {
                    return this->Width;
                }
                
                void swap_Width( float & _value ) const
                {
                    std::swap( _value, this->Width);
                }
                
                template<class C, class M>
                void method_Width( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Width );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
                
                template<class C, class M>
                void method_BlendingMode( C * _self, M _method ) const
                {
                    if( BlendingMode_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->BlendingMode );
                }
                
                const float & get_In() const
                {
                    return this->In;
                }
                
                void swap_In( float & _value ) const
                {
                    std::swap( _value, this->In);
                }
                
                template<class C, class M>
                void method_In( C * _self, M _method ) const
                {
                    (_self->*_method)( this->In );
                }
                
                const uint32_t & get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( uint32_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                template<class C, class M>
                void method_Index( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Index );
                }
                
                const Menge::ConstString & get_Name() const
                {
                    return this->Name;
                }
                
                void swap_Name( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Name);
                }
                
                template<class C, class M>
                void method_Name( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Name );
                }
                
                const float & get_Out() const
                {
                    return this->Out;
                }
                
                void swap_Out( float & _value ) const
                {
                    std::swap( _value, this->Out);
                }
                
                template<class C, class M>
                void method_Out( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Out );
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
                
                template<class C, class M>
                void method_Parent( C * _self, M _method ) const
                {
                    if( Parent_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Parent );
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
                
                template<class C, class M>
                void method_PlayCount( C * _self, M _method ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->PlayCount );
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
                
                template<class C, class M>
                void method_Polygon( C * _self, M _method ) const
                {
                    if( Polygon_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Polygon );
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
                
                template<class C, class M>
                void method_Shape( C * _self, M _method ) const
                {
                    if( Shape_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Shape );
                }
                
                const Menge::ConstString & get_Source() const
                {
                    return this->Source;
                }
                
                void swap_Source( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Source);
                }
                
                template<class C, class M>
                void method_Source( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Source );
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
                
                template<class C, class M>
                void method_StartInterval( C * _self, M _method ) const
                {
                    if( StartInterval_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->StartInterval );
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
                
                template<class C, class M>
                void method_Stretch( C * _self, M _method ) const
                {
                    if( Stretch_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Stretch );
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
                
                template<class C, class M>
                void method_Switch( C * _self, M _method ) const
                {
                    if( Switch_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Switch );
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
                
                template<class C, class M>
                void method_TimeRemap( C * _self, M _method ) const
                {
                    if( TimeRemap_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->TimeRemap );
                }
                
                const Menge::ConstString & get_Type() const
                {
                    return this->Type;
                }
                
                void swap_Type( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Type);
                }
                
                template<class C, class M>
                void method_Type( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Type );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool BlendingMode_successful;
                mutable Menge::ConstString BlendingMode;
                mutable float In;
                mutable uint32_t Index;
                mutable Menge::ConstString Name;
                mutable float Out;
                bool Parent_successful;
                mutable uint32_t Parent;
                bool PlayCount_successful;
                mutable uint32_t PlayCount;
                bool Polygon_successful;
                mutable bool Polygon;
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
                
                template<class C, class M>
                void method_BlendingMode( C * _self, M _method ) const
                {
                    if( BlendingMode_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->BlendingMode );
                }
                
                const float & get_In() const
                {
                    return this->In;
                }
                
                void swap_In( float & _value ) const
                {
                    std::swap( _value, this->In);
                }
                
                template<class C, class M>
                void method_In( C * _self, M _method ) const
                {
                    (_self->*_method)( this->In );
                }
                
                const uint32_t & get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( uint32_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                template<class C, class M>
                void method_Index( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Index );
                }
                
                const Menge::ConstString & get_Name() const
                {
                    return this->Name;
                }
                
                void swap_Name( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Name);
                }
                
                template<class C, class M>
                void method_Name( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Name );
                }
                
                const float & get_Out() const
                {
                    return this->Out;
                }
                
                void swap_Out( float & _value ) const
                {
                    std::swap( _value, this->Out);
                }
                
                template<class C, class M>
                void method_Out( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Out );
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
                
                template<class C, class M>
                void method_Parent( C * _self, M _method ) const
                {
                    if( Parent_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Parent );
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
                
                template<class C, class M>
                void method_PlayCount( C * _self, M _method ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->PlayCount );
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
                
                template<class C, class M>
                void method_Shape( C * _self, M _method ) const
                {
                    if( Shape_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Shape );
                }
                
                const Menge::ConstString & get_Source() const
                {
                    return this->Source;
                }
                
                void swap_Source( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Source);
                }
                
                template<class C, class M>
                void method_Source( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Source );
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
                
                template<class C, class M>
                void method_StartInterval( C * _self, M _method ) const
                {
                    if( StartInterval_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->StartInterval );
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
                
                template<class C, class M>
                void method_Stretch( C * _self, M _method ) const
                {
                    if( Stretch_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Stretch );
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
                
                template<class C, class M>
                void method_Switch( C * _self, M _method ) const
                {
                    if( Switch_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Switch );
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
                
                template<class C, class M>
                void method_TimeRemap( C * _self, M _method ) const
                {
                    if( TimeRemap_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->TimeRemap );
                }
                
                const Menge::ConstString & get_Type() const
                {
                    return this->Type;
                }
                
                void swap_Type( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Type);
                }
                
                template<class C, class M>
                void method_Type( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Type );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool BlendingMode_successful;
                mutable Menge::ConstString BlendingMode;
                mutable float In;
                mutable uint32_t Index;
                mutable Menge::ConstString Name;
                mutable float Out;
                bool Parent_successful;
                mutable uint32_t Parent;
                bool PlayCount_successful;
                mutable uint32_t PlayCount;
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
            bool Duration_Value_successful;
            mutable float Duration_Value;
            bool FrameDuration_Value_successful;
            mutable float FrameDuration_Value;
            bool Height_Value_successful;
            mutable float Height_Value;
            bool KeyFramesPackPath_Codec_successful;
            mutable Menge::ConstString KeyFramesPackPath_Codec;
            bool KeyFramesPackPath_Converter_successful;
            mutable Menge::ConstString KeyFramesPackPath_Converter;
            bool KeyFramesPackPath_Path_successful;
            mutable Menge::ConstString KeyFramesPackPath_Path;
            bool Loop_Segment_successful;
            mutable mt::vec2f Loop_Segment;
            bool Width_Value_successful;
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
            const uint32_t & get_AtlasCount_Value() const
            {
                return this->AtlasCount_Value;
            }
            
            void swap_AtlasCount_Value( uint32_t & _value ) const
            {
                std::swap(_value, this->AtlasCount_Value);
            }
            
            template<class C, class M>
            void method_AtlasCount_Value( C * _self, M _method )
            {
                (_self->*_method)( this->AtlasCount_Value );
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
            
            template<class C, class M>
            void method_File_Converter( C * _self, M _method )
            {
                if( File_Converter_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Converter );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Atlas
                : public Metabuf::Metadata
            { 
            public:
                Meta_Atlas();
            
            public:
                uint32_t getId() const override;
            
            public:
                const uint32_t & get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( uint32_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                template<class C, class M>
                void method_Index( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Index );
                }
                
                const Menge::ConstString & get_ResourceName() const
                {
                    return this->ResourceName;
                }
                
                void swap_ResourceName( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->ResourceName);
                }
                
                template<class C, class M>
                void method_ResourceName( C * _self, M _method ) const
                {
                    (_self->*_method)( this->ResourceName );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            mutable Menge::ConstString File_Path;
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
            
            template<class C, class M>
            void method_Loop_Value( C * _self, M _method )
            {
                if( Loop_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Loop_Value );
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
            
            template<class C, class M>
            void method_Shuffle_Value( C * _self, M _method )
            {
                if( Shuffle_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Shuffle_Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
                    
                    template<class C, class M>
                    void method_Codec( C * _self, M _method ) const
                    {
                        if( Codec_successful == false )
                        {
                            return;
                        }
                    
                        (_self->*_method)( this->Codec );
                    }
                    
                    const Menge::ConstString & get_File() const
                    {
                        return this->File;
                    }
                    
                    void swap_File( Menge::ConstString & _value ) const
                    {
                        std::swap( _value, this->File);
                    }
                    
                    template<class C, class M>
                    void method_File( C * _self, M _method ) const
                    {
                        (_self->*_method)( this->File );
                    }
                    
                protected:
                    bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                    bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                    bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                    bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
                public:
                protected:
                protected:
                    bool Codec_successful;
                    mutable Menge::ConstString Codec;
                    mutable Menge::ConstString File;
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
            
            template<class C, class M>
            void method_Polygon_Value( C * _self, M _method )
            {
                (_self->*_method)( this->Polygon_Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_DefaultVolume_Value( C * _self, M _method )
            {
                if( DefaultVolume_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->DefaultVolume_Value );
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
            
            template<class C, class M>
            void method_File_Codec( C * _self, M _method )
            {
                if( File_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Codec );
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
            
            template<class C, class M>
            void method_File_Converter( C * _self, M _method )
            {
                if( File_Converter_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Converter );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
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
            
            template<class C, class M>
            void method_IsStreamable_Value( C * _self, M _method )
            {
                if( IsStreamable_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->IsStreamable_Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool DefaultVolume_Value_successful;
            mutable float DefaultVolume_Value;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable Menge::ConstString File_Path;
            bool IsStreamable_Value_successful;
            mutable bool IsStreamable_Value;
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
            
            template<class C, class M>
            void method_File_Alpha( C * _self, M _method )
            {
                if( File_Alpha_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Alpha );
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
            
            template<class C, class M>
            void method_File_Codec( C * _self, M _method )
            {
                if( File_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Codec );
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
            
            template<class C, class M>
            void method_File_Converter( C * _self, M _method )
            {
                if( File_Converter_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_Converter );
            }
            
            bool has_File_FrameRate() const
            {
                return File_FrameRate_successful;
            }
            
            bool get_File_FrameRate( float & _value ) const
            {
                if( File_FrameRate_successful == false )
                {
                    return false;
                }
            
                _value = this->File_FrameRate;
            
                return true;
            }
            
            bool swap_File_FrameRate( float & _value ) const
            {
                if( File_FrameRate_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->File_FrameRate);
            
                return true;
            }
            
            template<class C, class M>
            void method_File_FrameRate( C * _self, M _method )
            {
                if( File_FrameRate_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_FrameRate );
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
            
            template<class C, class M>
            void method_File_NoSeek( C * _self, M _method )
            {
                if( File_NoSeek_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->File_NoSeek );
            }
            
            const Menge::ConstString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            bool File_FrameRate_successful;
            mutable float File_FrameRate;
            bool File_NoSeek_successful;
            mutable bool File_NoSeek;
            mutable Menge::ConstString File_Path;
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
            
            template<class C, class M>
            void method_WindowBackground_ResourceImageName( C * _self, M _method )
            {
                if( WindowBackground_ResourceImageName_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->WindowBackground_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowBottom_Offset() const
            {
                return this->WindowBottom_Offset;
            }
            
            void swap_WindowBottom_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowBottom_Offset);
            }
            
            template<class C, class M>
            void method_WindowBottom_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowBottom_Offset );
            }
            
            const Menge::ConstString & get_WindowBottom_ResourceImageName() const
            {
                return this->WindowBottom_ResourceImageName;
            }
            
            void swap_WindowBottom_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowBottom_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowBottom_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowBottom_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowLeft_Offset() const
            {
                return this->WindowLeft_Offset;
            }
            
            void swap_WindowLeft_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowLeft_Offset);
            }
            
            template<class C, class M>
            void method_WindowLeft_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowLeft_Offset );
            }
            
            const Menge::ConstString & get_WindowLeft_ResourceImageName() const
            {
                return this->WindowLeft_ResourceImageName;
            }
            
            void swap_WindowLeft_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowLeft_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowLeft_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowLeft_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowLeftBottom_Offset() const
            {
                return this->WindowLeftBottom_Offset;
            }
            
            void swap_WindowLeftBottom_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowLeftBottom_Offset);
            }
            
            template<class C, class M>
            void method_WindowLeftBottom_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowLeftBottom_Offset );
            }
            
            const Menge::ConstString & get_WindowLeftBottom_ResourceImageName() const
            {
                return this->WindowLeftBottom_ResourceImageName;
            }
            
            void swap_WindowLeftBottom_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowLeftBottom_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowLeftBottom_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowLeftBottom_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowLeftTop_Offset() const
            {
                return this->WindowLeftTop_Offset;
            }
            
            void swap_WindowLeftTop_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowLeftTop_Offset);
            }
            
            template<class C, class M>
            void method_WindowLeftTop_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowLeftTop_Offset );
            }
            
            const Menge::ConstString & get_WindowLeftTop_ResourceImageName() const
            {
                return this->WindowLeftTop_ResourceImageName;
            }
            
            void swap_WindowLeftTop_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowLeftTop_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowLeftTop_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowLeftTop_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowRight_Offset() const
            {
                return this->WindowRight_Offset;
            }
            
            void swap_WindowRight_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowRight_Offset);
            }
            
            template<class C, class M>
            void method_WindowRight_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowRight_Offset );
            }
            
            const Menge::ConstString & get_WindowRight_ResourceImageName() const
            {
                return this->WindowRight_ResourceImageName;
            }
            
            void swap_WindowRight_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowRight_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowRight_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowRight_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowRightBottom_Offset() const
            {
                return this->WindowRightBottom_Offset;
            }
            
            void swap_WindowRightBottom_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowRightBottom_Offset);
            }
            
            template<class C, class M>
            void method_WindowRightBottom_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowRightBottom_Offset );
            }
            
            const Menge::ConstString & get_WindowRightBottom_ResourceImageName() const
            {
                return this->WindowRightBottom_ResourceImageName;
            }
            
            void swap_WindowRightBottom_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowRightBottom_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowRightBottom_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowRightBottom_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowRightTop_Offset() const
            {
                return this->WindowRightTop_Offset;
            }
            
            void swap_WindowRightTop_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowRightTop_Offset);
            }
            
            template<class C, class M>
            void method_WindowRightTop_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowRightTop_Offset );
            }
            
            const Menge::ConstString & get_WindowRightTop_ResourceImageName() const
            {
                return this->WindowRightTop_ResourceImageName;
            }
            
            void swap_WindowRightTop_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowRightTop_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowRightTop_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowRightTop_ResourceImageName );
            }
            
            const mt::vec2f & get_WindowTop_Offset() const
            {
                return this->WindowTop_Offset;
            }
            
            void swap_WindowTop_Offset( mt::vec2f & _value ) const
            {
                std::swap(_value, this->WindowTop_Offset);
            }
            
            template<class C, class M>
            void method_WindowTop_Offset( C * _self, M _method )
            {
                (_self->*_method)( this->WindowTop_Offset );
            }
            
            const Menge::ConstString & get_WindowTop_ResourceImageName() const
            {
                return this->WindowTop_ResourceImageName;
            }
            
            void swap_WindowTop_ResourceImageName( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->WindowTop_ResourceImageName);
            }
            
            template<class C, class M>
            void method_WindowTop_ResourceImageName( C * _self, M _method )
            {
                (_self->*_method)( this->WindowTop_ResourceImageName );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
        typedef stdex::auto_array<Meta_Include> TVectorMeta_Include;
    
        const TVectorMeta_Include & get_IncludesInclude() const
        {
            return this->includes_Meta_Include;
        }
    
    protected:
        TVectorMeta_Include includes_Meta_Include;
    public:
        typedef stdex::auto_array<Meta_Resource *> TVectorMeta_Resource;
    
        const TVectorMeta_Resource & get_IncludesResource() const
        {
            return this->includes_Meta_Resource;
        }
    
    protected:
        TVectorMeta_Resource includes_Meta_Resource;
    };
    
    class Meta_Font
        : public Metabuf::Metadata
    { 
    public:
        Meta_Font();
    
    public:
        uint32_t getId() const override;
    
    public:
        const float & get_height() const
        {
            return this->height;
        }
        
        void swap_height( float & _value ) const
        {
            std::swap( _value, this->height);
        }
        
        template<class C, class M>
        void method_height( C * _self, M _method ) const
        {
            (_self->*_method)( this->height );
        }
        
    protected:
        bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_Char
            : public Metabuf::Metadata
        { 
        public:
            Meta_Char();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::GlyphCode & get_code() const
            {
                return this->code;
            }
            
            void swap_code( Menge::GlyphCode & _value ) const
            {
                std::swap( _value, this->code);
            }
            
            template<class C, class M>
            void method_code( C * _self, M _method ) const
            {
                (_self->*_method)( this->code );
            }
            
            const mt::vec2f & get_offset() const
            {
                return this->offset;
            }
            
            void swap_offset( mt::vec2f & _value ) const
            {
                std::swap( _value, this->offset);
            }
            
            template<class C, class M>
            void method_offset( C * _self, M _method ) const
            {
                (_self->*_method)( this->offset );
            }
            
            const mt::vec4f & get_rect() const
            {
                return this->rect;
            }
            
            void swap_rect( mt::vec4f & _value ) const
            {
                std::swap( _value, this->rect);
            }
            
            template<class C, class M>
            void method_rect( C * _self, M _method ) const
            {
                (_self->*_method)( this->rect );
            }
            
            const float & get_width() const
            {
                return this->width;
            }
            
            void swap_width( float & _value ) const
            {
                std::swap( _value, this->width);
            }
            
            template<class C, class M>
            void method_width( C * _self, M _method ) const
            {
                (_self->*_method)( this->width );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Kerning
                : public Metabuf::Metadata
            { 
            public:
                Meta_Kerning();
            
            public:
                uint32_t getId() const override;
            
            public:
                const float & get_advance() const
                {
                    return this->advance;
                }
                
                void swap_advance( float & _value ) const
                {
                    std::swap( _value, this->advance);
                }
                
                template<class C, class M>
                void method_advance( C * _self, M _method ) const
                {
                    (_self->*_method)( this->advance );
                }
                
                const Menge::GlyphCode & get_id() const
                {
                    return this->id;
                }
                
                void swap_id( Menge::GlyphCode & _value ) const
                {
                    std::swap( _value, this->id);
                }
                
                template<class C, class M>
                void method_id( C * _self, M _method ) const
                {
                    (_self->*_method)( this->id );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable float advance;
                mutable Menge::GlyphCode id;
            };
            
        protected:
        protected:
            mutable Menge::GlyphCode code;
            mutable mt::vec2f offset;
            mutable mt::vec4f rect;
            mutable float width;
        public:
            typedef stdex::auto_array<Meta_Kerning> TVectorMeta_Kerning;
        
            const TVectorMeta_Kerning & get_IncludesKerning() const
            {
                return this->includes_Meta_Kerning;
            }
        
        protected:
            TVectorMeta_Kerning includes_Meta_Kerning;
        };
        
    protected:
    protected:
        mutable float height;
    public:
        typedef stdex::auto_array<Meta_Char> TVectorMeta_Char;
    
        const TVectorMeta_Char & get_IncludesChar() const
        {
            return this->includes_Meta_Char;
        }
    
    protected:
        TVectorMeta_Char includes_Meta_Char;
    };
    
    class Meta_KeyFramesPack
        : public Metabuf::Metadata
    { 
    public:
        Meta_KeyFramesPack();
    
    public:
        uint32_t getId() const override;
    
    public:
        const uint32_t & get_MaxIndex() const
        {
            return this->MaxIndex;
        }
        
        void swap_MaxIndex( uint32_t & _value ) const
        {
            std::swap( _value, this->MaxIndex);
        }
        
        template<class C, class M>
        void method_MaxIndex( C * _self, M _method ) const
        {
            (_self->*_method)( this->MaxIndex );
        }
        
    protected:
        bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_Count( C * _self, M _method ) const
            {
                if( Count_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Count );
            }
            
            const float & get_Height() const
            {
                return this->Height;
            }
            
            void swap_Height( float & _value ) const
            {
                std::swap( _value, this->Height);
            }
            
            template<class C, class M>
            void method_Height( C * _self, M _method ) const
            {
                (_self->*_method)( this->Height );
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
            
            template<class C, class M>
            void method_Immutable( C * _self, M _method ) const
            {
                if( Immutable_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Immutable );
            }
            
            const uint32_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
            const float & get_Width() const
            {
                return this->Width;
            }
            
            void swap_Width( float & _value ) const
            {
                std::swap( _value, this->Width);
            }
            
            template<class C, class M>
            void method_Width( C * _self, M _method ) const
            {
                (_self->*_method)( this->Width );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
                
                template<class C, class M>
                void method_Polygon( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Polygon );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_Count( C * _self, M _method ) const
            {
                if( Count_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Count );
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
            
            template<class C, class M>
            void method_Immutable( C * _self, M _method ) const
            {
                if( Immutable_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Immutable );
            }
            
            const uint32_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
                
                template<class C, class M>
                void method_AnchorPoint( C * _self, M _method ) const
                {
                    if( AnchorPoint_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->AnchorPoint );
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
                
                template<class C, class M>
                void method_Count( C * _self, M _method ) const
                {
                    if( Count_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Count );
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
                
                template<class C, class M>
                void method_Opacity( C * _self, M _method ) const
                {
                    if( Opacity_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Opacity );
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
                
                template<class C, class M>
                void method_Position( C * _self, M _method ) const
                {
                    if( Position_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Position );
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
                
                template<class C, class M>
                void method_Rotation( C * _self, M _method ) const
                {
                    if( Rotation_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Rotation );
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
                
                template<class C, class M>
                void method_Scale( C * _self, M _method ) const
                {
                    if( Scale_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Scale );
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
                
                template<class C, class M>
                void method_Volume( C * _self, M _method ) const
                {
                    if( Volume_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Volume );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_Count( C * _self, M _method ) const
            {
                if( Count_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Count );
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
            
            template<class C, class M>
            void method_Immutable( C * _self, M _method ) const
            {
                if( Immutable_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Immutable );
            }
            
            const uint32_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
                
                template<class C, class M>
                void method_AnchorPoint( C * _self, M _method ) const
                {
                    if( AnchorPoint_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->AnchorPoint );
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
                
                template<class C, class M>
                void method_Count( C * _self, M _method ) const
                {
                    if( Count_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Count );
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
                
                template<class C, class M>
                void method_Opacity( C * _self, M _method ) const
                {
                    if( Opacity_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Opacity );
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
                
                template<class C, class M>
                void method_Position( C * _self, M _method ) const
                {
                    if( Position_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Position );
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
                
                template<class C, class M>
                void method_Rotation( C * _self, M _method ) const
                {
                    if( Rotation_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Rotation );
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
                
                template<class C, class M>
                void method_Scale( C * _self, M _method ) const
                {
                    if( Scale_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Scale );
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
                
                template<class C, class M>
                void method_Volume( C * _self, M _method ) const
                {
                    if( Volume_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Volume );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                bool AnchorPoint_successful;
                mutable mt::vec3f AnchorPoint;
                bool Count_successful;
                mutable uint32_t Count;
                bool Opacity_successful;
                mutable float Opacity;
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
            const uint32_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
            const Menge::Polygon & get_Value() const
            {
                return this->Value;
            }
            
            void swap_Value( Menge::Polygon & _value ) const
            {
                std::swap( _value, this->Value);
            }
            
            template<class C, class M>
            void method_Value( C * _self, M _method ) const
            {
                (_self->*_method)( this->Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            const uint32_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( uint32_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
            const Menge::Floats & get_Time() const
            {
                return this->Time;
            }
            
            void swap_Time( Menge::Floats & _value ) const
            {
                std::swap( _value, this->Time);
            }
            
            template<class C, class M>
            void method_Time( C * _self, M _method ) const
            {
                (_self->*_method)( this->Time );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
        bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
    public:
        class Meta_Fonts
            : public Metabuf::Metadata
        { 
        public:
            Meta_Fonts();
        
        public:
            uint32_t getId() const override;
        
        public:
            const Menge::ConstString & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
            template<class C, class M>
            void method_Path( C * _self, M _method ) const
            {
                (_self->*_method)( this->Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Path;
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
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Resource
                : public Metabuf::Metadata
            { 
            public:
                Meta_Resource();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::ConstString & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
                template<class C, class M>
                void method_Path( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Path );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::ConstString Path;
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
            
            template<class C, class M>
            void method_Module( C * _self, M _method ) const
            {
                if( Module_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Module );
            }
            
            const Menge::ConstString & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
            template<class C, class M>
            void method_Path( C * _self, M _method ) const
            {
                (_self->*_method)( this->Path );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
        protected:
        protected:
            bool Module_successful;
            mutable Menge::ConstString Module;
            mutable Menge::ConstString Path;
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
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
        public:
            class Meta_Text
                : public Metabuf::Metadata
            { 
            public:
                Meta_Text();
            
            public:
                uint32_t getId() const override;
            
            public:
                const Menge::ConstString & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
                template<class C, class M>
                void method_Path( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Path );
                }
                
            protected:
                bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
                bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
                bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
                bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::ConstString Path;
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
        typedef stdex::auto_array<Meta_Fonts> TVectorMeta_Fonts;
    
        const TVectorMeta_Fonts & get_IncludesFonts() const
        {
            return this->includes_Meta_Fonts;
        }
    
    protected:
        TVectorMeta_Fonts includes_Meta_Fonts;
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
        bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
        bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
        bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
        bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
            
            template<class C, class M>
            void method_CharOffset( C * _self, M _method ) const
            {
                if( CharOffset_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->CharOffset );
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
            
            template<class C, class M>
            void method_Font( C * _self, M _method ) const
            {
                if( Font_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Font );
            }
            
            const Menge::ConstString & get_Key() const
            {
                return this->Key;
            }
            
            void swap_Key( Menge::ConstString & _value ) const
            {
                std::swap( _value, this->Key);
            }
            
            template<class C, class M>
            void method_Key( C * _self, M _method ) const
            {
                (_self->*_method)( this->Key );
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
            
            template<class C, class M>
            void method_LineOffset( C * _self, M _method ) const
            {
                if( LineOffset_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->LineOffset );
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
            
            template<class C, class M>
            void method_Value( C * _self, M _method ) const
            {
                if( Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Value );
            }
            
        protected:
            bool _parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id ) override;
            bool _preparationIncludes( uint32_t _includes, uint32_t _count ) override;
            bool _parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes ) override;
            bool _parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators ) override;
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
