#include "TTFFontGlyphValidator.h"

#include "Interface/VocabularyServiceInterface.h"

#include "TTFFont.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontGlyphValidator::_validate( const TTFFontGlyphPtr & _glyph ) const
    {
        DataflowInterfacePtr dataflowTTF = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        if( dataflowTTF == nullptr )
        {
            return false;
        }

        const ContentInterfacePtr & content = _glyph->getGlyphContent();

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
            LOGGER_ERROR( "ttf font glyph '%s' should be scalable"
                , _glyph->getName().c_str()
            );

            return false;
        }

        FT_UInt glyph_index = FT_Get_Char_Index( face, MENGINE_TTF_METRICS_SYMBOL );

        FT_Error err_load_glyph = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR );

        if( err_load_glyph != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_load_glyph );

            LOGGER_ERROR( "ttf font glyph '%s' invalid load glyph code '%u' error [%s]"
                , _glyph->getName().c_str()
                , MENGINE_TTF_METRICS_SYMBOL
                , err_message
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}