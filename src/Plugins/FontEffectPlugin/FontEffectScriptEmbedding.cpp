#include "FontEffectScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#include "FontEffectPluginInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class FontEffectScriptMethod
            : public Factorable
        {
        public:
            FontEffectScriptMethod()
            {
            }

            ~FontEffectScriptMethod() override
            {
            }

        protected:
            FileGroupInterfacePtr getDefaultFileGroup_() const
            {
                const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

                return fileGroup;
            }

        public:
            FontEffectInterfacePtr s_createFontEffectFromFile( const FilePath & _filePath, const ConstString & _effectName, uint32_t _sample )
            {
                FileGroupInterfacePtr fileGroup = this->getDefaultFileGroup_();

                ContentInterfacePtr content = Helper::makeFileContent( fileGroup, _filePath, MENGINE_DOCUMENT_PYTHON );

                if( content == nullptr )
                {
                    return nullptr;
                }

                FontEffectInterfacePtr fontEffect = FONTEFFECT_SERVICE()
                    ->createFontEffectFromFile( content, _effectName, _sample, MENGINE_DOCUMENT_PYTHON );

                return fontEffect;
            }

            bool s_applyFontEffect( const ConstString & _fontName, const FontEffectInterfacePtr & _fontEffect )
            {
                FontInterfacePtr font;
                if( FONT_SERVICE()
                    ->existFont( _fontName, &font ) == false )
                {
                    LOGGER_ERROR( "font '%s' not found"
                        , _fontName.c_str()
                    );

                    return false;
                }

                if( font->isCompileFont() == true )
                {
                    LOGGER_ERROR( "font '%s' is compiled, release it before apply effect"
                        , _fontName.c_str()
                    );

                    return false;
                }

                font->setEffect( _fontEffect );

                return true;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<FontEffectScriptMethod> FontEffectScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectScriptEmbedding::FontEffectScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectScriptEmbedding::~FontEffectScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        Detail::FontEffectScriptMethodPtr scriptMethod = Helper::makeFactorableUnique<Detail::FontEffectScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        pybind::def_functor( _kernel, "createFontEffectFromFile", scriptMethod, &Detail::FontEffectScriptMethod::s_createFontEffectFromFile );
        pybind::def_functor( _kernel, "applyFontEffect", scriptMethod, &Detail::FontEffectScriptMethod::s_applyFontEffect );

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        m_implement = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
