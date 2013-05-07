/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "regex.h"
#include "mem.h"
#include <string.h>

static const unsigned char *match_regex(const unsigned char *text,
                                        const unsigned char *tbegin,
                                        const unsigned char *tend, 
                                        const unsigned char *regexp,
                                        const unsigned char *rbegin,
                                        const unsigned char *rend,
                                        size_t *mend,
                                        const unsigned char recurssive);

static const unsigned char *next_meta_regex_symbol(const unsigned char *regexp);

#define regex_meta_char(c) ( (c) == '^' ^ (c) == '$' ^ (c) == '[' ^ (c) == ']' ^\
                             (c) == '(' ^ (c) == ')' ^ (c) == '{' ^ (c) == '}' ^\
                             (c) == '?' ^ (c) == '*' ^ (c) == '+' ^ (c) == '.' )

int check_regex(const unsigned char *regex){

    const unsigned char *p;
    unsigned char bkslashed, c, ored;
    int ac, ar;
    unsigned char aux[2][20];

    if (regex == NULL || *regex == 0) return 0;

    for (p = regex; *p != 0 ; p++) {

        switch (*p) {

            case '(':

                c = 0;
                bkslashed = 0;
                ored = 0;

                p++;

                while (*p && *p != ')') {

                    if (*p == '|' && !c)
                        return 0;
                    else if (*p == '|' && c)
                        c = 0;
                    else if (bkslashed || *p != '\\')
                        c = 1;

                    ored += (!bkslashed && *p == '|');

                    if (!bkslashed && *p == '\\')
                        bkslashed = 1;
                    else if (bkslashed)
                        bkslashed = 0;

                    p += 1 + bkslashed;

                }

                if (!c || !ored || *p != ')') return 0;

                break;

            case '[':

                c = 0;
                bkslashed = 0;

                p++;

                while (*p && *p != ']') {

                    if (!c && (*p != '^' || bkslashed)) 
                        c  = 1;
                    // [c-]
                    if (!bkslashed && *p == '-' && *(p + 1) == ']') return 0; 

                    if(!bkslashed && *p == '\\') 
                        bkslashed = 1;
                    else if(bkslashed) 
                        bkslashed = 0;

                    p += 1 + bkslashed;

                }

                if (!c || *p != ']') return 0;

                break;

            case '{':

                p++;
                c = 0;
                ac = 0;
                ar = 0;

                aux[0][0] = 0;
                aux[1][0] = 0;

                while (*p && *p!='}') {

                    if (!c && *p != ',') c = 1;

                    if (*p != ',' && !isdigit(*p)) return 0;

                    if (*p == ',' && ar < 1) {
                        ar++;
                        ac = 0;
                    } else if (*p == ',' && ar == 1) {
                        return 0; // {d,d,d}
                    } else {
                        aux[ar][ac] = *p;
                        ac = (ac + 1) % sizeof(aux[0]);
                        aux[ar][ac] = 0;
                    }

                    p++;

                }

                if (!c || *p != '}') return 0;

                if (!( (!aux[0][0] && atoi((char *)aux[1])) ||
                        (atoi((char *)aux[0]) && !aux[1][0]) ||
                       (atoi((char *)aux[0]) && atoi((char *)aux[1])) ) )
                    return 0;

                break;

            case '$':

                if(*(p + 1)) return 0;

                break;

            case '\\':

                p += 2; // "backslashed" char

                break;

        }

    }

    return 1;
}

void preprocess_usr_regex(unsigned char *dest, const unsigned char *src,
                          const size_t sdest, const size_t ssrc){

    unsigned char *sp, *lsp, *nsp, *dp, *de;
    unsigned char *tmp;
    unsigned char *ebuf, *ebp;
    int open;

    memset(dest, 0, sdest);

    if (ssrc <= 0) return;

    tmp = (unsigned char *) hefesto_mloc(ssrc);

    memset(tmp, 0, ssrc);
    memcpy(tmp,src,ssrc);

    de = dest + sdest - 1;
    sp = tmp;
    dp = dest;
    //sp_end = sp + ssrc - 1;
    if (sp < tmp + ssrc) nsp = sp + 1;
    lsp = sp;

    while (sp < (tmp + ssrc) && *sp && dp < de) {

    switch(*sp){

        case '{':

            for (; *sp && *(sp - 1)!='}' && dp < de; sp++, dp++) *dp = *sp;
            *dp = *lsp;
            dp++;
            sp--;

            break;

        case '?':
        case '*':
        case '+':

            if (*lsp != ']' && *lsp != '}' && *lsp != ')') {
                *dp = *sp;
                dp++;
                *dp = *lsp;
                dp++;
            } else {

                switch (*lsp) {

                    case ']':

                        *dp = *lsp;

                        for (open = 1; dp != dest && open > 0; dp--) {

                            if (*dp == '\\') {
                                dp--;
                                continue;
                            }

                            if (*dp == '[') open--;
                            else if (*dp == ']') open++;

                        }
                        *dp = *sp;

                        ebuf = (unsigned char *) hefesto_mloc(0xffff);

                        for (ebp = ebuf, lsp = ++dp, open = 1; 
                                ebp != (ebuf+0xffff) && open > 0;lsp++,ebp++) {
                            if (*lsp == '\\') {

                                *ebp = *lsp;
                                lsp++;
                                ebp++;
                                *ebp = *lsp;

                                continue;

                            }

                            *ebp = *lsp;

                            if (*lsp == ']') open--;
                            else if (*lsp == '[') open++; 

                        }

                        lsp = ebp;

                        *dp = '[';
                        for (dp++, ebp = ebuf; ebp != lsp; ebp++, dp++)
                            *dp = *ebp;

                        *dp = ']';
                        dp++;

                        free(ebuf);

                        break;

                    case '}':

                        *dp = *lsp;

                        for(open = 1; dp != dest && open > 0; dp--) {

                            if (*dp == '\\') {
                                dp--;
                                continue;
                            }

                            if (*dp == '{') open--;
                            else if (*dp == '}') open++;

                        }
                        *dp = *sp;

                        ebuf = (unsigned char *) hefesto_mloc(0xffff);

                        for (ebp = ebuf, lsp = ++dp, open = 1;
                                 ebp != (ebuf + 0xffff) && open > 0;
                                                        lsp++, ebp++) {
                            if (*lsp == '\\') {

                                *ebp = *lsp;
                                lsp++;
                                ebp++;
                                *ebp = *lsp;

                                continue;

                            }

                            *ebp = *lsp;

                            if (*lsp == '}') open--;
                            else if (*lsp == '{') open++; 

                        }

                        lsp = ebp;

                        *dp = '{';
                        for (dp++, ebp = ebuf; ebp != lsp; ebp++, dp++)
                            *dp = *ebp;

                        *dp = '}';
                        dp++;

                        free(ebuf);

                        break;

                    case ')':

                        *dp = *lsp;

                        for (open = 1; dp != dest && open > 0; dp--) {

                            if (*dp == '\\') {
                                dp--;
                                continue;
                            }

                            if(*dp == '(') open--;
                            else if(*dp == ')') open++;

                        }
                        *dp = *sp;

                        ebuf = (unsigned char *) hefesto_mloc(0xffff);

                        for (ebp = ebuf, lsp = ++dp, open = 1;
                                    ebp != (ebuf + 0xffff) && open > 0;
                                                            lsp++, ebp++) {
                            if (*lsp == '\\') {

                                *ebp = *lsp;
                                lsp++;
                                ebp++;
                                *ebp = *lsp;

                                continue;

                            }

                            *ebp = *lsp;

                            if (*lsp == ')') open--;
                            else if (*lsp == '(') open++; 

                        }

                        lsp = ebp;

                        *dp = '(';
                        for (dp++, ebp = ebuf; ebp != lsp; ebp++, dp++)
                            *dp = *ebp;

                        *dp = ')';
                        dp++;

                        free(ebuf);

                        break;

                }
            }

            break;

            default:
                //if (sp < sp_end) {
                if (!(sp < (tmp + ssrc) && (nsp < (tmp + ssrc) &&
                                            (*nsp == '{' ||
                                             *nsp == '?' || *nsp == '*' ||
                                             *nsp == '+' /*|| *nsp == '-'*/)))) {
                    *dp = *sp;
                    dp++;
                }
                //}
                break;

        }

        lsp=sp;
        sp++;
        nsp = sp + 1;

    }

    free(tmp);

    *dp = 0;

    dest[ssrc] = 0;
}

static const unsigned char *match_regex(const unsigned char *text,
                                        const unsigned char *tbegin,
                                        const unsigned char *tend,
                                        const unsigned char *regex,
                                        const unsigned char *rbegin,
                                        const unsigned char *rend,
                                        size_t *mend,
                                        const unsigned char recurssive) {

    const unsigned char *r, *t;
    unsigned char *rr;
    int fmatch, cmatch;
    size_t o;
    const unsigned char *match = NULL;
    unsigned char oc_floorb[20], oc_ceilb[20];
    int oc_floor, oc_ceil;
    unsigned char *buf, *sbuf;

    if (regex == rend) {
        *mend = (size_t) text;
        return (text - 1);
    }

    switch (*regex) {

        case '[': //done

            if (*(regex + 1) != '^') {
                match = NULL;
                t = text;

                for (o = 1, rr = (unsigned char *)regex + 1;
                                    rr != rend && o > 0; rr++) {
                    if (*rr == '\\') continue;

                    if (*rr == '[') o++;
                    else if (*rr == ']') o--;

                }

                for (match = NULL, r = regex + 1; !match && r != rr; r++)
                    if (match_regex(t, tbegin, tend, r, rbegin, rend, mend, 0)) {
                        match = t;
                    }

            } else {
                match = NULL;
                t = text;

                for (o = 1, rr = (unsigned char *)regex + 2;
                                    rr != rend && o > 0; rr++) {
                    if (*rr == '\\') continue;

                    if (*rr == '[') o++;
                    else if (*rr == ']') o--;

                }

                for (match = text, r = regex + 2; match && r != rr; r++) {

                    if (!match_regex(t,tbegin,tend,r,rbegin,rend,mend,0))
                        match = text;
                    else
                        match = NULL;

                }

            }

            if (match && recurssive) {
                if (match_regex(t + 1, tbegin, tend,
                                next_meta_regex_symbol(regex),
                                rbegin, rend, mend, 1)) {
                    match = t;
                } else {
                    match = NULL;
                }
            }

            break;

        case '*': //done

            // regex+1 pode ocorrer nenhuma ou mais vezes

            match = NULL;
            for (t = text, o = 1; t <= tend && !match && o; t++) {
                if (match_regex(t, tbegin, tend,
                                next_meta_regex_symbol(regex + 1),
                                rbegin, rend, mend, 1)) {
                    match = t;
                }

                if (!match)
                    o = (match_regex(t, tbegin,
                                     tend, regex + 1,
                                     rbegin, rend, mend, 0) != NULL);
            }

            // aqui nao ha reestabelecimento da recurssao pois toda regex
            // ja tera sido combinada no primeiro statement do loop de
            // busca anterior.

            break;

        case '?': //done

            match = NULL;

            if (match_regex(text, tbegin, tend,
                            next_meta_regex_symbol(regex + 1),
                            rbegin, rend, mend, 1)) {
                match = text;
            }

            if (!match) {
                if (match_regex(text, tbegin, tend, regex + 1,
                                rbegin, rend, mend, 0)) {
                    if (match_regex(text + 1, tbegin, tend,
                                    next_meta_regex_symbol(regex + 1),
                                    rbegin, rend, mend, 1)) match = text + 1;
                }
            }

            break;

        case '$': //done

            if (text == tend)
                match = text - 1;
            else
                match = NULL;

            break;

        case '+': //done

            // casa *(regex+1) uma ou mais vezes.

            t = text;
            o = 0;

            if (match_regex(t, tbegin, tend,
                            regex + 1, rbegin, rend, mend, 0)) match = t;

            while (t<=tend && !o && match) {

                t++;

                if (match)
                    o = (match_regex(t, tbegin, tend,
                                     next_meta_regex_symbol(regex + 1),
                                     rbegin, rend, mend, 1) != NULL);

                if (!o) t++;

                if (!o && match_regex(t, tbegin, tend, regex + 1, rbegin,
                                      rend, mend, 0)) {
                    match = t;
                } else if (!o) match = NULL;

            }

            break;

        case '{': //done

            // casa um simbolo {i,n} ou {i} ou {i,} vezes.

            for (o = 0, r = regex + 1;
                *r != ',' && *r != '}' && r != rend;
                                r++, o = (o + 1) % 20) {
                oc_floorb[o] = *r;
            }
            oc_floorb[o] = 0;

            o = 0;

            oc_floor = atoi((char *)oc_floorb);
            if (*r == ',') {
                for (r++; *r != '}' && r != rend; r++, o = (o + 1) % 20) {
                    oc_ceilb[o] = *r;
                }

                oc_ceilb[o] = 0;

                if (oc_ceilb[0]) 
                    oc_ceil = atoi((char *)oc_ceilb);
                else
                    oc_ceil = -2;
            }
            else oc_ceil = -1;

            for (o = 0, match = text;
                    (text + o) <= tend && match;
                            o += (match) ? 1 : 0) {
                match = match_regex(text + o, tbegin, tend,
                                    r + 1, rbegin, rend, mend, 0);
            }

            fmatch = (oc_ceil == -1) ? (o == oc_floor) : (o >= oc_floor);

            if (fmatch && oc_ceil > -1) {
                for (o = 0, match = text; (text + o) <= tend &&
                                        match; o += (match) ? 1 : 0) {
                    match = match_regex(text + o, tbegin, tend, r + 1,
                                        rbegin, rend, mend, 0);
                }
                cmatch = (o <= oc_ceil); 
            }
            else cmatch = 1;

            if (fmatch && cmatch) match = text;
            else match = NULL;

            if (match && recurssive) {

                if (match_regex(text + o, tbegin, tend,
                                next_meta_regex_symbol(r + 1),
                                rbegin, rend, mend, 1)) {
                    match = text + o;
                } else {
                    match = NULL;
                }
            }

            break;

        case '(': //done

            // casa uma sequencia ou outra(s)

            for (o = 1, sbuf = (unsigned char *)regex + 1;
                            sbuf != rend && o > 0; sbuf++) {

                if(*sbuf == '\\') continue;

                if(*sbuf == '(') o++;
                else if(*sbuf == ')') o--;

            }

            match = NULL;

            for (r = regex + 1; r != rend && r != sbuf; r++) {
                for (o = 0, rr = (unsigned char *)r;
                        rr != rend && *rr != '|' && *rr != ')'; rr++, o++);
                if(o == 1) continue;
                buf = (unsigned char *) hefesto_mloc(o + (rend - regex));
                for (rr = buf;
                        r != rend && *r != '|' && *r != ')';
                                            r++, rr++) *rr = *r;
                t = r;
                for (; r != rend && *r != ')'; r++);
                for (r++; r != rend; r++, rr++) *rr = *r;
                *rr = 0;
                r = t;
                for (t = text; t <= tend && !match; t++)
                    if (match_regex(t, tbegin, tend, buf, buf,
                                    buf + strlen((char *)buf), mend, 1)) {
                        match = t;
                    }
                free(buf);
            }
            break;

        case '^': //done

            if (text == tbegin) match = text;
            else match = NULL;

            if (match && recurssive) {
                if (match_regex(text, tbegin, tend,
                                next_meta_regex_symbol(regex),
                                rbegin, rend, mend, 1)) match = text;
                else match = NULL;
            }
            break;

        default: //done

            r = (regex + ( (*regex == '\\') ? 1 : 0 ) );


            if (r && (*text == *r || (*r == '.' && *regex != '\\'))) {
                match = text;
            } else {
                match = NULL;
            }

            if (match && recurssive) {
                if (match_regex(text + 1, tbegin, tend,
                                next_meta_regex_symbol(regex),
                                rbegin, rend, mend, 1)) match = text;
                else match = NULL;
            }

            break;
    }
    // provavelmente pode ser tirado esse bloco de codigo
    if (!match && text == tbegin && regex == rbegin && *regex != '^') {
        for (t = text + 1, match = NULL, o = 1; t <= tend && !match; t++, o++) {
            if (match_regex(t, tbegin, tend, regex, rbegin,
                            rend, mend, recurssive)) match = t;
        }
    }

    return match;

}

static const unsigned char *next_meta_regex_symbol(const unsigned char *regex) {

    const unsigned char *r;
    int open;

    switch (*regex) {

        case '*':
        case '?':
        case '+':

            return next_meta_regex_symbol(regex + 1);

            break;
        case '[':

            for (open = 1, r=regex + 1; open > 0; r++) {

                if (*r == '\\') continue;

                if (*r == '[') open++;
                else if (*r == ']') open--;

            }

            return r;

            break;
        case '(':

            for (open = 1, r = regex + 1; open > 0; r++) {

                if (*r == '\\') continue;

                if (*r == '(') open++;
                else if (*r == ')') open--;

            }

            return r;

            break;
        case '{':

            for (open = 1, r = regex + 1; open > 0; r++) {

                if (*r == '\\') continue;

                if (*r == '{') open++;
                else if (*r == '}') open--;

            }

            return next_meta_regex_symbol(r);

            break;
        case '\\':

            return (regex + 2);

            break;
        default:

            return (regex + 1);

            break;

    }

}

unsigned char *regex_replace(const unsigned char *regex, 
                             const unsigned char *text,
                             const size_t text_size,
                             const unsigned char *replace_text,
                             const size_t replace_text_size, size_t *osize) {

    const unsigned char *match = NULL;
    unsigned char *output = NULL;
    const unsigned char *tend = (text + text_size);
    const unsigned char *rend = (regex + strlen((char *)regex));
    const unsigned char *r, *t;
    unsigned char *o;
    size_t mend = (size_t) text;
    size_t mcount;
    size_t output_offset;

    if (text == NULL) return NULL;

    if (regex == NULL) return NULL;

    match = match_regex(text,text,tend,regex,regex,rend,&mend,1);
    *osize = 0;

    if (match) {
        mcount = 1;
        output = (unsigned char *) malloc((replace_text_size * 
                                          mcount + text_size));
        t = text;
        o = output;
        output_offset = 0;

        while (match) {
            for (; t != match; t++, o++, output_offset++) *o = *t;
            *o = 0;
            for (r = replace_text; r != (replace_text+replace_text_size);
                                                        r++, o++, t++) *o = *r;
            output_offset = o - output;
            if (t < (unsigned char *)mend) {
                for (t = (unsigned char *)mend; t < tend; t++, o++) *o = *t;
            }
            *osize = o - output;

            t = (unsigned char *)mend + 2;
            match = match_regex(t, text, tend, regex, regex, rend, &mend, 1);

            if (match) {
                mcount++;
                output = (unsigned char *) realloc(output, (replace_text_size *
                                                           mcount + text_size));
                o = output + output_offset + 1;
            }
        }

        output = (unsigned char *) realloc(output,*osize);
    } else {
        output = (unsigned char *) malloc(text_size + 1);
        memset(output, 0, text_size + 1);
        memcpy(output, text, text_size);
        *osize = text_size;
    }

    return output;
}

int bool_match_regex(const unsigned char *text, const unsigned char *tbegin,
                     const unsigned char *tend, const unsigned char *regex,
                     const unsigned char *rbegin, const unsigned char *rend,
                     const unsigned char recurssive)
{

    const unsigned char *r, *t;
    unsigned char *rr;
    int match, fmatch, cmatch;
    size_t o;
    unsigned char oc_floorb[20], oc_ceilb[20];
    int oc_floor, oc_ceil;
    unsigned char *buf, *group_end, *sub_end;

    if (regex == rend) return 1;

    switch (*regex) {

        case '[': //done

            if (*(regex + 1) != '^') {
                match = 0;
                t = text;

                for (o = 1, rr = (unsigned char *)regex + 1;
                                  rr != rend && o > 0; rr++) {
                    if (*rr == '\\') continue;

                    if (*rr == '[') o++;
                    else if (*rr == ']') o--;

                }

                for (match = 0, r = regex + 1; !match && r != rr; r++)
                    match = bool_match_regex(t, tbegin, tend,
                                             r, rbegin, rend, 0);

            }  else {
                match = 0;
                t = text;

                for (o = 1, rr = (unsigned char *)regex + 2;
                                    rr != rend && o > 0; rr++) {
                    if(*rr == '\\') continue;

                    if(*rr == '[') o++;
                    else if(*rr == ']') o--;

                }

                for (match = 1, r = regex + 2; match && r != rr; r++)
                    match = !(bool_match_regex(t, tbegin, tend,
                                               r, rbegin, rend, 0));

            }

            if (match && recurssive)
                match = bool_match_regex(t + 1, tbegin, tend,
                                         next_meta_regex_symbol(regex),
                                         rbegin, rend, 1);

            break;

        case '*': //done

            // regex+1 pode ocorrer nenhuma ou mais vezes

            match = 0;
            for (t = text, o = 1; t <= tend && !match && o; t++) {
                match = bool_match_regex(t, tbegin, tend,
                                         next_meta_regex_symbol(regex + 1),
                                         rbegin, rend, 1);
                if(!match)
                    o = bool_match_regex(t, tbegin, tend,
                                         regex + 1, rbegin, rend, 0);
            }

            // aqui nao ha reestabelecimento da recurssao pois toda regex 
            // ja tera sido combinada no primeiro statement do loop de busca 
            // anterior.

            break;

        case '?': //done

            match = 0;

            match = bool_match_regex(text, tbegin, tend,
                                     next_meta_regex_symbol(regex + 1),
                                     rbegin, rend, 1);

            if (!match) {
                if (bool_match_regex(text, tbegin, tend,
                                     regex + 1, rbegin, rend, 0))
                    match = bool_match_regex(text + 1, tbegin, tend,
                                  next_meta_regex_symbol(regex + 1),
                                                     rbegin, rend, 1);
            }

            break;

        case '$': //done

            match = (text == tend); // representa o fim de linha.

            break;

        case '+': //done

            // casa *(regex+1) uma ou mais vezes.

            t = text;
            o = 0;
            while (t <= tend && !o &&
                   (match=bool_match_regex(t, tbegin,
                                           tend, regex + 1,
                                           rbegin, rend, 0))) {
                t++;
                if (match)
                    o = bool_match_regex(t, tbegin, tend,
                                         next_meta_regex_symbol(regex + 1),
                                         rbegin, rend, 1);

                if (!o) t++;

            }

            break;

        case '{': //done

            // casa um simbolo {i,n} ou {i} ou {i,} vezes.

            for (o = 0, r = regex + 1;
                 *r != ',' && *r != '}' && r != rend; r++, o = (o + 1) % 20) {
                oc_floorb[o] = *r;
            }

            oc_floorb[o] = 0;

            o = 0;
            oc_floor = atoi((char *)oc_floorb);

            if (*r == ',') {
                for (r++; *r != '}' && r != rend; r++, o = (o + 1) % 20) {
                    oc_ceilb[o] = *r;
                }

                oc_ceilb[o] = 0;

                if (oc_ceilb[0]) 
                    oc_ceil = atoi((char *)oc_ceilb);
                else
                    oc_ceil = -2;
            } else oc_ceil = -1;

            for (o = 0, match = 1; (text + o) <= tend &&
                              match; o += (match) ? 1 : 0) {
                match = bool_match_regex(text + o, tbegin, tend,
                                         r + 1, rbegin, rend, 0);
            }

            fmatch = (oc_ceil == -1) ? (o == oc_floor) : (o >= oc_floor);

            if (fmatch && oc_ceil > -1) {
                for (o = 0, match = 1; (text + o) <= tend &&
                               match; o += (match) ? 1 : 0) {
                    match = bool_match_regex(text + o, tbegin,
                                             tend, r + 1, rbegin, rend, 0);
                }

                cmatch = (o <= oc_ceil); 
            } else cmatch = 1;

            match = (fmatch && cmatch);

            if (match && recurssive)
                match = bool_match_regex(text + o, tbegin, tend,
                                         next_meta_regex_symbol(r + 1),
                                         rbegin, rend, 1);
            break;

        case '(': //done

            // casa uma sequencia ou outra(s)

            for (o = 1, group_end = (unsigned char *)regex + 1;
                    group_end != rend && o > 0; group_end++) {

                if (*group_end == '\\') continue;

                if (*group_end == '(') o++;
                else if (*group_end == ')') o--;

            }
            match = 0;
            for(r = regex + 1; r != rend && r != group_end && match == 0; r++) {
                if (*r == '(') {
                    buf = (unsigned char *) hefesto_mloc(group_end - regex);
                    memset(buf, 0, group_end - regex);
                    o = 1;
                    rr = buf;
                    *rr = *r;
                    rr++;
                    r++;
                    for (; r != rend && o > 0; rr++, r++) {
                        *rr = *r;
                        if (*(r - 1) != '\\') {
                            if (*r == '(') o++;
                            else if (*r == ')') o--;
                        }
                    }
                    t = r;
                } else {
                    oc_ceil = 0;
                    for (o = 0, rr = (unsigned char *)r; rr != rend; rr++, o++) {
                        if (*rr == '(') oc_ceil++;
                        else if (*rr == ')') {
                            if (oc_ceil == 0) break;
                            oc_ceil--;
                        } else if (*rr == '|' && oc_ceil == 0) {
                            break;
                        } else if (*rr == '\\') rr += 1;
                    }
                    sub_end = rr;
                    buf = (unsigned char *) hefesto_mloc(o + (rend - regex));
                    for (rr = buf; r != rend && r != sub_end; r++, rr++) {
                        *rr = *r;
                    }
                    t = r;
                }
                *rr = 0;
                for (r = group_end; r != rend; r++, rr++) *rr = *r;
                *rr = 0;
                r = t;
                for (t = text; t <= tend && !match; t++)
                    match = bool_match_regex(t, tbegin, tend, buf, buf,
                                             buf + strlen((char *)buf), 1);
                    free(buf);
                }
                break;

            case '^': //done
                match = (text == tbegin);

                if (match && recurssive)
                    match = bool_match_regex(text, tbegin, tend,
                                             next_meta_regex_symbol(regex),
                                             rbegin, rend, 1);

                break;

            default: //done

                r = (regex + ( (*regex == '\\') ? 1 : 0 ) );

                match = (*text == *r || (*r == '.' && *regex != '\\'));

                if (match && recurssive)
                    match = bool_match_regex(text + 1, tbegin, tend,
                                             next_meta_regex_symbol(regex),
                                             rbegin, rend, 1);

                break;

    }

    // provavelmente pode ser tirado esse bloco de codigo
    if (!match && text == tbegin && regex == rbegin && *regex != '^') { 
        for (t = text + 1, match = 0, o = 1;t <= tend && !match; t++, o++)
        match = bool_match_regex(t, tbegin, tend, regex,
                                 rbegin, rend, recurssive);
    }

    return match;

}
