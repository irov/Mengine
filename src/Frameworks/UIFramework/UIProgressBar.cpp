#include "UIProgressBar.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIProgressBar::UIProgressBar()
        : m_progress( 0.f )
        , m_direction( EUIPBD_LEFT_TO_RIGHT )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIProgressBar::~UIProgressBar()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIProgressBar::setFillNode( const ShapeQuadFlexPtr & _fillNode )
    {
        if( m_fillNode == _fillNode )
        {
            return;
        }

        if( m_fillNode != nullptr && m_fillNode->getParent() == this )
        {
            m_fillNode->removeFromParent();
        }

        m_fillNode = _fillNode;

        if( m_fillNode != nullptr )
        {
            this->addChild( m_fillNode );
        }

        this->updateFill_();
    }
    //////////////////////////////////////////////////////////////////////////
    const ShapeQuadFlexPtr & UIProgressBar::getFillNode() const
    {
        return m_fillNode;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIProgressBar::setProgress( float _progress )
    {
        float progress = _progress;

        if( progress < 0.f )
        {
            progress = 0.f;
        }
        else if( progress > 1.f )
        {
            progress = 1.f;
        }

        if( m_progress == progress )
        {
            return;
        }

        m_progress = progress;

        this->updateFill_();
    }
    //////////////////////////////////////////////////////////////////////////
    float UIProgressBar::getProgress() const
    {
        return m_progress;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIProgressBar::setDirection( EUIProgressBarDirection _direction )
    {
        if( m_direction == _direction )
        {
            return;
        }

        m_direction = _direction;

        this->updateFill_();
    }
    //////////////////////////////////////////////////////////////////////////
    EUIProgressBarDirection UIProgressBar::getDirection() const
    {
        return m_direction;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIProgressBar::_dispose()
    {
        m_fillNode = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIProgressBar::updateFill_()
    {
        if( m_fillNode == nullptr )
        {
            return;
        }

        mt::vec4f visibility;

        switch( m_direction )
        {
        case EUIPBD_LEFT_TO_RIGHT:
            visibility = mt::vec4f( 0.f, 0.f, m_progress, 1.f );
            break;
        case EUIPBD_RIGHT_TO_LEFT:
            visibility = mt::vec4f( 1.f - m_progress, 0.f, m_progress, 1.f );
            break;
        case EUIPBD_TOP_TO_BOTTOM:
            visibility = mt::vec4f( 0.f, 0.f, 1.f, m_progress );
            break;
        case EUIPBD_BOTTOM_TO_TOP:
            visibility = mt::vec4f( 0.f, 1.f - m_progress, 1.f, m_progress );
            break;
        default:
            visibility = mt::vec4f( 0.f, 0.f, m_progress, 1.f );
            break;
        }

        m_fillNode->setPercentVisibility( visibility );
    }
    //////////////////////////////////////////////////////////////////////////
}
