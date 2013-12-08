/*
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 * Copyright (C) 2007 by Rafael Santiago
 *
 * #huffman.h#
 * por Rafael Santiago
 *
 * engloba funcoes relativas a codificacao de huffman
 *
 */
#ifndef _HUFFMAN_H
#define _HUFFMAN_H 1

#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*macros*/
#define dataBTREEassigment(x,y)(\
 x->freq = y->freq,\
 x->byte = y->byte,\
 x->l    = y->l,\
 x->r    = y->r )

#define hmsafeindex(x,y) ( (x) % (y) )

#define hmasciitobit(x) ( (x - 48) )

/*constantes*/
#define HMDATASIZE 10000

#define HMFILETEMP ".temp.huff"

#define HMHEADER "HM"

#define HMNODE '*'

#define HMTERMINATOR "* $"
#define HMTERMCHAR '$'

/*contextos*/
struct _HUFFMANCODESCTX {
 char code[HMDATASIZE];
};

/*prototypes*/
int fileFreqExtract(BTREE *[256], char *);
void bubbleBTREEArray(BTREE *[256]);
int getFileSize(char *);
void buildHuffmanCodes(BTREE *[256], int);
void getHuffmanCodesFromBTREE(struct _HUFFMANCODESCTX [256], BTREE *);
int huffmanFileCompress(char *, char *);
void writeBTREEOnFile(FILE *, BTREE *, BTREE *);
void shiftStringLeft(char *, int);
int huffmanFileDecompress(char *, char *);
void getBTREEConfigFromFile(unsigned char *, FILE *);
void rebuildBTREEFromStrConfig(BTREE *, unsigned char *, BTREE *);
void bytetostrbyte(unsigned char *, unsigned char);
float getCompressionPercent(int, int);

#ifdef __cplusplus
}
#endif

#endif
