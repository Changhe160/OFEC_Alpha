/*
**  mempool.h
**
**  Header for mempool.c
**
**  The type alloc_handle_t provides an opaque reference to the
**  alloc pool - only the alloc routines know its structure.
*/
#ifndef EPANET_MEMPOOL_H
#define EPANET_MEMPOOL_H

namespace epanet {
	typedef struct
	{
		long  dummy;
	}  alloc_handle_t;

	typedef struct alloc_hdr_s
	{
		struct alloc_hdr_s *next;   /* Next Block          */
		char               *block,  /* Start of block      */
			*free,   /* Next free in block  */
			*end;    /* block + block size  */
	}  alloc_hdr_t;

	/*
	**  alloc_root_t - Header for the whole pool.
	*/

	typedef struct alloc_root_s
	{
		alloc_hdr_t *first,    /* First header in pool */
			*current;  /* Current header       */
	}  alloc_root_t;

	class MemPool {
	private:
		/*
		**  root - Pointer to the current pool.
		*/
		alloc_root_t *root = nullptr;   // static

	public:
		alloc_handle_t *AllocInit(void);
		char           *Alloc(long);
		alloc_handle_t *AllocSetPool(alloc_handle_t *);
		void            AllocReset(void);
		void            AllocFreePool(void);
	};
}
#endif //!EPANET_MEMPOOL_H