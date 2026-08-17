#include "UIPageView.h"

#include <cstdint>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIPageView::UIPageView()
        : m_pageIndex( UINT32_MAX )
        , m_loop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIPageView::~UIPageView()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIPageView::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIPageView::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIPageView::addPage( const NodePtr & _page )
    {
        if( _page == nullptr )
        {
            return;
        }

        m_pages.emplace_back( _page );
        this->addChild( _page );

        if( m_pageIndex == UINT32_MAX )
        {
            m_pageIndex = 0;
        }

        this->updatePages_();
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr UIPageView::removePage( uint32_t _index )
    {
        if( _index >= m_pages.size() )
        {
            return nullptr;
        }

        NodePtr page = m_pages[_index];

        if( page->getParent() == this )
        {
            page->removeFromParent();
        }

        m_pages.erase( m_pages.begin() + _index );

        uint32_t oldIndex = m_pageIndex;

        if( m_pages.empty() == true )
        {
            m_pageIndex = UINT32_MAX;
        }
        else if( m_pageIndex > _index )
        {
            --m_pageIndex;
        }
        else if( m_pageIndex >= m_pages.size() )
        {
            m_pageIndex = (uint32_t)m_pages.size() - 1;
        }

        this->updatePages_();

        if( oldIndex != m_pageIndex && m_changeCallback != nullptr )
        {
            m_changeCallback->onUIIndexValue( m_pageIndex );
        }

        return page;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & UIPageView::getPage( uint32_t _index ) const
    {
        return m_pages[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIPageView::getPageCount() const
    {
        return (uint32_t)m_pages.size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIPageView::setPageIndex( uint32_t _index )
    {
        if( _index >= m_pages.size() )
        {
            return false;
        }

        if( m_pageIndex == _index )
        {
            return true;
        }

        m_pageIndex = _index;

        this->updatePages_();

        if( m_changeCallback != nullptr )
        {
            m_changeCallback->onUIIndexValue( m_pageIndex );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIPageView::getPageIndex() const
    {
        return m_pageIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIPageView::nextPage()
    {
        if( m_pages.empty() == true )
        {
            return false;
        }

        uint32_t nextIndex = m_pageIndex + 1;

        if( nextIndex >= m_pages.size() )
        {
            if( m_loop == false )
            {
                return false;
            }

            nextIndex = 0;
        }

        return this->setPageIndex( nextIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIPageView::previousPage()
    {
        if( m_pages.empty() == true )
        {
            return false;
        }

        uint32_t previousIndex;

        if( m_pageIndex == 0 )
        {
            if( m_loop == false )
            {
                return false;
            }

            previousIndex = (uint32_t)m_pages.size() - 1;
        }
        else
        {
            previousIndex = m_pageIndex - 1;
        }

        return this->setPageIndex( previousIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIPageView::setChangeCallback( const UIIndexValueCallbackInterfacePtr & _callback )
    {
        m_changeCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIIndexValueCallbackInterfacePtr & UIPageView::getChangeCallback() const
    {
        return m_changeCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIPageView::_dispose()
    {
        m_changeCallback = nullptr;
        m_pages.clear();
        m_pageIndex = UINT32_MAX;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIPageView::updatePages_()
    {
        for( uint32_t index = 0; index != m_pages.size(); ++index )
        {
            const NodePtr & page = m_pages[index];

            if( index == m_pageIndex )
            {
                page->enable();
            }
            else
            {
                page->disable();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
