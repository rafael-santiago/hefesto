/*
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 * Copyright (C) 2007 by Rafael Santiago
 *
 * #huffman.c#
 * por Rafael Santiago
 *
 * implementa ''huffman.h``
 *
 */
#include "huffman.h"
#include <string.h>
/*extrai frequencia do arquivo indicado em @file*/
int fileFreqExtract(BTREE *asciiset[256], char *file) {
    FILE *fp;
    int w;
    unsigned char c;

    if (!(fp = fopen(file,"rb"))) {
        printf("huffman error: impossivel abrir arquivo '%s'\n",file);
        return(0);
    }

    /*alocando a tabela ascii*/
    for (w = 0; w < 256; w++) {
        createBTREEnode(asciiset[w]);
        asciiset[w]->byte = w;
    }

    /*extracao da frequencia*/
    c = getc(fp);
    while (!feof(fp)) {
        asciiset[c]->freq ++;
        c = getc(fp);
    }

    fclose(fp);
    return(1);
}
/*ordena de modo crescente o array de nodos BTREE*/
void bubbleBTREEArray(BTREE *asciiset[256]){
    BTREE *aux;
    int swap,w;

    swap = 1;
    createBTREEnode(aux);

    while(swap){
        for(w = 1, swap = 0; w < 256 && !swap; w++) {
            if ((asciiset[w]->freq < asciiset[w-1]->freq && asciiset[w]->freq) ^ (!asciiset[w-1]->freq && asciiset[w]->freq)) {
                dataBTREEassigment(aux,asciiset[w]);
                dataBTREEassigment(asciiset[w],asciiset[w-1]);
                dataBTREEassigment(asciiset[w-1],aux);
                swap=1;
            }
        }
    }

    destroyBTREEnode(aux);
}
/*dado um conjunto de folhas, constroi a arvore (raiz em @asciiset[0])
  @size = tamanho total de bytes na entrada original.*/
void buildHuffmanCodes(BTREE *asciiset[256], int size){
    BTREE *aux;
    int w;

    do
    {
        bubbleBTREEArray(asciiset);
        aux = link2NodesOn3(asciiset[0],asciiset[1]); 
        asciiset[1]->freq = 0;
        asciiset[0]->freq = 0;
        destroyBTREEnode(asciiset[0]);
        asciiset[0] = aux;
    } while (aux->freq < size);

}
/*retorna tamanho em bytes do arquivo indicado em @file*/
int getFileSize(char *file){
    FILE *fp;
    int bytes;

    if (!(fp = fopen(file,"rb"))) return(0);

    fseek(fp,0L,SEEK_END);

    bytes = ftell(fp);

    fclose(fp);

    return( bytes );
}
/*monta o codigo para cada elemento da arvore em sua posicao de array correspondente
  i.e: 'a' @huffmancodesctx['a'].code = xxx*/
void getHuffmanCodesFromBTREE(struct _HUFFMANCODESCTX huffmancodesctx[256], BTREE *root){
    int w, found;

    for (w = 0, found = 0; w < 256; w++, found = 0)
        getNodeWay(root,w,huffmancodesctx[w].code,&found);
}
/*compressiona um arquivo*/
int huffmanFileCompress(char *input, char *output){
    FILE *fin, *fout;
    BTREE *asciiset[256];
    struct _HUFFMANCODESCTX huffmancodesctx[256];
    unsigned char bytebuf[HMDATASIZE],c,way[HMDATASIZE],temp[HMDATASIZE];
    int w, found, x, y;

    if (!fileFreqExtract(asciiset,input)) return(0);
    x = getFileSize(input);

    sprintf(bytebuf,"%d;",x);
    for(w=0;bytebuf[w]!=0;w++) asciiset[bytebuf[w]]->freq ++;

    buildHuffmanCodes(asciiset,x);

    /*a raiz da arvore estara na frente da heap*/
    getHuffmanCodesFromBTREE(huffmancodesctx,asciiset[0]);
    if (!(fin = fopen(input,"rb"))) {
        printf("huffman error: impossivel abrir arquivo de entrada!\n");
        BTREENodeArrayCleanUp(asciiset,256);
        return(0);
    }

    if(!(fout = fopen(HMFILETEMP,"wb"))){
        printf("huffman error: impossivel criar arquivo temporario!\n");
        fclose(fin);
        BTREENodeArrayCleanUp(asciiset,256);
        return(0);
    }

    writeBTREEOnFile(fout,asciiset[0],asciiset[0]);

    /*gravando o tamanho original depois da configuracao da arvore.*/
    for(w=0,x=0;bytebuf[w]!=0;w++){
        found = 0;
        getNodeWay(asciiset[0],bytebuf[w],temp,&found);
        for (y = 0; temp[y] != 0; way[x++] = temp[y++], way[x] = 0);
    }

    x = w = 0;
    goto __processbitblock0;

    while(!feof(fin)){
        found = 0;
        getNodeWay(asciiset[0],c,way,&found);
        if(w < 8){
            __processbitblock0:
            for(;w<8 && way[x]!=0;bytebuf[w] = way[x],hmsafeindex(w++,HMDATASIZE),hmsafeindex(x++,HMDATASIZE),bytebuf[w]=0);
            if(way[x] == 0) x = 0;
            if(w == 8) goto __writebyte0;
            ;
        }
        else
        {
            __writebyte0:
            for(w=0,c=0;w<8;w++)
            c = c << 1 | (unsigned char) hmasciitobit(bytebuf[w]);
            fwrite(&c,sizeof(c),1,fout); 
            w = 0;
            memset(bytebuf,0,sizeof(char)*HMDATASIZE);
            if(x>0)
                goto __processbitblock0;
            ;
        }
        c = getc(fin);
    }

    if(w>0){
        for(x=0,c=0;x<w;x++)
         c = c << 1 | (unsigned char) hmasciitobit(bytebuf[x]);
        while(w++<8)/*padding*/
        c = c << 1;
        fwrite(&c,sizeof(c),1,fout); 
    }

    fclose(fin);
    fclose(fout);

    if(!(fin = fopen(HMFILETEMP,"rb"))){
        printf("huffman error: impossivel ler arquivo temporario!\n");
        BTREENodeArrayCleanUp(asciiset,256);
        remove(HMFILETEMP);
        return(0);
    }

    if(!(fout = fopen(output,"wb"))){
        printf("huffman error: impossivel criar arquivo de saida!\n");
        BTREENodeArrayCleanUp(asciiset,256);
        fclose(fin);
        remove(HMFILETEMP);
        return(0);
    }

    c = getc(fin);
    while(!feof(fin)){
        fwrite(&c,sizeof(c),1,fout);
        c = getc(fin);
    }

    fclose(fin);
    fclose(fout);
    BTREENodeArrayCleanUp(asciiset,256);

    remove(HMFILETEMP);
    return(1);
}
/*descompressiona um arquivo*/
int huffmanFileDecompress(char *input, char *output){
    FILE *fin, *fout;
    unsigned char way[HMDATASIZE],c, bytebuf[HMDATASIZE], data[HMDATASIZE];
    BTREE *root;
    int fsize, w, found, x, bytestotal;

    if(!(fin = fopen(input,"rb"))){
        printf("huffman error: impossivel ler arquivo de entrada!\n");
        return(0);
    }

    if(!(fout = fopen(HMFILETEMP,"wb"))){
        printf("huffman error: impossivel criar arquivo temporario!\n");
        fclose(fin);
        return(0);
    }

    getBTREEConfigFromFile(bytebuf,fin);
    if(!(bytebuf[0] == 'H' && bytebuf[1] == 'M')){
        printf("huffman error: cabecalho do arquivo de entrada e invalido ou esta corrompido!\n");
        fclose(fin);
        fclose(fout);
        remove(HMFILETEMP);
        return(0);
    }

    shiftStringLeft(bytebuf,HMDATASIZE);
    shiftStringLeft(bytebuf,HMDATASIZE);
    createBTREEnode(root);
    rebuildBTREEFromStrConfig(root,bytebuf,root);

    memset(bytebuf,0,sizeof(char)*HMDATASIZE);
    memset(way,0,sizeof(char)*HMDATASIZE);
    x = w = 0;
    /*pegando o tamanho original do arquivo*/
    do
    {
        c = getc(fin);
        for(fsize = 0;way[fsize]!=0;bytebuf[w++] = way[fsize++],bytebuf[w] = 0){
            if((found = getDataOnWay(root,bytebuf))>-1){
                if(found == ';') break;
                data[x++] = found;
                data[x  ] = 0;
                w = 0;
            }
        }
        if(found != ';'){
            found = 0;
            memset(way,0,sizeof(way));
            bytetostrbyte(way,c);
        }
        else
            for(x=0;x<fsize;x++)
                shiftStringLeft(way,HMDATASIZE);
    }while(/*getDataOnWay(root,bytebuf)!=';'*/found!=';' && !feof(fin));
    fsize = atoi(data);
    bytestotal = 0;
    x = w = 0;
    memset(bytebuf,0,sizeof(char)*HMDATASIZE);
    while(bytestotal < fsize){
        for(x = 0;way[x]!=0 && bytestotal<fsize;bytebuf[w++] = way[x++],bytebuf[w] = 0){
            if((found = getDataOnWay(root,bytebuf))>-1){
                fwrite(&found,sizeof(char),1,fout);
                bytestotal ++;
                w = 0;
            }
        }
        found = 0;
        memset(way,0,sizeof(way));
        bytetostrbyte(way,c);
        c = getc(fin);
    }

    destroyBTREEnode(root);
    fclose(fin);
    fclose(fout);

    if(!(fin = fopen(HMFILETEMP,"rb"))){
        printf("huffman error: impossivel ler arquivo temporario!\n");
        remove(HMFILETEMP);
        return(0);
    }

    if(!(fout = fopen(output,"wb"))){
        printf("huffman error: impossivel criar arquivo de saida!\n");
        fclose(fin);
        remove(HMFILETEMP);
        return(0);
    }

    c = getc(fin);

    while(!feof(fin)){
        fwrite(&c,sizeof(c),1,fout);
        c = getc(fin);
    }

    fclose(fin);
    fclose(fout);
    remove(HMFILETEMP);

    return(1);
}
/*retorna a representacao em string binaria do byte passado*/
void bytetostrbyte(unsigned char *strbyte, unsigned char byte){
    int w = strlen(strbyte),b;
    for(b=0;/*(w)%8 || !w*/b<8;w++, byte = byte << 1, b++)
     strbyte[w] = (byte >> 7) + 48;
}
/*recupera a configuracao da arvore gravada no arquivo compressionado*/
void getBTREEConfigFromFile(unsigned char *config, FILE *fp){
    unsigned char c;
    int w;

    c = getc(fp);
    w = 0;
    config[hmsafeindex(w++,HMDATASIZE)] = c;
    config[hmsafeindex(w,HMDATASIZE)]   = 0;
    c = getc(fp);
    config[hmsafeindex(w++,HMDATASIZE)] = c;
    config[hmsafeindex(w,HMDATASIZE)]   = 0;
    c = getc(fp);
    config[hmsafeindex(w++,HMDATASIZE)] = c;
    config[hmsafeindex(w,HMDATASIZE)]   = 0;
    while(!(config[hmsafeindex(w-3,HMDATASIZE)] == '*' && config[hmsafeindex(w-2,HMDATASIZE)] == ' ' && config[hmsafeindex(w-1,HMDATASIZE)] == '$') && !feof(fp)){
        c = getc(fp);
        config[hmsafeindex(w++,HMDATASIZE)] = c;
        config[hmsafeindex(w,HMDATASIZE)]   = 0;
    }
    config[hmsafeindex(w-3,HMDATASIZE)] = 0;
}
/*grava a configuracao da arvore usada na compressao do arquivo*/
void writeBTREEOnFile(FILE *fp, BTREE *root, BTREE *next){
    if(next){
        if(next == root) fprintf(fp,"%s",HMHEADER);
        if(next->byte > -1){
            if(next->byte == HMNODE ^ next->byte == ' ')
                fprintf(fp," ");
            fprintf(fp,"%c",next->byte);
        } 
        else
            fprintf(fp,"%c",HMNODE);
        writeBTREEOnFile(fp,root,next->l);
        writeBTREEOnFile(fp,root,next->r);
        if(next == root) fprintf(fp,"%s",HMTERMINATOR);
    }
}
/*reconstroi arvore a partir da configuracao recuperada*/
void rebuildBTREEFromStrConfig(BTREE *root, unsigned char *str, BTREE *next){
    switch(*str){
        case HMNODE:
            createBTREEnode(next->l);
            shiftStringLeft(str,HMDATASIZE);
            rebuildBTREEFromStrConfig(root,str,next->l);
            createBTREEnode(next->r);
            shiftStringLeft(str,HMDATASIZE);
            rebuildBTREEFromStrConfig(root,str,next->r);
            break;
        case ' ':
            str ++;
            next->byte = *str;
            shiftStringLeft(str,HMDATASIZE);
            break;
        default:
            next->byte = *str;
            break;
    }
}
/*elimina o byte mais a esquerda em uma string*/
void shiftStringLeft(char *str, int size){
    int w;
    for(w=1;w<size/*str[w]!=0*/;str[w-1] = str[w],str[w++] = 0);
}
/*retorna taxa de compressao*/
float getCompressionPercent(int x, int y){
    return( (((float)x / (float)y) * 100) );
}
