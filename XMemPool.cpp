/*
 * XMemPool.cpp
 *
 *  Created on: Mar 17, 2011
 *      Author: pozen
 */
#include "XMemPool.h"

XMemPool::XMemPool( const size_t& maxBlockSize, const size_t& lmaxSubPoolCapability, const size_t& lpoolXep ):\
_maxBlockSize( maxBlockSize ),
_lmaxSubPoolCapability( lmaxSubPoolCapability ),
_lpoolXep( lpoolXep )
{
    _ssubPoolNum = ( S_MEM_THRE + _spoolXep - 1 ) / _spoolXep;
    _lsubPoolNum = ( _maxBlockSize + _lpoolXep - 1 ) / _lpoolXep;
    _totalMemSize = 0;
    _ssubPoolSize = new size_t[_ssubPoolNum];
    _lsubPoolSize = new size_t[_lsubPoolNum];
    _ssubPool[0] = new XMemBlock*[_ssubPoolNum];
    _ssubPool[1] = new XMemBlock*[_ssubPoolNum];
    for( int i = 0; i < _ssubPoolNum; i++ )
    {
        _ssubPool[0][i] = 0;
        _ssubPool[1][i] = 0;
        _ssubPoolSize[i] = 0;
    }
    _lsubPool[0] = new XMemBlock*[_lsubPoolNum];
    _lsubPool[1] = new XMemBlock*[_lsubPoolNum];
    for( int i = 0; i < _lsubPoolNum; i++ )
    {
        _lsubPool[0][i] = 0;
        _lsubPool[1][i] = 0;
        _lsubPoolSize[i] = 0;
    }
}

XMemBlock* XMemPool::alloc( size_t size ) //byte unit
{
    if( size <= S_MEM_THRE )
    {
        size_t idx = ( size + _spoolXep - 1 ) / _spoolXep - 1;
        XMemBlock *block = 0;
        if( _ssubPool[0][idx] )
        {
            block = _ssubPool[0][idx];
            _ssubPool[0][idx] = block->_next;
            if( _ssubPool[1][idx] )
                _ssubPool[1][idx]->_pre = block;
            block->_next = _ssubPool[1][idx];
            _ssubPool[1][idx] = block;
            _ssubPool[1][idx]->_pre = 0;
            return block;
        }
        else if( _ssubPoolSize[idx] < _smaxSubPoolCapability )
        {
            size_t msize = (idx + 1)*_spoolXep;
            _totalMemSize += msize;
            block = new XMemBlock( idx, msize );
            _ssubPoolSize[idx]++;
            if( _ssubPool[1][idx] )
                _ssubPool[1][idx]->_pre = block;
            block->_next = _ssubPool[1][idx];
            _ssubPool[1][idx] = block;
            return block;
        }
    }
    else if( size <= _maxBlockSize )
    {
        size_t idx = ( size + _lpoolXep - 1 ) / _lpoolXep - 1;
        XMemBlock *block = 0;
        if( _lsubPool[0][idx] )
        {
            block = _lsubPool[0][idx];
            _lsubPool[0][idx] = block->_next;
            if( _lsubPool[1][idx] )
                _lsubPool[1][idx]->_pre = block;
            block->_next = _lsubPool[1][idx];
            _lsubPool[1][idx] = block;
            _lsubPool[1][idx]->_pre = 0;
            return block;
        }
        else if( _lsubPoolSize[idx] < _lmaxSubPoolCapability )
        {
            size_t msize = (idx + 1)*_lpoolXep;
            _totalMemSize += msize;
            block = new XMemBlock( idx, msize );
            _lsubPoolSize[idx]++;
            if( _lsubPool[1][idx] )
                _lsubPool[1][idx]->_pre = block;
            block->_next = _lsubPool[1][idx];
            _lsubPool[1][idx] = block;
            return block;
        }
    }

     return (new XMemBlock( -1, size ));
}

void XMemPool::free( XMemBlock *block )
{
    if( block->_index < 0 )
    {
        delete block;
        return;
    }
    if( block->_size <= S_MEM_THRE )
    {
        if( block->_next )
            block->_next->_pre = block->_pre;
        if( block->_pre )
            block->_pre->_next = block->_next;
        else if( !block->_next && !block->_pre )
            _ssubPool[1][block->_index] = 0;
        block->_next = _ssubPool[0][block->_index];
        if( _ssubPool[0][block->_index] )
            _ssubPool[0][block->_index]->_pre = block;
        _ssubPool[0][block->_index] = block;
    }
    else
    {
        if( block->_next )
            block->_next->_pre = block->_pre;
        if( block->_pre )
            block->_pre->_next = block->_next;
        else if( !block->_next && !block->_pre )
            _lsubPool[1][block->_index] = 0;
        block->_next = _lsubPool[0][block->_index];
        if( _lsubPool[0][block->_index] )
            _lsubPool[0][block->_index]->_pre = block;
        _lsubPool[0][block->_index] = block;
    }
}

void XMemPool::destroy()
{
    for( int i = 0; i < _ssubPoolNum; i++ )
    {
        XMemBlock *block = _ssubPool[0][i], *tmp;
        while( block )
        {
            tmp = block->_next;
            delete block;
            block = tmp;
        }
        _ssubPool[0][i] = 0;

        block = _ssubPool[1][i];
        while( block )
        {
            tmp = block->_next;
            delete block;
            block = tmp;
        }
        _ssubPool[1][i] = 0;
        _ssubPoolSize[i] = 0;
    }
    for( int i = 0; i < _lsubPoolNum; i++ )
    {
        XMemBlock *block = _lsubPool[0][i], *tmp;
        while( block )
        {
            tmp = block->_next;
            delete block;
            block = tmp;
        }
        _lsubPool[0][i] = 0;

        block = _lsubPool[1][i];
        while( block )
        {
            tmp = block->_next;
            delete block;
            block = tmp;
        }
        _lsubPool[1][i] = 0;
        _lsubPoolSize[i] = 0;
    }
}

XMemPool::~XMemPool()
{
    destroy();
    delete []_ssubPoolSize;
    delete []_lsubPoolSize;
    delete []_ssubPool[0];
    delete []_ssubPool[1];
    delete []_lsubPool[0];
    delete []_lsubPool[1];
}


