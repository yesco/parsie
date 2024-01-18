

Z'@': T= *(D*)m(T,A,n); Z'!': *(D*)m(T,A,n)= S[-1]; S-=2;
// - stack
Z'd':S1=T;S++;Z'\\':S--;Z'o':S1=S[-1];S++;Z's':{D d=T;T=S[-1];S[-1]=d;}
// - numbers / functions call/define
//Z'A'...'Z': al(F[p[-1]-'A'],0,0,0,0); Z'^': A[1]= T; S=A+1; RET p-1;
Z':': e=strchr(p,';'); if(e) F[*p-'A']=strndup(p+1,e-p),p=e+1;
Z'[': { e=p; while(*p&&*p!=']')p++;U=newstr(e, p++-e);
  {D f=POP; D c=obj(); Obj* o= PTR(TOBJ, c); D* pars=&(o->np[0].name);
    //o->proto=OCLOS;
    // Can store 12 values/frame!
    // TODO: copy from current frame
    // For each frame UP
    // - coppy to new obj()
    // - change frameptr stack values
    // - chain up each frame as next
    // (not using proto)
    // alt: use actual names in closure!
  }
 }
// TODO: error if no method?
//Z'(':{x=S-K;U=E?E-K:0;D*s=S;p=alf(p,A,n,E,1);D n=atom(parsename(&p));
//    D m=deq(n,nil)?POP:get(s[1],n),f=funf(m),*c=fune(m);
//    P(" [FUN:");dprint(n);P(" '");dprint(m); P("' %ld] ", c?c-K:0);
//    alf(f?dchars(f):F[L m-'A'],s+1,S-s,c,0); *s=T;S=s;}
  //DEBUG(P("\n\tCALL: o="); dprint(s[1]); P(" nom="); dprint(nom); P(" m="); dprint(m); pc('\n'););
// -- numbers / math
Z'0'...'9': p--;U=reader(&p,0); // slow
Z'~': S--;T=deq(T,S1);
Z'%': S--;T=L T%L S1; Z'z': T=!T;Z'n':T=-L T;
#define OP(op,e) Z #op[0]: S--;T=T op##e S1;
OP(+,);OP(-,);OP(*,);OP(/,);OP(<,);OP(>,);OP(=,=);OP(|,|);OP(&,&);
// -- memory
Z'h': U=H-M; Z'm':x=T;T=H-M;H+=x; Z'a':H+=L POP;
Z'g': case ',': align(); if (p[-1]=='g') goto next; memcpy(H,&POP,SL); H+=SL;
//Z'@': T= *(D*)m(T,A,n); Z'!': *(D*)m(T,A,n)= S[-1]; S-=2;
// -- cX - char | string
/*
Z'c': s=0;switch(c=*p++){ Z'r':pc('\n'); Z'c':T=dlen(T); Z'=':S--;T=dcmp(T, S1);
  Z'"':case'\'':x=p[-1];e=p;while(*p&&*p!=x)p++;U=newstr(e,p++-e);
  // c?=charclass c1=first char
  // Z'?':case'1':x=T;if(isnan(T)){e=dchars(T);x=e?*e:0;};if(c=='1'){U=x;break;}
  // T= isalpha(x)?'a':isdigit(x)?'d':isspace(x)?'s':x=='_'?'_':'o';

  // TODO: need to free e???

  /// > ./alf -d <alf-printers.tst

  // but for ca% ???? errro
  //#define SP(P) do{T=newstr(e=P,-1);}while(0)

  #define SP(P) do{T=newstr(e=P,-1);free(e);}while(0)

  // TODO: buffer "type" to not create things for the GC?
  Z'a':{d=POP;s=sncat(0,dchars(POP),-1);U=d;c=*p++;case'e':case's':case'q':case'%':switch(c){
    Z'b':{char a[]={' ',0};SP(sncat(s,a,-1));} // TODO: use c fix sncat
    Z'e':{char a[]={T,0};SP(sncat(s,a,-1));} // TODO: use c fix sncat
    Z'q':SP(sdprinq(s,T)); Z'%':{e=--p;while(*p&&!isspace(*p))p++;
	char* f=strndup(e,p-e);SP(sdprintf(s,f,T));free(f);}
    case's':p++;default:p--;SP(sdprinc(s,T));}}}
      //    goto next;default:p-=2;goto error;}}}
*/
// -- printing
Z'.': dprint(POP);pc(' '); Z'e':pc(POP); Z't':P("%*s.",(int)T,M+L S[-1]);S-=2;
Z'p': dprint(POP); Z'\'': U= *p++; Z'"': while(*p&&*p!='"')pc(*p++); p++;
// -- hash/atoms/dict
Z'#': switch(*p++){ Z'a'...'z':case'A'...'Z':case'_':p--;U=atom(parsename(&p));
  Z',': S--;set(T,dlen(T),S1); Z':': S-=2;set(T,S1,S[2]);
  Z'@': S--;T=get(S1,T); Z'!': S-=3;set(S[3],S[2],S1);
  Z' ': case'\n': case 0: if ((e=dchars(T))) T=atom(e);
  Z'?': T=typ(T); Z'^': U=obj(); goto next; default: goto error; }
// -- loop/if
Z'}': RET(iff?p:0); Z'{': while(!((e=alf(p, A, n, E, 0)))){}; p= e;
// -- bitops
//#define LOP(op,e) Z#op[0]: S--; T=(L S1) op##e L T;
//Z'b': switch(*p++){ LOP(&,);LOP(|,);LOP(^,); Z'~': T= ~L T; }
// -- ` address of stuff
Z'`': switch(*p++) { Z'#': U=n; Z'0'...'9': U='0'-p[-1]-1; Z'A'...'Z':
 case'a'...'z':case'_':p--;U=atom(parsename(&p)); case' ':case 0:case'\n':
   T=atomaddr(T);break;default:goto error;}

// string/stack/misc functions
Z'$': x=1; switch(c=*p++){ Z'.':prstack(); case'n':pc('\n'); Z'd':x=S-K;U=x;
  Z'$':n=POP;A=S-n; Z'0'...'9':U=A[p[-1]-'0']; Z'h':P("%lx\n",L POP);Z'q':S=1+K;
  Z'!':A[*p++-'0']=POP; Z's':x=POP;case' ':while(x-->=0)pc(' ');
  Z'"':case'\'':e=H;while(*p&&*p!=c)*H++=*p++;*H++=0;if(*p)p++;U=e-M;U=H-e-1;
  Z'D':dump(); Z'K': prK(); goto next; default:p--;}/*err*/


