/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../../types.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

static int _accacia_kbhit();
static unsigned char _accacia_getch();
static char _accacia_getkey();

void accacia_kbhit(struct hefesto_modio **modio) {
    int k = _accacia_kbhit();
    (*modio)->rtype = HEFESTO_VAR_TYPE_INT;
    (*modio)->ret = (int *) malloc(sizeof(int));
    *(int *)(*modio)->ret = k;
    (*modio)->rsize = sizeof(int);
}

void accacia_getch(struct hefesto_modio **modio) {
    unsigned char k = _accacia_getch();
    (*modio)->rtype = HEFESTO_VAR_TYPE_INT;
    (*modio)->ret = (int *) malloc(sizeof(int));
    *(int *)(*modio)->ret = (int) k;
    (*modio)->rsize = sizeof(int);
}

void accacia_getkey(struct hefesto_modio **modio) {
    char k = _accacia_getkey();
    (*modio)->rtype = HEFESTO_VAR_TYPE_INT;
    (*modio)->ret = (int *) malloc(sizeof(int));
    *(int *)(*modio)->ret = (int) k;
    (*modio)->rsize = sizeof(int);
}

/*
 * retorna 1 caso houver um evento de tecla e 
 * 0 se nao houver nda
 */
static int _accacia_kbhit() {
    int res;
    struct termios attr, oldattr;

    /*
     * forcando a leitura da saida... pois observei que se algo fosse
     * mudado na tela antes da chamada do kbhit, a tela so era
     * atualizada apos um evento de tecla ou quando imprimia outra coisa
     * o que resultaria na descarga da ultima atualizacao para armazenar
     * esse dado novo no buffer, mas dessa forma causava um efeito desagradavel
     * no cursor... pois tinha que restaurar a posicao antiga toda vez... isso
     * fazia ele piscar...  
     */
    getc(stdout);
    /*
     * essa parte do codigo foi baseada no kbhit.c de Pete Wilson
     */
    tcgetattr(STDIN_FILENO,&attr);/*configuracoes atuais do tty*/
    oldattr = attr;
    attr.c_lflag = ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&attr);
    /*
     *  thanks Pete Wilson! ;)
     */
    ioctl(STDIN_FILENO, FIONREAD, &res);/*lendo a quantidade de caracteres lidos pd ser 1 ou 0*/

    tcsetattr(STDIN_FILENO,TCSANOW,&oldattr);/*voltando as configuracoes do usuario*/

    return(res);
}
/*
 * espera por um evento de tecla e
 * retorna a tecla digitada
 */
static unsigned char _accacia_getch() {
    unsigned char c;
    struct termios attr, oldattr;
    getc(stdout); /*forcando possivel atualizacao da tela*/
    tcgetattr(STDIN_FILENO,&attr);
    oldattr = attr;
    attr.c_lflag = ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&attr);
    read(STDIN_FILENO,&c,1);
    tcsetattr(STDIN_FILENO,TCSANOW,&oldattr);
    return(c);
}
/*
 * retorna o char caso houver um evento de tecla  
 * mas nao imprime na tela
 */
static char _accacia_getkey() {
    int res;
    struct termios attr, oldattr;
    /*
     * forcando a leitura da saida... pois observei que se algo fosse
     * mudado na tela antes da chamada do kbhit, a tela so era
     * atualizada apos um evento de tecla ou quando imprimia outra coisa
     * o que resultaria na descarga da ultima atualizacao para armazenar
     * esse dado novo no buffer, mas dessa forma causava um efeito desagradavel
     * no cursor... pois tinha que restaurar a posicao antiga toda vez... isso
     * fazia ele piscar...  
     */
    getc(stdout);
    /*
     * essa parte do codigo foi baseada no kbhit.c de Pete Wilson
     */
    tcgetattr(STDIN_FILENO,&attr); /*configuracoes atuais do tty*/
    oldattr = attr;
    attr.c_lflag = ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&attr);
    /*
     *  thanks Pete Wilson! ;)
     */
    res = _accacia_getch();
    tcsetattr(STDIN_FILENO,TCSANOW,&oldattr); /*voltando as configuracoes do usuario*/
    return(res);
}
