// readers and printers
//
// particularly a serializer
// that retains binary sortability

#include <stdlib.h>
#include <string.h>

#include "mem.c"
#include "nantypes.c"

// print number binary sortable
dstr soprnum(dstr s, D f) {
  int l=strlen(s?s:0);
  if (f<0) {
    s= sncat(s,"-",1);

    char w[10]={0};
    int wc= 1024-(int)trunc(log10(-f));
    wctomb(w, wc);
    s= sncat(s,w,-1);

    s= sdprintf(s, "%.16g", -f);
    // "reverse"
    char* p =s+l;
    while(*p) {
      if (isdigit(*p)) *p='9'-*p+'0';
      p++;
    }
    //if (f<0) s=sncat(s,"_",1); // !

  } else if (f==L f && f<1e10) { // int
    if (f>=10) s= sdprintf(s, "_%1.0f=",trunc(log10(f)));
    s= sdprintf(s, "%.17g", f);
  } else if (f<0.1) {
    s= sncat(s,"TODO:small",-1);
  } else {
    char w[10]={0};
    int wc= 1024+(int)trunc(log10(f));
    wctomb(w, wc);

    s= sncat(s,w,-1);
    s= sdprintf(s, "%.17g", f);
  }

  s= sdprintf(s, "\t(%.8g)", f);
  return s;
}

dstr oprint(dstr s, D v) { return istyped(v)?sdprinq(s, v):soprnum(s, v);}

void p(dstr *s) {
  printf(">%s<\n",*s);
  free(*s);
  *s= sncat(0,"",-1);
}

int main(void) {
  initmem(16*1024); inittypes();

  dstr s= 0;
  p(&s); s= oprint(s, -1e300);
  p(&s); s= oprint(s, -1e100);
  p(&s); s= oprint(s, -1e18);
  p(&s); s= oprint(s, -1e17);
  p(&s); s= oprint(s, -1000000000000016);
  p(&s); s= oprint(s, -100000000000015);
  p(&s); s= oprint(s, -10000000000014);
  p(&s); s= oprint(s, -1000000000013);
  p(&s); s= oprint(s, -100000000012);
  p(&s); s= oprint(s, -0000000011);
  p(&s); s= oprint(s, -1000000010);
  p(&s); s= oprint(s, -100000009);
  p(&s); s= oprint(s, -10000008);
  p(&s); s= oprint(s, -1000000);
  p(&s); s= oprint(s, -100000);
  p(&s); s= oprint(s, -10000);
  p(&s); s= oprint(s, -9999);
  p(&s); s= oprint(s, -1000);
  p(&s); s= oprint(s, -999);
  p(&s); s= oprint(s, -100);
  p(&s); s= oprint(s, -99);
  p(&s); s= oprint(s, -9);
  p(&s); s= oprint(s, -21);
  p(&s); s= oprint(s, -20);
  p(&s); s= oprint(s, -19);
  p(&s); s= oprint(s, -12);
  p(&s); s= oprint(s, -11);
  p(&s); s= oprint(s, -10);
  p(&s); s= oprint(s, -9);
  p(&s); s= oprint(s, -2);
  p(&s); s= oprint(s, -1);

  p(&s); s= oprint(s, 0);

  p(&s); s= oprint(s, 1);
  p(&s); s= oprint(s, 2);
  p(&s); s= oprint(s, 9);
  p(&s); s= oprint(s, 10);
  p(&s); s= oprint(s, 100);
  p(&s); s= oprint(s, 10000008);
  p(&s); s= oprint(s, 100000009);
  p(&s); s= oprint(s, 1000000010);
  p(&s); s= oprint(s, 10000000011);
  p(&s); s= oprint(s, 100000000012);
  p(&s); s= oprint(s, 1000000000013);
  p(&s); s= oprint(s, 10000000000014);
  p(&s); s= oprint(s, 100000000000015);
  p(&s); s= oprint(s, 1000000000000016);
  p(&s); s= oprint(s, 1e17);
  p(&s); s= oprint(s, 1e18);
  p(&s); s= oprint(s, 1e100);
  p(&s); s= oprint(s, 1e300);
  p(&s);
}
