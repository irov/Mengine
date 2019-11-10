#include "JewelryMatrix.h"

#include "Jewelry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JewelryMatrix::JewelryMatrix()
        : m_columnCount( 0 )
        , m_rowCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryMatrix::~JewelryMatrix()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryMatrix::initialize( const GOAP::EventPtr & _event, uint32_t _columnCount, uint32_t _rowCount )
    {
        m_eventSlot = _event;

        m_columnCount = _columnCount;
        m_rowCount = _rowCount;

        m_slots.resize( _columnCount * _rowCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryMatrix::finalize()
    {
        m_slots.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t JewelryMatrix::getColumnCount() const
    {
        return m_columnCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t JewelryMatrix::getRowCount() const
    {
        return m_rowCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryMatrix::removeJewelry( const JewelryPtr & _jewelry )
    {
        uint32_t jewelry_column = _jewelry->getColumn();
        uint32_t jewelry_row = _jewelry->getRow();

        uint32_t index = this->getIndex_( jewelry_column, jewelry_row );

        Slot & slot = m_slots[index];
        slot.jewelry = nullptr;

        m_eventSlot->call();
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryMatrix::setJewelry( uint32_t _column, uint32_t _row, const JewelryPtr & _jewelry )
    {
        uint32_t index = this->getIndex_( _column, _row );

        Slot & slot = m_slots[index];
         
        if( slot.jewelry != nullptr )
        {
            return;
        }

        slot.jewelry = _jewelry;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryMatrix::moveJewelry( uint32_t _column, uint32_t _row, const JewelryPtr & _jewelry )
    {
        uint32_t index = this->getIndex_( _column, _row );

        Slot & slot = m_slots[index];

        slot.jewelry = _jewelry;

        if( _row > 0 )
        {
            uint32_t prev_row = _jewelry->getRow();
            uint32_t prev_index = this->getIndex_( _column, prev_row );
            Slot & prev_slot = m_slots[prev_index];

            prev_slot.jewelry = nullptr;            
        }

        _jewelry->setRow( _row );

        if( _row > 0 )
        {
            m_eventSlot->call();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const JewelryPtr & JewelryMatrix::getJewelry( uint32_t _column, uint32_t _row ) const
    {
        uint32_t index = this->getIndex_( _column, _row );

        const Slot & slot = m_slots[index];

        const JewelryPtr & jewelry = slot.jewelry;

        return jewelry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryMatrix::existJewelry( uint32_t _column, uint32_t _row ) const
    {
        uint32_t index = this->getIndex_( _column, _row );

        const Slot & slot = m_slots[index];

        if( slot.jewelry == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t JewelryMatrix::getIndex_( uint32_t _column, uint32_t _row ) const
    {
        uint32_t index = _column + m_columnCount * _row;

        return index;
    }
}