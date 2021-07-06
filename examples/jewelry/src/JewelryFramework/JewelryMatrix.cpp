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
    bool JewelryMatrix::initialize( const GOAP::EventInterfacePtr & _event, uint32_t _columnCount, uint32_t _rowCount )
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
    bool JewelryMatrix::isFall( const JewelryPtr & _jewelry ) const
    {
        uint32_t jewelry_column = _jewelry->getColumn();
        uint32_t jewelry_row = _jewelry->getRow();

        if( jewelry_row + 1 == m_rowCount )
        {
            return false;
        }

        const JewelryPtr & bottom_jewelry = this->getJewelry( jewelry_column, jewelry_row + 1 );

        if( bottom_jewelry == nullptr )
        {
            return true;
        }

        if( bottom_jewelry->isMove() == true )
        {
            return true;
        }

        bool result = this->isFall( bottom_jewelry );
        
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryMatrix::getNeighbours( uint32_t _column, uint32_t _row, uint32_t _type, VectorJewelries & _jewelries, const LambdaJewelry & _lambda ) const
    {
        const JewelryPtr & jewelry = this->getJewelry( _column, _row );

        if( jewelry == nullptr )
        {
            return;
        }

        if( jewelry->isDead() == true )
        {
            return;
        }
        
        uint32_t jewelry_type = jewelry->getType();

        if( jewelry_type != _type )
        {
            return;
        }

        if( std::find( _jewelries.begin(), _jewelries.end(), jewelry ) != _jewelries.end() )
        {
            return;
        }

        if( _lambda( jewelry ) == false )
        {
            return;
        }

        _jewelries.emplace_back( jewelry );

        if( _column > 0 )
        {
            this->getNeighbours( _column - 1, _row, _type, _jewelries, _lambda );
        }

        if( _column + 1 < m_columnCount )
        {
            this->getNeighbours( _column + 1, _row, _type, _jewelries, _lambda );
        }

        if( _row > 0 )
        {
            this->getNeighbours( _column, _row - 1, _type, _jewelries, _lambda );
        }

        if( _row + 1 < m_rowCount )
        {
            this->getNeighbours( _column, _row + 1, _type, _jewelries, _lambda );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t JewelryMatrix::getIndex_( uint32_t _column, uint32_t _row ) const
    {
        uint32_t index = _column + m_columnCount * _row;

        return index;
    }
}