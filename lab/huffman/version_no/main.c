/*
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 * Copyright (C) 2007 by Rafael Santiago
 *
 * #main.c#
 * por Rafael Santiago
 * 
 * main file do projeto 
 *
 */
#include "huffman.h"
#include "version_no.h"
/*./main c|d <entrada> <saida>*/
int main(int argc, char **argv){
 int insz, outsz;
 if(argc>=4){
  switch(*argv[1]){
   case 'c':
    insz  = getFileSize(argv[2]); 
    if(huffmanFileCompress(argv[2],argv[3])){
     printf("info: arquivo '%s' foi comprimido com sucesso em '%s'.\n",argv[2],argv[3]);
     outsz = getFileSize(argv[3]);
     printf("      arquivo '%s' comprimido em %.0f%% de seu tamanho original.\n",argv[3],100-getCompressionPercent(outsz,insz));
    } 
    else
     printf("info: arquivo '%s' nao pode ser comprimido.\n",argv[2]);
   break;
   case 'd':
    if(huffmanFileDecompress(argv[2],argv[3]))
     printf("info: arquivo '%s' foi descomprimido com sucesso em '%s'.\n",argv[2],argv[3]);
    else
     printf("info: arquivo '%s' nao pode ser descomprimido.\n",argv[2]);
   break;
   default:
    printf("info: opcao '%s' e invalida!\n",argv[1]);
   break;      
  }
 }
 else {
  if (argc > 1 && *argv[1] == 'v') { 
    printf("versao: %s\n", VERSION);
  } else {
   printf("use: %s c|d|v <in> <out>\nc = comprimir\nd = descomprimir\n",argv[0]);
  }
 }
 return(0);
}
