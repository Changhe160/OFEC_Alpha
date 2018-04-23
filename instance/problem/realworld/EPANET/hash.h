/* HASH.H
**
** Header file for Hash Table module HASH.C
**
*/
#ifndef HASH_H
#define HASH_H
#define HTMAXSIZE 1999
#define NOTFOUND  0
namespace OFEC {
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
#endif