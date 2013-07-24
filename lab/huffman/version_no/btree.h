/*
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 * Copyright (C) 2007 by Rafael Santiago
 *
 * #btree.h# 
 * por Rafael Santiago
 *
 * engloba funcoes envolvendo arvores binarias.
 *
 */
#ifndef _BTREE_H
#define _BTREE_H 1

#ifdef __cplusplus
extern "C" {
#endif

/*contexto*/
typedef struct __BTREENODE {
 int byte;
 int freq;
 struct __BTREENODE *l, *r;
}BTREE;

/*macros*/
#define createBTREEnode(n) (\
 n = (BTREE *)mallocX(sizeof(BTREE)),\
		        n->byte = -1,\
                         n->freq = 0,\
                            n->l = 0,\
                            n->r = 0 )

#define destroyBTREEnode(n) ( n = 0 )
#define destroyBTREE(n)(\
 destroyBTREESubNodes(n),\
 destroyBTREEnode(n) )
/*prototypes*/
void *mallocX(int);
BTREE *link2NodesOn3(BTREE *, BTREE *);
void getNodeWay(BTREE *, int, char *, int *);
int getDataOnWay(BTREE *, char *);
void destroyBTREESubNodes(BTREE *);
void BTREENodeArrayCleanUp(BTREE **, int);

#ifdef __cplusplus
}
#endif


#endif
