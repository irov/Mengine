#pragma once

#include "Interface/RenderPipelineInterface.h"
#include "Interface/FontInterface.h"

#include "Kernel/RenderContext.h"
#include "Kernel/ConstString.h"
#include "Kernel/Color.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

#include "math/vec2.h"

namespace Mengine
{
    namespace Helper
    {
        void drawTextDebug( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::vec2f & _pos, const FontInterfacePtr & _font, const Color & _color, const DocumentInterfacePtr & _doc, MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 7, 8 );
    }
}
