#pragma once

#include "Interface/RenderPipelineInterface.h"

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
        void drawTextDebug( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::vec2f & _pos, const ConstString & _fontName, const Color & _color, const Char * _format, ... );
    }
}
