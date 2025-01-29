/* HASH.H
**
** Header file for Hash Table module HASH.C
**
*/
#ifndef EPANET_HASH_H
#define EPANET_HASH_H

namespace epanet {
	struct HTentry
	{
		char 	*key;
		int 	data;
		struct	HTentry *next;
	};

	typedef struct HTentry *HTtable;

	HTtable *HTcreate(void);
	int     HTinsert(HTtable *, char *, int);
	int 	HTfind(HTtable *, char *);
	char    *HTfindKey(HTtable *, char *);
	void	HTfree(HTtable *);
}
#endif //!EPANET_HASH_H