/*
// ColdMUD was created and is copyright 1993, 1994 by Greg Hudson
//
// Genesis is a derivitive work, and is copyright 1995 by Brandon Gillespie.
// Full details and copyright information can be found in the file doc/CREDITS
//
// File: web.c
// ---
//
// Some portions of this code are derived from code created by Kurt Olsen.
*/

/*
// RFC 1738:
//    
//   Many URL schemes reserve certain characters for a special meaning:
//   their appearance in the scheme-specific part of the URL has a
//   designated semantics. If the character corresponding to an octet is
//   reserved in a scheme, the octet must be encoded.  The characters ";",
//   "/", "?", ":", "@", "=" and "&" are the characters which may be
//   reserved for special meaning within a scheme. No other characters may
//   be reserved within a scheme.
//
//   [...]
//
//   Thus, only alphanumerics, the special characters "$-_.+!*'(),", and
//   reserved characters used for their reserved purposes may be used
//   unencoded within a URL.
//
//   valid ascii: 48-57 (0-9) 65-90 (A-Z) 97-122 (a-z)
*/

#include "config.h"
#include "defs.h"
#include "util.h"
#include "operators.h"
#include "execute.h"
#include "cdc_string.h"
#include "web.h"

/* valid ascii: 48-57 (0-9) 65-90 (A-Z) 97-122 (a-z) */

char * dec_2_hex[] = {
   (char) NULL, (char) NULL, (char) NULL, (char) NULL, (char) NULL,
   (char) NULL, (char) NULL, (char) NULL, (char) NULL, (char) NULL,
   (char) NULL, (char) NULL, (char) NULL, (char) NULL, (char) NULL,
   (char) NULL, (char) NULL, (char) NULL, (char) NULL, (char) NULL,
   (char) NULL, (char) NULL, (char) NULL, (char) NULL, (char) NULL,
   (char) NULL, (char) NULL, (char) NULL, (char) NULL, (char) NULL,
   (char) NULL, (char) NULL, (char) NULL,
   "%21", "%22", "%23", "%24", "%25", "%26", "%27", "%28", "%29", "%2a", "%2b",
   "%2c", "%2d", "%2e", "%2f", "%30", "%31", "%32", "%33", "%34", "%35", "%36",
   "%37", "%38", "%39", "%3a", "%3b", "%3c", "%3d", "%3e", "%3f", "%40", "%41",
   "%42", "%43", "%44", "%45", "%46", "%47", "%48", "%49", "%4a", "%4b", "%4c",
   "%4d", "%4e", "%4f", "%50", "%51", "%52", "%53", "%54", "%55", "%56", "%57",
   "%58", "%59", "%5a", "%5b", "%5c", "%5d", "%5e", "%5f", "%60", "%61", "%62",
   "%63", "%64", "%65", "%66", "%67", "%68", "%69", "%6a", "%6b", "%6c", "%6d",
   "%6e", "%6f", "%70", "%71", "%72", "%73", "%74", "%75", "%76", "%77", "%78",
   "%79", "%7a", "%7b", "%7c", "%7d", "%7e", (char) NULL
};

#define tohex(c) (dec_2_hex[(int) c])


void init_web(int argc, char ** argv) {
}

void uninit_web(void) {
}

INTERNAL char tochar(char h, char l) {
     char p;

     h = UCASE(h);
     l = UCASE(l);
     h -= '0';
     if (h > 9)
          h -= 7;
     l -= '0';
     if (l > 9)
          l -= 7;
     p = h * 16 + l;

     return p;
}

string_t * decode(string_t * str) {
    char * s = str->s,
         * n = str->s,
           h,
           l;

    for (; *s != (char) NULL; s++, n++) {
        switch (*s) {
            case '+':
                *n = ' ';
                break;
            case '%':
                h = *++s;
                l = *++s;
                *n = tochar(h, l);
                break;
            default:
                *n = *s;
        }
    }

    *n = NULL;
    str->len = strlen(str->s);

    return str;
}


string_t * encode(char * s) {
    string_t * str = string_new(strlen(s) * 2); /* this gives us a buffer
                                                   to expand into */

    for (;*s != NULL; s++) {
        if ((int) *s == 32)
            str = string_addc(str, '+');
        else if ((int) *s > 32 && (int) *s < 127) {
            if (((int) *s >= 48 && (int) *s <= 57) ||
                ((int) *s >= 65 && (int) *s <= 90) ||
                ((int) *s >= 97 && (int) *s <= 122))
                str = string_addc(str, *s);
            else
                str = string_add_chars(str, tohex(*s), 3);
        }
    }

    return str;
}

void native_decode(void) {
    data_t   * args;
    string_t * str;

    /* Accept a string to take the length of. */
    if (!func_init_1(&args, STRING))
        return;

    /* decode directly munches the string, so simply duplicate it */
    str = decode(string_from_chars(args[0].u.str->s, args[0].u.str->len));

    pop(1);
    push_string(str);
    string_discard(str);
}

void native_encode(void) {
    data_t   * args;
    string_t * str;

    /* Accept a string to take the length of. */
    if (!func_init_1(&args, STRING))
        return;

    str = encode(string_chars(args[0].u.str));

    pop(1);
    push_string(str);
    string_discard(str);
}

