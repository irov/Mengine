#   pragma once

#   include <metabuf/Metabuf.hpp>

#   include "Metatype.h"

#   include <vector>
#   include <algorithm>

namespace Metacode
{
    class Meta_DataBlock
        : public Metabuf::Metadata
    { 
    public:
        Meta_DataBlock()
            : Metabuf::Metadata()
        {
        }
        ~Meta_DataBlock()
        {
            for( TVectorMeta_Resource::const_iterator
                it = includes_Meta_Resource.begin(),
                it_end = includes_Meta_Resource.end();
            it != it_end;
            ++it )
            {
                delete *it;
            }
        }
    public:
    protected:
        bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
        bool _preparationIncludes( size_t _includes, size_t _count ) override;
        bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
        bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
    public:
        class Meta_FileStore
            : public Metabuf::Metadata
        { 
        public:
            Meta_FileStore()
                : Metabuf::Metadata()
            {
            }
        public:
            const size_t & get_Count() const
            {
                return this->Count;
            }
            
            void swap_Count( size_t & _value ) const
            {
                std::swap( _value, this->Count);
            }
            
            template<class C, class M>
            void method_Count( C * _self, M _method ) const
            {
                (_self->*_method)( this->Count );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_File
                : public Metabuf::Metadata
            { 
            public:
                Meta_File()
                    : Metabuf::Metadata()
                    , ID_successful(false)
                    , Path_successful(false)
                {
                }
            public:
                bool get_ID( size_t & _value ) const
                {
                    if( ID_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->ID;
                
                    return true;
                }
                
                bool swap_ID( size_t & _value ) const
                {
                    if( ID_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->ID);
                
                    return true;
                }
                
                template<class C, class M>
                void method_ID( C * _self, M _method ) const
                {
                    if( ID_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->ID );
                }
                
                const size_t & get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( size_t & _value ) const
                {
                    std::swap( _value, this->Index);
                }
                
                template<class C, class M>
                void method_Index( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Index );
                }
                
                bool get_Path( Menge::WString & _value ) const
                {
                    if( Path_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Path;
                
                    return true;
                }
                
                bool swap_Path( Menge::WString & _value ) const
                {
                    if( Path_successful == false )
                    {
                        return false;
                    }
                
                    std::swap( _value, this->Path);
                
                    return true;
                }
                
                template<class C, class M>
                void method_Path( C * _self, M _method ) const
                {
                    if( Path_successful == false )
                    {
                        return;
                    }
                
                    (_self->*_method)( this->Path );
                }
                
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                bool ID_successful;
                mutable size_t ID;
                mutable size_t Index;
                bool Path_successful;
                mutable Menge::WString Path;
            };
            
        protected:
        protected:
            mutable size_t Count;
        public:
            typedef std::vector<Meta_File> TVectorMeta_File;
        
            const TVectorMeta_File & get_IncludesFile() const
            {
                return this->includes_Meta_File;
            }
        
        protected:
            TVectorMeta_File includes_Meta_File;
        };
        
        class Meta_Resource
            : public Metabuf::Metadata
        { 
        public:
            Meta_Resource()
                : Metabuf::Metadata()
            {
            }
            virtual ~Meta_Resource()
            {
            }
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
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
            Meta_ResourceAnimation()
                : Meta_Resource()
            {
            }
        public:
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_Sequence
                : public Metabuf::Metadata
            { 
            public:
                Meta_Sequence()
                    : Metabuf::Metadata()
                {
                }
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
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                mutable float Delay;
                mutable Menge::ConstString ResourceImageName;
            };
            
        protected:
        protected:
        public:
            typedef std::vector<Meta_Sequence> TVectorMeta_Sequence;
        
            const TVectorMeta_Sequence & get_IncludesSequence() const
            {
                return this->includes_Meta_Sequence;
            }
        
        protected:
            TVectorMeta_Sequence includes_Meta_Sequence;
        };
        
        class Meta_ResourceCursorICO
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCursorICO()
                : Meta_Resource()
            {
            }
        public:
            const Menge::WString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::WString File_Path;
        };
        
        class Meta_ResourceCursorSystem
            : public Meta_Resource
        { 
        public:
            Meta_ResourceCursorSystem()
                : Meta_Resource()
            {
            }
        public:
            const Menge::WString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::WString File_Path;
        };
        
        class Meta_ResourceEmitterContainer
            : public Meta_Resource
        { 
        public:
            Meta_ResourceEmitterContainer()
                : Meta_Resource()
            {
            }
        public:
            const Menge::WString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
            const Menge::WString & get_Folder_Path() const
            {
                return this->Folder_Path;
            }
            
            void swap_Folder_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->Folder_Path);
            }
            
            template<class C, class M>
            void method_Folder_Path( C * _self, M _method )
            {
                (_self->*_method)( this->Folder_Path );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::WString File_Path;
            mutable Menge::WString Folder_Path;
        };
        
        class Meta_ResourceFont
            : public Meta_Resource
        { 
        public:
            Meta_ResourceFont()
                : Meta_Resource()
                , Color_Value_successful(false)
                , Image_Codec_successful(false)
                , OutlineImage_Codec_successful(false)
                , OutlineImage_Path_successful(false)
            {
            }
        public:
            bool get_Color_Value( Menge::ColourValue & _value ) const
            {
                if( Color_Value_successful == false )
                {
                    return false;
                }
            
                _value = this->Color_Value;
            
                return true;
            }
            
            bool swap_Color_Value( Menge::ColourValue & _value ) const
            {
                if( Color_Value_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Color_Value);
            
                return true;
            }
            
            template<class C, class M>
            void method_Color_Value( C * _self, M _method )
            {
                if( Color_Value_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Color_Value );
            }
            
            bool get_Image_Codec( Menge::ConstString & _value ) const
            {
                if( Image_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->Image_Codec;
            
                return true;
            }
            
            bool swap_Image_Codec( Menge::ConstString & _value ) const
            {
                if( Image_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->Image_Codec);
            
                return true;
            }
            
            template<class C, class M>
            void method_Image_Codec( C * _self, M _method )
            {
                if( Image_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->Image_Codec );
            }
            
            const Menge::WString & get_Image_Path() const
            {
                return this->Image_Path;
            }
            
            void swap_Image_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->Image_Path);
            }
            
            template<class C, class M>
            void method_Image_Path( C * _self, M _method )
            {
                (_self->*_method)( this->Image_Path );
            }
            
            bool get_OutlineImage_Codec( Menge::ConstString & _value ) const
            {
                if( OutlineImage_Codec_successful == false )
                {
                    return false;
                }
            
                _value = this->OutlineImage_Codec;
            
                return true;
            }
            
            bool swap_OutlineImage_Codec( Menge::ConstString & _value ) const
            {
                if( OutlineImage_Codec_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->OutlineImage_Codec);
            
                return true;
            }
            
            template<class C, class M>
            void method_OutlineImage_Codec( C * _self, M _method )
            {
                if( OutlineImage_Codec_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->OutlineImage_Codec );
            }
            
            bool get_OutlineImage_Path( Menge::WString & _value ) const
            {
                if( OutlineImage_Path_successful == false )
                {
                    return false;
                }
            
                _value = this->OutlineImage_Path;
            
                return true;
            }
            
            bool swap_OutlineImage_Path( Menge::WString & _value ) const
            {
                if( OutlineImage_Path_successful == false )
                {
                    return false;
                }
            
                std::swap(_value, this->OutlineImage_Path);
            
                return true;
            }
            
            template<class C, class M>
            void method_OutlineImage_Path( C * _self, M _method )
            {
                if( OutlineImage_Path_successful == false )
                {
                    return;
                }
            
                (_self->*_method)( this->OutlineImage_Path );
            }
            
            const Menge::ConstString & get_ResourceGlyph_Name() const
            {
                return this->ResourceGlyph_Name;
            }
            
            void swap_ResourceGlyph_Name( Menge::ConstString & _value ) const
            {
                std::swap(_value, this->ResourceGlyph_Name);
            }
            
            template<class C, class M>
            void method_ResourceGlyph_Name( C * _self, M _method )
            {
                (_self->*_method)( this->ResourceGlyph_Name );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            bool Color_Value_successful;
            mutable Menge::ColourValue Color_Value;
            bool Image_Codec_successful;
            mutable Menge::ConstString Image_Codec;
            mutable Menge::WString Image_Path;
            bool OutlineImage_Codec_successful;
            mutable Menge::ConstString OutlineImage_Codec;
            bool OutlineImage_Path_successful;
            mutable Menge::WString OutlineImage_Path;
            mutable Menge::ConstString ResourceGlyph_Name;
        };
        
        class Meta_ResourceGlyph
            : public Meta_Resource
        { 
        public:
            Meta_ResourceGlyph()
                : Meta_Resource()
            {
            }
        public:
            const Menge::WString & get_GlyphPath_Path() const
            {
                return this->GlyphPath_Path;
            }
            
            void swap_GlyphPath_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->GlyphPath_Path);
            }
            
            template<class C, class M>
            void method_GlyphPath_Path( C * _self, M _method )
            {
                (_self->*_method)( this->GlyphPath_Path );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::WString GlyphPath_Path;
        };
        
        class Meta_ResourceImageCombineRGBAndAlpha
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageCombineRGBAndAlpha()
                : Meta_Resource()
                , File_Alpha_successful(false)
                , File_CodecAlpha_successful(false)
                , File_CodecRGB_successful(false)
                , File_MaxSize_successful(false)
                , File_Offset_successful(false)
                , File_PathAlpha_successful(false)
                , File_PathRGB_successful(false)
                , File_Size_successful(false)
                , File_UV_successful(false)
                , File_WrapX_successful(false)
                , File_WrapY_successful(false)
            {
            }
        public:
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
            
            bool get_File_PathAlpha( Menge::WString & _value ) const
            {
                if( File_PathAlpha_successful == false )
                {
                    return false;
                }
            
                _value = this->File_PathAlpha;
            
                return true;
            }
            
            bool swap_File_PathAlpha( Menge::WString & _value ) const
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
            
            bool get_File_PathRGB( Menge::WString & _value ) const
            {
                if( File_PathRGB_successful == false )
                {
                    return false;
                }
            
                _value = this->File_PathRGB;
            
                return true;
            }
            
            bool swap_File_PathRGB( Menge::WString & _value ) const
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
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
            mutable Menge::WString File_PathAlpha;
            bool File_PathRGB_successful;
            mutable Menge::WString File_PathRGB;
            bool File_Size_successful;
            mutable mt::vec2f File_Size;
            bool File_UV_successful;
            mutable mt::vec4f File_UV;
            bool File_WrapX_successful;
            mutable bool File_WrapX;
            bool File_WrapY_successful;
            mutable bool File_WrapY;
        };
        
        class Meta_ResourceImageDefault
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageDefault()
                : Meta_Resource()
                , File_Alpha_successful(false)
                , File_Codec_successful(false)
                , File_MaxSize_successful(false)
                , File_Size_successful(false)
                , File_UV_successful(false)
                , File_WrapX_successful(false)
                , File_WrapY_successful(false)
            {
            }
        public:
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
            
            const Menge::WString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_MaxSize_successful;
            mutable mt::vec2f File_MaxSize;
            mutable Menge::WString File_Path;
            bool File_Size_successful;
            mutable mt::vec2f File_Size;
            bool File_UV_successful;
            mutable mt::vec4f File_UV;
            bool File_WrapX_successful;
            mutable bool File_WrapX;
            bool File_WrapY_successful;
            mutable bool File_WrapY;
        };
        
        class Meta_ResourceImageSolid
            : public Meta_Resource
        { 
        public:
            Meta_ResourceImageSolid()
                : Meta_Resource()
            {
            }
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ColourValue Color_Value;
            mutable mt::vec2f Size_Value;
        };
        
        class Meta_ResourceInternalObject
            : public Meta_Resource
        { 
        public:
            Meta_ResourceInternalObject()
                : Meta_Resource()
            {
            }
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::ConstString Internal_Group;
            mutable Menge::ConstString Internal_Name;
        };
        
        class Meta_ResourceMovie
            : public Meta_Resource
        { 
        public:
            Meta_ResourceMovie()
                : Meta_Resource()
                , Duration_Value_successful(false)
                , FrameDuration_Value_successful(false)
                , Height_Value_successful(false)
                , KeyFramesPackPath_Path_successful(false)
                , Width_Value_successful(false)
            {
            }
        public:
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
            
            bool get_KeyFramesPackPath_Path( Menge::WString & _value ) const
            {
                if( KeyFramesPackPath_Path_successful == false )
                {
                    return false;
                }
            
                _value = this->KeyFramesPackPath_Path;
            
                return true;
            }
            
            bool swap_KeyFramesPackPath_Path( Menge::WString & _value ) const
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_MovieCamera3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_MovieCamera3D()
                    : Metabuf::Metadata()
                {
                }
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
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
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
                Meta_MovieLayer2D()
                    : Metabuf::Metadata()
                    , PlayCount_successful(false)
                {
                }
            public:
                const Menge::ConstString & get_BlendingMode() const
                {
                    return this->BlendingMode;
                }
                
                void swap_BlendingMode( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->BlendingMode);
                }
                
                template<class C, class M>
                void method_BlendingMode( C * _self, M _method ) const
                {
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
                
                const size_t & get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( size_t & _value ) const
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
                
                const size_t & get_Parent() const
                {
                    return this->Parent;
                }
                
                void swap_Parent( size_t & _value ) const
                {
                    std::swap( _value, this->Parent);
                }
                
                template<class C, class M>
                void method_Parent( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Parent );
                }
                
                bool get_PlayCount( size_t & _value ) const
                {
                    if( PlayCount_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->PlayCount;
                
                    return true;
                }
                
                bool swap_PlayCount( size_t & _value ) const
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
                
                const float & get_StartInterval() const
                {
                    return this->StartInterval;
                }
                
                void swap_StartInterval( float & _value ) const
                {
                    std::swap( _value, this->StartInterval);
                }
                
                template<class C, class M>
                void method_StartInterval( C * _self, M _method ) const
                {
                    (_self->*_method)( this->StartInterval );
                }
                
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::ConstString BlendingMode;
                mutable float In;
                mutable size_t Index;
                mutable Menge::ConstString Name;
                mutable float Out;
                mutable size_t Parent;
                bool PlayCount_successful;
                mutable size_t PlayCount;
                mutable Menge::ConstString Source;
                mutable float StartInterval;
            };
            
            class Meta_MovieLayer3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_MovieLayer3D()
                    : Metabuf::Metadata()
                {
                }
            public:
                const Menge::ConstString & get_BlendingMode() const
                {
                    return this->BlendingMode;
                }
                
                void swap_BlendingMode( Menge::ConstString & _value ) const
                {
                    std::swap( _value, this->BlendingMode);
                }
                
                template<class C, class M>
                void method_BlendingMode( C * _self, M _method ) const
                {
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
                
                const size_t & get_Index() const
                {
                    return this->Index;
                }
                
                void swap_Index( size_t & _value ) const
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
                
                const size_t & get_Parent() const
                {
                    return this->Parent;
                }
                
                void swap_Parent( size_t & _value ) const
                {
                    std::swap( _value, this->Parent);
                }
                
                template<class C, class M>
                void method_Parent( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Parent );
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
                
                const float & get_StartInterval() const
                {
                    return this->StartInterval;
                }
                
                void swap_StartInterval( float & _value ) const
                {
                    std::swap( _value, this->StartInterval);
                }
                
                template<class C, class M>
                void method_StartInterval( C * _self, M _method ) const
                {
                    (_self->*_method)( this->StartInterval );
                }
                
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::ConstString BlendingMode;
                mutable float In;
                mutable size_t Index;
                mutable Menge::ConstString Name;
                mutable float Out;
                mutable size_t Parent;
                mutable Menge::ConstString Source;
                mutable float StartInterval;
            };
            
        protected:
        protected:
            bool Duration_Value_successful;
            mutable float Duration_Value;
            bool FrameDuration_Value_successful;
            mutable float FrameDuration_Value;
            bool Height_Value_successful;
            mutable float Height_Value;
            bool KeyFramesPackPath_Path_successful;
            mutable Menge::WString KeyFramesPackPath_Path;
            bool Width_Value_successful;
            mutable float Width_Value;
        public:
            typedef std::vector<Meta_MovieCamera3D> TVectorMeta_MovieCamera3D;
        
            const TVectorMeta_MovieCamera3D & get_IncludesMovieCamera3D() const
            {
                return this->includes_Meta_MovieCamera3D;
            }
        
        protected:
            TVectorMeta_MovieCamera3D includes_Meta_MovieCamera3D;
        public:
            typedef std::vector<Meta_MovieLayer2D> TVectorMeta_MovieLayer2D;
        
            const TVectorMeta_MovieLayer2D & get_IncludesMovieLayer2D() const
            {
                return this->includes_Meta_MovieLayer2D;
            }
        
        protected:
            TVectorMeta_MovieLayer2D includes_Meta_MovieLayer2D;
        public:
            typedef std::vector<Meta_MovieLayer3D> TVectorMeta_MovieLayer3D;
        
            const TVectorMeta_MovieLayer3D & get_IncludesMovieLayer3D() const
            {
                return this->includes_Meta_MovieLayer3D;
            }
        
        protected:
            TVectorMeta_MovieLayer3D includes_Meta_MovieLayer3D;
        };
        
        class Meta_ResourcePlaylist
            : public Meta_Resource
        { 
        public:
            Meta_ResourcePlaylist()
                : Meta_Resource()
                , Loop_Value_successful(false)
                , Shuffle_Value_successful(false)
            {
            }
        public:
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_Tracks
                : public Metabuf::Metadata
            { 
            public:
                Meta_Tracks()
                    : Metabuf::Metadata()
                {
                }
            public:
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
                class Meta_Track
                    : public Metabuf::Metadata
                { 
                public:
                    Meta_Track()
                        : Metabuf::Metadata()
                        , Codec_successful(false)
                    {
                    }
                public:
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
                    
                    const Menge::WString & get_File() const
                    {
                        return this->File;
                    }
                    
                    void swap_File( Menge::WString & _value ) const
                    {
                        std::swap( _value, this->File);
                    }
                    
                    template<class C, class M>
                    void method_File( C * _self, M _method ) const
                    {
                        (_self->*_method)( this->File );
                    }
                    
                protected:
                    bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                    bool _preparationIncludes( size_t _includes, size_t _count ) override;
                    bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                    bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
                public:
                protected:
                protected:
                    bool Codec_successful;
                    mutable Menge::ConstString Codec;
                    mutable Menge::WString File;
                };
                
            protected:
            protected:
            public:
                typedef std::vector<Meta_Track> TVectorMeta_Track;
            
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
            typedef std::vector<Meta_Tracks> TVectorMeta_Tracks;
        
            const TVectorMeta_Tracks & get_IncludesTracks() const
            {
                return this->includes_Meta_Tracks;
            }
        
        protected:
            TVectorMeta_Tracks includes_Meta_Tracks;
        };
        
        class Meta_ResourceSound
            : public Meta_Resource
        { 
        public:
            Meta_ResourceSound()
                : Meta_Resource()
                , DefaultVolume_Value_successful(false)
                , File_Codec_successful(false)
                , File_Converter_successful(false)
                , IsStreamable_Value_successful(false)
            {
            }
        public:
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
            
            const Menge::WString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            bool DefaultVolume_Value_successful;
            mutable float DefaultVolume_Value;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            bool File_Converter_successful;
            mutable Menge::ConstString File_Converter;
            mutable Menge::WString File_Path;
            bool IsStreamable_Value_successful;
            mutable bool IsStreamable_Value;
        };
        
        class Meta_ResourceVideo
            : public Meta_Resource
        { 
        public:
            Meta_ResourceVideo()
                : Meta_Resource()
                , File_Alpha_successful(false)
                , File_Codec_successful(false)
            {
            }
        public:
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
            
            const Menge::WString & get_File_Path() const
            {
                return this->File_Path;
            }
            
            void swap_File_Path( Menge::WString & _value ) const
            {
                std::swap(_value, this->File_Path);
            }
            
            template<class C, class M>
            void method_File_Path( C * _self, M _method )
            {
                (_self->*_method)( this->File_Path );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            bool File_Alpha_successful;
            mutable bool File_Alpha;
            bool File_Codec_successful;
            mutable Menge::ConstString File_Codec;
            mutable Menge::WString File_Path;
        };
        
        class Meta_ResourceWindow
            : public Meta_Resource
        { 
        public:
            Meta_ResourceWindow()
                : Meta_Resource()
                , WindowBackground_ResourceImageName_successful(false)
            {
            }
        public:
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
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
    public:
        typedef std::vector<Meta_FileStore> TVectorMeta_FileStore;
    
        const TVectorMeta_FileStore & get_IncludesFileStore() const
        {
            return this->includes_Meta_FileStore;
        }
    
    protected:
        TVectorMeta_FileStore includes_Meta_FileStore;
    public:
        typedef std::vector<Meta_Resource *> TVectorMeta_Resource;
    
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
        Meta_Font()
            : Metabuf::Metadata()
        {
        }
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
        bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
        bool _preparationIncludes( size_t _includes, size_t _count ) override;
        bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
        bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
    public:
        class Meta_Char
            : public Metabuf::Metadata
        { 
        public:
            Meta_Char()
                : Metabuf::Metadata()
            {
            }
        public:
            const wchar_t & get_code() const
            {
                return this->code;
            }
            
            void swap_code( wchar_t & _value ) const
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_Kerning
                : public Metabuf::Metadata
            { 
            public:
                Meta_Kerning()
                    : Metabuf::Metadata()
                {
                }
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
                
                const wchar_t & get_id() const
                {
                    return this->id;
                }
                
                void swap_id( wchar_t & _value ) const
                {
                    std::swap( _value, this->id);
                }
                
                template<class C, class M>
                void method_id( C * _self, M _method ) const
                {
                    (_self->*_method)( this->id );
                }
                
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                mutable float advance;
                mutable wchar_t id;
            };
            
        protected:
        protected:
            mutable wchar_t code;
            mutable mt::vec2f offset;
            mutable mt::vec4f rect;
            mutable float width;
        public:
            typedef std::vector<Meta_Kerning> TVectorMeta_Kerning;
        
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
        typedef std::vector<Meta_Char> TVectorMeta_Char;
    
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
        Meta_KeyFramesPack()
            : Metabuf::Metadata()
        {
        }
    public:
        const size_t & get_MaxIndex() const
        {
            return this->MaxIndex;
        }
        
        void swap_MaxIndex( size_t & _value ) const
        {
            std::swap( _value, this->MaxIndex);
        }
        
        template<class C, class M>
        void method_MaxIndex( C * _self, M _method ) const
        {
            (_self->*_method)( this->MaxIndex );
        }
        
    protected:
        bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
        bool _preparationIncludes( size_t _includes, size_t _count ) override;
        bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
        bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
    public:
        class Meta_KeyFrames2D
            : public Metabuf::Metadata
        { 
        public:
            Meta_KeyFrames2D()
                : Metabuf::Metadata()
                , Count_successful(false)
                , Immutable_successful(false)
            {
            }
        public:
            bool get_Count( size_t & _value ) const
            {
                if( Count_successful == false )
                {
                    return false;
                }
            
                _value = this->Count;
            
                return true;
            }
            
            bool swap_Count( size_t & _value ) const
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
            
            const size_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( size_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_KeyFrame2D
                : public Metabuf::Metadata
            { 
            public:
                Meta_KeyFrame2D()
                    : Metabuf::Metadata()
                    , AnchorPoint_successful(false)
                    , Count_successful(false)
                    , Opacity_successful(false)
                    , Position_successful(false)
                    , Rotation_successful(false)
                    , Scale_successful(false)
                    , Volume_successful(false)
                {
                }
            public:
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
                
                bool get_Count( size_t & _value ) const
                {
                    if( Count_successful == false )
                    {
                        return false;
                    }
                
                    _value = this->Count;
                
                    return true;
                }
                
                bool swap_Count( size_t & _value ) const
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
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                bool AnchorPoint_successful;
                mutable mt::vec2f AnchorPoint;
                bool Count_successful;
                mutable size_t Count;
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
            mutable size_t Count;
            bool Immutable_successful;
            mutable bool Immutable;
            mutable size_t LayerIndex;
        public:
            typedef std::vector<Meta_KeyFrame2D> TVectorMeta_KeyFrame2D;
        
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
            Meta_KeyFrames3D()
                : Metabuf::Metadata()
            {
            }
        public:
            const size_t & get_LayerIndex() const
            {
                return this->LayerIndex;
            }
            
            void swap_LayerIndex( size_t & _value ) const
            {
                std::swap( _value, this->LayerIndex);
            }
            
            template<class C, class M>
            void method_LayerIndex( C * _self, M _method ) const
            {
                (_self->*_method)( this->LayerIndex );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_KeyFrame3D
                : public Metabuf::Metadata
            { 
            public:
                Meta_KeyFrame3D()
                    : Metabuf::Metadata()
                    , AnchorPoint_successful(false)
                    , Opacity_successful(false)
                    , Position_successful(false)
                    , Rotation_successful(false)
                    , Scale_successful(false)
                    , Volume_successful(false)
                {
                }
            public:
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
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                bool AnchorPoint_successful;
                mutable mt::vec3f AnchorPoint;
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
            mutable size_t LayerIndex;
        public:
            typedef std::vector<Meta_KeyFrame3D> TVectorMeta_KeyFrame3D;
        
            const TVectorMeta_KeyFrame3D & get_IncludesKeyFrame3D() const
            {
                return this->includes_Meta_KeyFrame3D;
            }
        
        protected:
            TVectorMeta_KeyFrame3D includes_Meta_KeyFrame3D;
        };
        
    protected:
    protected:
        mutable size_t MaxIndex;
    public:
        typedef std::vector<Meta_KeyFrames2D> TVectorMeta_KeyFrames2D;
    
        const TVectorMeta_KeyFrames2D & get_IncludesKeyFrames2D() const
        {
            return this->includes_Meta_KeyFrames2D;
        }
    
    protected:
        TVectorMeta_KeyFrames2D includes_Meta_KeyFrames2D;
    public:
        typedef std::vector<Meta_KeyFrames3D> TVectorMeta_KeyFrames3D;
    
        const TVectorMeta_KeyFrames3D & get_IncludesKeyFrames3D() const
        {
            return this->includes_Meta_KeyFrames3D;
        }
    
    protected:
        TVectorMeta_KeyFrames3D includes_Meta_KeyFrames3D;
    };
    
    class Meta_Pak
        : public Metabuf::Metadata
    { 
    public:
        Meta_Pak()
            : Metabuf::Metadata()
        {
        }
    public:
    protected:
        bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
        bool _preparationIncludes( size_t _includes, size_t _count ) override;
        bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
        bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
    public:
        class Meta_Resources
            : public Metabuf::Metadata
        { 
        public:
            Meta_Resources()
                : Metabuf::Metadata()
            {
            }
        public:
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_Resource
                : public Metabuf::Metadata
            { 
            public:
                Meta_Resource()
                    : Metabuf::Metadata()
                {
                }
            public:
                const Menge::WString & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::WString & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
                template<class C, class M>
                void method_Path( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Path );
                }
                
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::WString Path;
            };
            
        protected:
        protected:
        public:
            typedef std::vector<Meta_Resource> TVectorMeta_Resource;
        
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
            Meta_Scripts()
                : Metabuf::Metadata()
            {
            }
        public:
            const Menge::WString & get_Path() const
            {
                return this->Path;
            }
            
            void swap_Path( Menge::WString & _value ) const
            {
                std::swap( _value, this->Path);
            }
            
            template<class C, class M>
            void method_Path( C * _self, M _method ) const
            {
                (_self->*_method)( this->Path );
            }
            
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
        protected:
        protected:
            mutable Menge::WString Path;
        };
        
        class Meta_Texts
            : public Metabuf::Metadata
        { 
        public:
            Meta_Texts()
                : Metabuf::Metadata()
            {
            }
        public:
        protected:
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
        public:
            class Meta_Text
                : public Metabuf::Metadata
            { 
            public:
                Meta_Text()
                    : Metabuf::Metadata()
                {
                }
            public:
                const Menge::WString & get_Path() const
                {
                    return this->Path;
                }
                
                void swap_Path( Menge::WString & _value ) const
                {
                    std::swap( _value, this->Path);
                }
                
                template<class C, class M>
                void method_Path( C * _self, M _method ) const
                {
                    (_self->*_method)( this->Path );
                }
                
            protected:
                bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
                bool _preparationIncludes( size_t _includes, size_t _count ) override;
                bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
                bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
            public:
            protected:
            protected:
                mutable Menge::WString Path;
            };
            
        protected:
        protected:
        public:
            typedef std::vector<Meta_Text> TVectorMeta_Text;
        
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
        typedef std::vector<Meta_Resources> TVectorMeta_Resources;
    
        const TVectorMeta_Resources & get_IncludesResources() const
        {
            return this->includes_Meta_Resources;
        }
    
    protected:
        TVectorMeta_Resources includes_Meta_Resources;
    public:
        typedef std::vector<Meta_Scripts> TVectorMeta_Scripts;
    
        const TVectorMeta_Scripts & get_IncludesScripts() const
        {
            return this->includes_Meta_Scripts;
        }
    
    protected:
        TVectorMeta_Scripts includes_Meta_Scripts;
    public:
        typedef std::vector<Meta_Texts> TVectorMeta_Texts;
    
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
        Meta_Texts()
            : Metabuf::Metadata()
        {
        }
    public:
    protected:
        bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
        bool _preparationIncludes( size_t _includes, size_t _count ) override;
        bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
        bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
    public:
        class Meta_Text
            : public Metabuf::Metadata
        { 
        public:
            Meta_Text()
                : Metabuf::Metadata()
                , CharOffset_successful(false)
                , Font_successful(false)
                , LineOffset_successful(false)
                , Value_successful(false)
            {
            }
        public:
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
            bool _parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id ) override;
            bool _preparationIncludes( size_t _includes, size_t _count ) override;
            bool _parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes ) override;
            bool _parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators ) override;
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
        typedef std::vector<Meta_Text> TVectorMeta_Text;
    
        const TVectorMeta_Text & get_IncludesText() const
        {
            return this->includes_Meta_Text;
        }
    
    protected:
        TVectorMeta_Text includes_Meta_Text;
    };
    
}
