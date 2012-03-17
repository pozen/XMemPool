/*
 * XMemPool.h
 *
 *  Created on: Mar 17, 2011
 *      Author: pozen
 */
 #ifndef _X_MEM_POOL_H_
 #define _X_MEM_POOL_H_

 //! (1)alloc：O(1)
 //! (2)free ：O(1)
 //! (3)not thread safe
 //! (4)In order to achieve good performance, you'd better construct pool width appropriate params under your cache usage

#ifndef size_t
#define size_t unsigned int
#endif

class XMemPool;
class XMemBlock
{
public:
    XMemBlock( const int& index, const size_t& size ):
      _index( index ),
      _next( 0 ),
      _pre( 0 ),
      _size( size )
    {
        _data = new char[size];
    }
    ~XMemBlock()
    {
        delete []_data;
    }

public:
    friend class XMemPool;
    template<class T>
    T* getMem() const { return (T*)_data; }

private:
    const int    _index;
    const size_t _size;
    char        *_data;
    XMemBlock  *_next;
    XMemBlock  *_pre;
};

const size_t S_MEM_THRE        = 4096;//4K
const size_t S_POOL_STEP       = 8;
const size_t LARGE_POOL_STEP   = 4096;//4K
const size_t SMAX_SUB_POOL_CAP = 128;
const size_t LMAX_SUB_POOL_CAP = 64;

class XMemPool
{
public:
    XMemPool( const size_t& maxBlockSize, const size_t& lmaxSubPoolCapability = LMAX_SUB_POOL_CAP,\
        const size_t& lpoolXep = LARGE_POOL_STEP );
    ~XMemPool();
public:
    XMemBlock *alloc( size_t size );
    void free( XMemBlock* block );
    void destroy();
    size_t getTotalMemSize() const { return _totalMemSize; };

private:
    const size_t _maxBlockSize;
    const size_t _lmaxSubPoolCapability;
    static const size_t\
                 _smaxSubPoolCapability = SMAX_SUB_POOL_CAP;
    const size_t _lpoolXep;
    static const size_t\
                 _spoolXep = S_POOL_STEP;
    XMemBlock **_ssubPool[2];
    XMemBlock **_lsubPool[2];
    size_t       _ssubPoolNum;
    size_t       _lsubPoolNum;
    size_t      *_lsubPoolSize;
    size_t      *_ssubPoolSize;
    size_t       _totalMemSize;
};

#endif /* XMEMPOOL_H_ */
