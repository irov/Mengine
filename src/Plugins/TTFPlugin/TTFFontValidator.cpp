#include "TTFFontValidator.h"

#include "Interface/VocabularyServiceInterface.h"

#include "TTFFont.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontValidator::validate( const FontInterfacePtr & _font ) const
    {
        TTFFontPtr ttfFont = TTFFontPtr::from( _font );

        const FontEffectInterfacePtr & effect = ttfFont->getEffect();

        if( effect != nullptr )
        {
            if( effect->isValid() == false )
            {
                return false;
            }
        }

        DataflowInterfacePtr dataflowTTF = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        if( dataflowTTF == nullptr )
        {
            return false;
        }

        const ContentInterfacePtr & content = ttfFont->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        TTFDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflowTTF, &context, MENGINE_DOCUMENT_FACTORABLE );

        if( data == nullptr )
        {
            return false;
        }

        FT_Face face = data->getFTFace();

        if( face == nullptr )
        {
            return false;
        }

        if( FT_IS_SCALABLE( face ) == false )
        {
            LOGGER_ERROR( "ttf font '%s' should be scalable"
                , ttfFont->getName().c_str()
            );

            return false;
        }

        uint32_t height = ttfFont->getHeight();

        FT_Error err_pixel_sizes = FT_Set_Pixel_Sizes( face, 0, height );

        if( err_pixel_sizes != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_pixel_sizes );

            LOGGER_ERROR( "ttf font '%s' invalid set pixel sizes '%u' error [%s]"
                , ttfFont->getName().c_str()
                , height
                , err_message
            );

            return false;
        }

        GlyphCode code = ttfFont->getMetricSymbol();

        FT_UInt glyph_index = FT_Get_Char_Index( face, code );

        FT_Error err_load_glyph = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR );

        if( err_load_glyph != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_load_glyph );

            LOGGER_ERROR( "ttf font '%s' invalid load glyph code '%u' error [%s]"
                , ttfFont->getName().c_str()
                , code
                , err_message
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}