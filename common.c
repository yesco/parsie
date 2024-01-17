// Common misc functions
//
// most about strings/parsing
// #include directly

void spc(char**p) {while(isspace(**p))(*p)++;}

// Parse from P a name
//
// (P is pointer to char* and updated)
//
// Note: this allows any alnum || _
//
// Returns a static string (use fast!)
char* parsename(char** p) { static char s[64], i; i=0; while((isalnum(**p)
  ||**p=='_') && i<sizeof(s)-1) s[i++]=*(*p)++; s[i]= 0; return s;}

// skips a { block } returns after
//   TODO: must skip STRINGs!!! lol
char* skip(char* p){ int n=1;while(n&&*p)if(*p=='?'&&p[1]!='{')p+=2;else n+=(*p=='{')-(*p=='}'),p++;return p;}

typedef char* dstr;

// Dynamic String dstr append/cat string X N chars
//
// dstr: String pointer is resized in chunks
// of 1024 bytes. Free as normal
//
// S= NULL, or malloced destination
// X= NULL, or string to append from
// N= chars to copy, or -1=all
//
// Note: N string must be zero-terminated
// the length will be read from it.
//
//
// Returns S or a new pointer.
//  
// TODO: not safe if x not ZeroTerminated...
dstr sncat(dstr s, char* x, int n) {int i=s?strlen(s):0,l=(x||n<0)?strlen(x):n;
  if (n<0 || n>l) n= l; s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0;
  return strncpy(s+i, x?x:"", n), s;
}

// --- printers
// s=sdprinc(s,,D)   - stringify
// s=sdprinq(s,D)    - "foo" if str #atm
// s=sdprintf(s,f,D) - printf(f,D)

////////////////////////////////////////////////////////////////////////////////  
// only %lf %g %s makes sense...
dstr sdprintf(dstr s, char* f, D d) {char*x=dchars(d);
  if (x) {int n= snprintf(0,0,f,x); char q[n+1];
    snprintf(q,sizeof(q),f,x);
    s=sncat(s,q,-1);
    return s;
  } else if (iscons(d)) assert(!"TODO: sdprintf TCNS");
  int n= snprintf(0,0,f?f:"%.8g",d); char q[n+1];
  snprintf(q,sizeof(q),f?f:"%.8g",d); return sncat(s,q,-1); }

dstr sdprinc(dstr s, D d) {char*x=dchars(d);
  return x?sncat(s,x,-1):sdprintf(s,0,d); }

// quoted any " in string w \"
dstr _sdprinq(dstr s, D d) { dstr v= sdprinc(0,d),p=v;
  while(p&&*p){if(*p=='"')s=sncat(s,"\\",1);s=sncat(s,p++,1);}free(v);return s;}

// TODO: #atom
dstr sdprinq(dstr s, D d) {if (isatom(d)) s= sncat(s,"#",-1);char* p= dchars(d);
  if (isstr(d)) s= sncat(s,"c\"",-1); if (!p) s= sdprinc(s,d); else
    while(*p){ if(*p=='"')s=sncat(s,"\\",1); s=sncat(s,p++,1); }
  if (isstr(d)) s= sncat(s,"\"",-1); return  s; }
