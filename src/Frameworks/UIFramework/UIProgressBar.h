#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ShapeQuadFlex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EUIProgressBarDirection
    {
        EUIPBD_LEFT_TO_RIGHT,
        EUIPBD_RIGHT_TO_LEFT,
        EUIPBD_TOP_TO_BOTTOM,
        EUIPBD_BOTTOM_TO_TOP
    };
    //////////////////////////////////////////////////////////////////////////
    class UIProgressBar
        : public Node
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( UIProgressBar );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        UIProgressBar();
        ~UIProgressBar() override;

    public:
        void setFillNode( const ShapeQuadFlexPtr & _fillNode );
        const ShapeQuadFlexPtr & getFillNode() const;

        void setProgress( float _progress );
        float getProgress() const;

        void setDirection( EUIProgressBarDirection _direction );
        EUIProgressBarDirection getDirection() const;

    protected:
        void _dispose() override;

    private:
        void updateFill_();

    private:
        ShapeQuadFlexPtr m_fillNode;
        float m_progress;
        EUIProgressBarDirection m_direction;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIProgressBar> UIProgressBarPtr;
    //////////////////////////////////////////////////////////////////////////
}
