 /*
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 * Copyright (C) 2007 by Rafael Santiago
 *
 * #btree.c#
 * por Rafael Santiago
 *
 * implementa ''btree.h``
 *
 */
#include "btree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*malloc generico[mais barato duh!]*/
void *mallocX(int size){
 void *ptmem;
 
 ptmem = (void *) malloc(size);
 
 if(ptmem == 0){
  printf("run forest run: erro sem memoria suficiente!\n");
  exit(1);
 }
 
 return( ptmem );
}
/*linka dois nos em um terceiro*/
BTREE *link2NodesOn3(BTREE *a, BTREE *b){
 BTREE *n3;
 
 createBTREEnode(n3);
 createBTREEnode(n3->l);
 createBTREEnode(n3->r);
 
 n3->freq = a->freq+b->freq;

 memcpy(n3->l,a,sizeof(BTREE));
 memcpy(n3->r,b,sizeof(BTREE));

 return( n3 );
}
/*joga em @way o caminho exato para @wanted em @btree*/
void getNodeWay(BTREE *btree, int wanted, char *way, int *found){
 char *p;
 if(!*found){
  if(btree!=0){ 
   if(btree->byte == wanted){
    *found = 1;
    return;
   } 
   p = way;
   *p = '0';
   *(p+1) = 0;
   getNodeWay(btree->l,wanted,++p,found);
   if(!*found){
    memset(way,0,sizeof(way));
    p = way;
    *p = '1';
    *(p+1) = 0;
    getNodeWay(btree->r,wanted,++p,found);
   }
  }
 }
 if(!*found) memset(way,0,sizeof(way));
}
/*percorre a @btree baseando-se em @way, retornando ao fim o elemento*/
int getDataOnWay(BTREE *btree, char *way){
 char *p = way;
 BTREE *tp = btree;
 
 while(*p && tp){
  switch(*p){
   case '0': tp = tp->l; break;
   case '1': tp = tp->r; break;
  }
  p ++;
 }
 
 if(tp) return(tp->byte);
 else return(-1);
}
/*destroy sub-nodos de uma arvore*/
void destroyBTREESubNodes(BTREE *btree){
 if(btree){
  destroyBTREE(btree->l);
  destroyBTREEnode(btree->l);
  destroyBTREE(btree->r);
  destroyBTREEnode(btree->r); 
 }
}
/*limpa um array de nodos BTREE de tamanho @array_size*/
void BTREENodeArrayCleanUp(BTREE **node, int array_size){
 int w;
 
 for(w=0;w<array_size;w++)
  if(node[w]) destroyBTREEnode(node[w]);
}

