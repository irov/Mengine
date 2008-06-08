/*
================================================================================

tinypy contains tinypy code licensed in a MIT format license.  It also
contains some goodies grabbed from Python, so that license is included
as well.

================================================================================

The tinypy License

Copyright (c) 2008 Phil Hassey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

================================================================================

PYTHON SOFTWARE FOUNDATION LICENSE VERSION 2
--------------------------------------------

1. This LICENSE AGREEMENT is between the Python Software Foundation
("PSF"), and the Individual or Organization ("Licensee") accessing and
otherwise using this software ("Python") in source or binary form and
its associated documentation.

2. Subject to the terms and conditions of this License Agreement, PSF
hereby grants Licensee a nonexclusive, royalty-free, world-wide
license to reproduce, analyze, test, perform and/or display publicly,
prepare derivative works, distribute, and otherwise use Python
alone or in any derivative version, provided, however, that PSF's
License Agreement and PSF's notice of copyright, i.e., "Copyright (c)
2001, 2002, 2003, 2004, 2005, 2006, 2007 Python Software Foundation;
All Rights Reserved" are retained in Python alone or in any derivative
version prepared by Licensee.

3. In the event Licensee prepares a derivative work that is based on
or incorporates Python or any part thereof, and wants to make
the derivative work available to others as provided herein, then
Licensee hereby agrees to include in any such work a brief summary of
the changes made to Python.

4. PSF is making Python available to Licensee on an "AS IS"
basis.  PSF MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR
IMPLIED.  BY WAY OF EXAMPLE, BUT NOT LIMITATION, PSF MAKES NO AND
DISCLAIMS ANY REPRESENTATION OR WARRANTY OF MERCHANTABILITY OR FITNESS
FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF PYTHON WILL NOT
INFRINGE ANY THIRD PARTY RIGHTS.

5. PSF SHALL NOT BE LIABLE TO LICENSEE OR ANY OTHER USERS OF PYTHON
FOR ANY INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS AS
A RESULT OF MODIFYING, DISTRIBUTING, OR OTHERWISE USING PYTHON,
OR ANY DERIVATIVE THEREOF, EVEN IF ADVISED OF THE POSSIBILITY THEREOF.

6. This License Agreement will automatically terminate upon a material
breach of its terms and conditions.

7. Nothing in this License Agreement shall be deemed to create any
relationship of agency, partnership, or joint venture between PSF and
Licensee.  This License Agreement does not grant permission to use PSF
trademarks or trade name in a trademark sense to endorse or promote
products or services of Licensee, or any third party.

8. By copying, installing or otherwise using Python, Licensee
agrees to be bound by the terms and conditions of this License
Agreement.

================================================================================
*/

#ifndef TINYPY_H
#define TINYPY_H
#ifndef TP_H
#define TP_H

#include <setjmp.h>
#include <sys/stat.h>
#ifndef __USE_ISOC99
#define __USE_ISOC99
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#ifdef __GNUC__
#define tp_inline __inline__
#endif

#ifdef _MSC_VER
#define tp_inline __inline
#endif

#ifndef tp_inline
#error "Unsuported compiler"
#endif

#define tp_malloc(x) calloc((x),1)
#define tp_realloc(x,y) realloc(x,y)
#define tp_free(x) free(x)

/* #include <gc/gc.h>
   #define tp_malloc(x) GC_MALLOC(x)
   #define tp_realloc(x,y) GC_REALLOC(x,y)
   #define tp_free(x)*/

enum {
    TP_NONE,TP_NUMBER,TP_STRING,TP_DICT,
    TP_LIST,TP_FNC,TP_DATA,
};

typedef double tp_num;

typedef struct tp_number_ {
    int type;
    tp_num val;
} tp_number_;
typedef struct tp_string_ {
    int type;
    struct _tp_string *info;
    char const *val;
    int len;
} tp_string_;
typedef struct tp_list_ {
    int type;
    struct _tp_list *val;
} tp_list_;
typedef struct tp_dict_ {
    int type;
    struct _tp_dict *val;
} tp_dict_;
typedef struct tp_fnc_ {
    int type;
    struct _tp_fnc *info;
    int ftype;
    void *val;
} tp_fnc_;
typedef struct tp_data_ {
    int type;
    struct _tp_data *info;
    void *val;
    int magic;
} tp_data_;

typedef union tp_obj {
    int type;
    tp_number_ number;
    struct { int type; int *data; } gci;
    tp_string_ string;
    tp_dict_ dict;
    tp_list_ list;
    tp_fnc_ fnc;
    tp_data_ data;
} tp_obj;

typedef struct _tp_string {
    int gci;
    char s[1];
} _tp_string;
typedef struct _tp_list {
    int gci;
    tp_obj *items;
    int len;
    int alloc;
} _tp_list;
typedef struct tp_item {
    int used;
    int hash;
    tp_obj key;
    tp_obj val;
} tp_item;
typedef struct _tp_dict {
    int gci;
    tp_item *items;
    int len;
    int alloc;
    int cur;
    int mask;
    int used;
} _tp_dict;
typedef struct _tp_fnc {
    int gci;
    tp_obj self;
    tp_obj globals;
} _tp_fnc;


typedef union tp_code {
    unsigned char i;
    struct { unsigned char i,a,b,c; } regs;
    struct { char val[4]; } string;
    struct { float val; } number;
} tp_code;

typedef struct tp_frame_ {
    tp_code *codes;
    tp_code *cur;
    tp_code *jmp;
    tp_obj *regs;
    tp_obj *ret_dest;
    tp_obj fname;
    tp_obj name;
    tp_obj line;
    tp_obj globals;
    int lineno;
    int cregs;
} tp_frame_;

#define TP_GCMAX 4096
#define TP_FRAMES 256
/* #define TP_REGS_PER_FRAME 256*/
#define TP_REGS 16384
typedef struct tp_vm {
    tp_obj builtins;
    tp_obj modules;
    tp_frame_ frames[TP_FRAMES];
    tp_obj _params;
    tp_obj params;
    tp_obj _regs;
    tp_obj *regs;
    tp_obj root;
    jmp_buf buf;
    int jmp;
    tp_obj ex;
    char chars[256][2];
    int cur;
    /* gc*/
    _tp_list *white;
    _tp_list *grey;
    _tp_list *black;
    _tp_dict *strings;
    int steps;
} tp_vm;

#define TP tp_vm *tp
typedef struct _tp_data {
    int gci;
    void (*free)(TP,tp_obj);
} _tp_data;

/* NOTE: these are the few out of namespace items for convenience*/
#define tp_True tp_number(1)
#define tp_False tp_number(0)
#define TP_CSTR(v) ((tp_str(tp,(v))).string.val)

extern tp_obj tp_None;

void tp_set(TP,tp_obj,tp_obj,tp_obj);
tp_obj tp_get(TP,tp_obj,tp_obj);
tp_obj tp_len(TP,tp_obj);
tp_obj tp_str(TP,tp_obj);
int tp_cmp(TP,tp_obj,tp_obj);
void _tp_raise(TP,tp_obj);
tp_obj tp_printf(TP,char const *fmt,...);
tp_obj tp_track(TP,tp_obj);
void tp_grey(TP,tp_obj);

/* __func__ __VA_ARGS__ __FILE__ __LINE__ */
#define tp_raise(r,fmt,...) { \
    _tp_raise(tp,tp_printf(tp,fmt,__VA_ARGS__)); \
    return r; \
}
#define TP_OBJ() (tp_get(tp,tp->params,tp_None))
tp_inline static tp_obj tp_type(TP,int t,tp_obj v) {
    if (v.type != t) { tp_raise(tp_None,"_tp_type(%d,%s)",t,TP_CSTR(v)); }
    return v;
}
#define TP_TYPE(t) tp_type(tp,t,TP_OBJ())
#define TP_NUM() (TP_TYPE(TP_NUMBER).number.val)
#define TP_STR() (TP_CSTR(TP_TYPE(TP_STRING)))
#define TP_DEFAULT(d) (tp->params.list.val->len?tp_get(tp,tp->params,tp_None):(d))
#define TP_LOOP(e) \
    int __l = tp->params.list.val->len; \
    int __i; for (__i=0; __i<__l; __i++) { \
    (e) = _tp_list_get(tp,tp->params.list.val,__i,"TP_LOOP");
#define TP_END \
    }

tp_inline static int _tp_min(int a, int b) { return (a<b?a:b); }
tp_inline static int _tp_max(int a, int b) { return (a>b?a:b); }
tp_inline static int _tp_sign(tp_num v) { return (v<0?-1:(v>0?1:0)); }

tp_inline static tp_obj tp_number(tp_num v) {
    tp_obj val = {TP_NUMBER};
    val.number.val = v;
    return val;
}

tp_inline static tp_obj tp_string(char const *v) {
    tp_obj val;
    tp_string_ s = {TP_STRING, 0, v, 0};
    s.len = strlen(v);
    val.string = s;
    return val;
}

tp_inline static tp_obj tp_string_n(char const *v,int n) {
    tp_obj val;
    tp_string_ s = {TP_STRING, 0,v,n};
    val.string = s;
    return val;
}

#endif
void _tp_list_realloc(_tp_list *self,int len) ;
void _tp_list_set(TP,_tp_list *self,int k, tp_obj v, const char *error) ;
void _tp_list_free(_tp_list *self) ;
tp_obj _tp_list_get(TP,_tp_list *self,int k,const char *error) ;
void _tp_list_insertx(TP,_tp_list *self, int n, tp_obj v) ;
void _tp_list_appendx(TP,_tp_list *self, tp_obj v) ;
void _tp_list_insert(TP,_tp_list *self, int n, tp_obj v) ;
void _tp_list_append(TP,_tp_list *self, tp_obj v) ;
tp_obj _tp_list_pop(TP,_tp_list *self, int n, const char *error) ;
int _tp_list_find(TP,_tp_list *self, tp_obj v) ;
tp_obj tp_index(TP) ;
_tp_list *_tp_list_new(void) ;
tp_obj _tp_list_copy(TP, tp_obj rr) ;
tp_obj tp_append(TP) ;
tp_obj tp_pop(TP) ;
tp_obj tp_insert(TP) ;
tp_obj tp_extend(TP) ;
tp_obj tp_list(TP) ;
tp_obj tp_list_n(TP,int n,tp_obj *argv) ;
int _tp_sort_cmp(tp_obj *a,tp_obj *b) ;
tp_obj tp_sort(TP) ;
int tp_lua_hash(void const *v,int l) ;
void _tp_dict_free(_tp_dict *self) ;
int tp_hash(TP,tp_obj v) ;
void _tp_dict_hash_set(TP,_tp_dict *self, int hash, tp_obj k, tp_obj v) ;
void _tp_dict_tp_realloc(TP,_tp_dict *self,int len) ;
int _tp_dict_hash_find(TP,_tp_dict *self, int hash, tp_obj k) ;
int _tp_dict_find(TP,_tp_dict *self,tp_obj k) ;
void _tp_dict_setx(TP,_tp_dict *self,tp_obj k, tp_obj v) ;
void _tp_dict_set(TP,_tp_dict *self,tp_obj k, tp_obj v) ;
tp_obj _tp_dict_get(TP,_tp_dict *self,tp_obj k, const char *error) ;
void _tp_dict_del(TP,_tp_dict *self,tp_obj k, const char *error) ;
_tp_dict *_tp_dict_new(void) ;
tp_obj _tp_dict_copy(TP,tp_obj rr) ;
int _tp_dict_next(TP,_tp_dict *self) ;
tp_obj tp_merge(TP) ;
tp_obj tp_dict(TP) ;
tp_obj tp_dict_n(TP,int n, tp_obj* argv) ;
tp_obj *tp_ptr(tp_obj o) ;
tp_obj _tp_dcall(TP,tp_obj fnc(TP)) ;
tp_obj _tp_tcall(TP,tp_obj fnc) ;
tp_obj tp_fnc_new(TP,int t, void *v, tp_obj s, tp_obj g) ;
tp_obj tp_def(TP,void *v, tp_obj g) ;
tp_obj tp_fnc(TP,tp_obj v(TP)) ;
tp_obj tp_method(TP,tp_obj self,tp_obj v(TP)) ;
tp_obj tp_data(TP,int magic,void *v) ;
tp_obj tp_params(TP) ;
tp_obj tp_params_n(TP,int n, tp_obj argv[]) ;
tp_obj tp_params_v(TP,int n,...) ;
tp_obj tp_string_t(TP, int n) ;
int _tp_str_index(tp_obj s, tp_obj k) ;
tp_obj tp_join(TP) ;
tp_obj tp_string_slice(TP,tp_obj s, int a, int b) ;
tp_obj tp_split(TP) ;
tp_obj tp_find(TP) ;
tp_obj tp_str_index(TP) ;
tp_obj tp_str2(TP) ;
tp_obj tp_chr(TP) ;
tp_obj tp_ord(TP) ;
tp_obj tp_strip(TP) ;
tp_obj tp_replace(TP) ;
tp_obj tp_print(TP) ;
tp_obj tp_bind(TP) ;
tp_obj tp_min(TP) ;
tp_obj tp_max(TP) ;
tp_obj tp_copy(TP) ;
tp_obj tp_len_(TP) ;
tp_obj tp_assert(TP) ;
tp_obj tp_range(TP) ;
tp_obj tp_system(TP) ;
tp_obj tp_istype(TP) ;
tp_obj tp_float(TP) ;
tp_obj tp_save(TP) ;
tp_obj tp_load(TP) ;
tp_obj tp_fpack(TP) ;
tp_obj tp_abs(TP) ;
tp_obj tp_int(TP) ;
tp_num _roundf(tp_num v) ;
tp_obj tp_round(TP) ;
tp_obj tp_exists(TP) ;
tp_obj tp_mtime(TP) ;
void tp_follow(TP,tp_obj v) ;
void tp_reset(TP) ;
void tp_gc_init(TP) ;
void tp_gc_deinit(TP) ;
void tp_delete(TP,tp_obj v) ;
void tp_collect(TP) ;
void _tp_gcinc(TP) ;
void tp_full(TP) ;
void tp_gcinc(TP) ;
int tp_bool(TP,tp_obj v) ;
tp_obj tp_has(TP,tp_obj self, tp_obj k) ;
void tp_del(TP,tp_obj self, tp_obj k) ;
tp_obj tp_iter(TP,tp_obj self, tp_obj k) ;
int tp_iget(TP,tp_obj *r, tp_obj self, tp_obj k) ;
tp_obj tp_add(TP,tp_obj a, tp_obj b) ;
tp_obj tp_mul(TP,tp_obj a, tp_obj b) ;
tp_vm *_tp_init(void) ;
void tp_deinit(TP) ;
void tp_frame(TP,tp_obj globals,tp_code *codes,tp_obj *ret_dest) ;
void tp_print_stack(TP) ;
void tp_handle(TP) ;
void _tp_call(TP,tp_obj *dest, tp_obj fnc, tp_obj params) ;
void tp_return(TP, tp_obj v) ;
int tp_step(TP) ;
void tp_run(TP,int cur) ;
tp_obj tp_call(TP, const char *mod, const char *fnc, tp_obj params) ;
tp_obj tp_import(TP, char const *fname, char const *name, void *codes) ;
tp_obj tp_exec_(TP) ;
tp_obj tp_import_(TP) ;
void tp_builtins(TP) ;
void tp_args(TP,int argc, char *argv[]) ;
tp_obj tp_main(TP,char *fname, void *code) ;
tp_obj tp_compile(TP, tp_obj text, tp_obj fname) ;
tp_obj tp_exec(TP,tp_obj code, tp_obj globals) ;
tp_obj tp_eval(TP, char *text, tp_obj globals) ;
tp_vm *tp_init(int argc, char *argv[]) ;
void tp_compiler(TP) ;
#endif

extern unsigned char tp_tokenize[];
extern unsigned char tp_parse[];
extern unsigned char tp_encode[];
extern unsigned char tp_py2bc[];
void _tp_list_realloc(_tp_list *self,int len) {
    if (!len) { len=1; }
    self->items = (tp_obj*)tp_realloc(self->items,len*sizeof(tp_obj));
    self->alloc = len;
}

void _tp_list_set(TP,_tp_list *self,int k, tp_obj v, const char *error) {
    if (k >= self->len) { tp_raise(,"%s: KeyError: %d\n",error,k); }
    self->items[k] = v;
    tp_grey(tp,v);
}
void _tp_list_free(_tp_list *self) {
    tp_free(self->items);
    tp_free(self);
}

tp_obj _tp_list_get(TP,_tp_list *self,int k,const char *error) {
    if (k >= self->len) { tp_raise(tp_None,"%s: KeyError: %d\n",error,k); }
    return self->items[k];
}
void _tp_list_insertx(TP,_tp_list *self, int n, tp_obj v) {
    if (self->len >= self->alloc) {
        _tp_list_realloc(self,self->alloc*2);
    }
    if (n < self->len) { memmove(&self->items[n+1],&self->items[n],sizeof(tp_obj)*(self->len-n)); }
    self->items[n] = v;
    self->len += 1;
}
void _tp_list_appendx(TP,_tp_list *self, tp_obj v) {
    _tp_list_insertx(tp,self,self->len,v);
}
void _tp_list_insert(TP,_tp_list *self, int n, tp_obj v) {
    _tp_list_insertx(tp,self,n,v);
    tp_grey(tp,v);
}
void _tp_list_append(TP,_tp_list *self, tp_obj v) {
    _tp_list_insert(tp,self,self->len,v);
}
tp_obj _tp_list_pop(TP,_tp_list *self, int n, const char *error) {
    tp_obj r = _tp_list_get(tp,self,n,error);
    if (n != self->len-1) { memmove(&self->items[n],&self->items[n+1],sizeof(tp_obj)*(self->len-(n+1))); }
    self->len -= 1;
    return r;
}

int _tp_list_find(TP,_tp_list *self, tp_obj v) {
    int n;
    for (n=0; n<self->len; n++) {
        if (tp_cmp(tp,v,self->items[n]) == 0) {
            return n;
        }
    }
    return -1;
}

tp_obj tp_index(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    int i = _tp_list_find(tp,self.list.val,v);
    if (i < 0) { tp_raise(tp_None,"tp_index(%s,%s) - item not found",TP_CSTR(self),TP_CSTR(v)); }
    return tp_number(i);
}

_tp_list *_tp_list_new(void) {
    return (_tp_list*)tp_malloc(sizeof(_tp_list));
}

tp_obj _tp_list_copy(TP, tp_obj rr) {
    tp_obj val = {TP_LIST};
    _tp_list *o = rr.list.val;
    _tp_list *r = _tp_list_new();
    *r = *o; r->gci = 0;
    r->items = (tp_obj*)tp_malloc(sizeof(tp_obj)*o->alloc);
    memcpy(r->items,o->items,sizeof(tp_obj)*o->alloc);
    val.list.val = r;
    return tp_track(tp,val);
}

tp_obj tp_append(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    _tp_list_append(tp,self.list.val,v);
    return tp_None;
}

tp_obj tp_pop(TP) {
    tp_obj self = TP_OBJ();
    return _tp_list_pop(tp,self.list.val,self.list.val->len-1,"pop");
}

tp_obj tp_insert(TP) {
    tp_obj self = TP_OBJ();
    int n = TP_NUM();
    tp_obj v = TP_OBJ();
    _tp_list_insert(tp,self.list.val,n,v);
    return tp_None;
}

tp_obj tp_extend(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    int i;
    for (i=0; i<v.list.val->len; i++) {
        _tp_list_append(tp,self.list.val,v.list.val->items[i]);
    }
    return tp_None;
}

tp_obj tp_list(TP) {
    tp_obj r = {TP_LIST};
    r.list.val = _tp_list_new();
    return tp ? tp_track(tp,r) : r;
}

tp_obj tp_list_n(TP,int n,tp_obj *argv) {
    int i;
    tp_obj r = tp_list(tp); _tp_list_realloc(r.list.val,n);
    for (i=0; i<n; i++) {
        _tp_list_append(tp,r.list.val,argv[i]);
    }
    return r;
}

int _tp_sort_cmp(tp_obj *a,tp_obj *b) {
    return tp_cmp(0,*a,*b);
}

tp_obj tp_sort(TP) {
    tp_obj self = TP_OBJ();
    qsort(self.list.val->items, self.list.val->len, sizeof(tp_obj), (int(*)(const void*,const void*))_tp_sort_cmp);
    return tp_None;
}

int tp_lua_hash(void const *v,int l) {
    int i,step = (l>>5)+1;
    int h = l + (l >= 4?*(int*)v:0);
    for (i=l; i>=step; i-=step) {
        h = h^((h<<5)+(h>>2)+((unsigned char *)v)[i-1]);
    }
    return h;
}
void _tp_dict_free(_tp_dict *self) {
    tp_free(self->items);
    tp_free(self);
}

/* void _tp_dict_reset(_tp_dict *self) {
       memset(self->items,0,self->alloc*sizeof(tp_item));
       self->len = 0;
       self->used = 0;
       self->cur = 0;
   }*/

int tp_hash(TP,tp_obj v) {
    switch (v.type) {
        case TP_NONE: return 0;
        case TP_NUMBER: return tp_lua_hash(&v.number.val,sizeof(tp_num));
        case TP_STRING: return tp_lua_hash(v.string.val,v.string.len);
        case TP_DICT: return tp_lua_hash(&v.dict.val,sizeof(void*));
        case TP_LIST: {
            int r = v.list.val->len; int n; for(n=0; n<v.list.val->len; n++) {
            tp_obj vv = v.list.val->items[n]; r += vv.type != TP_LIST?tp_hash(tp,v.list.val->items[n]):tp_lua_hash(&vv.list.val,sizeof(void*)); } return r;
        }
        case TP_FNC: return tp_lua_hash(&v.fnc.info,sizeof(void*));
        case TP_DATA: return tp_lua_hash(&v.data.val,sizeof(void*));
    }
    tp_raise(0,"tp_hash(%s)",TP_CSTR(v));
}

void _tp_dict_hash_set(TP,_tp_dict *self, int hash, tp_obj k, tp_obj v) {
    tp_item item;
    int i,idx = hash&self->mask;
    for (i=idx; i<idx+self->alloc; i++) {
        int n = i&self->mask;
        if (self->items[n].used > 0) { continue; }
        if (self->items[n].used == 0) { self->used += 1; }
        item.used = 1;
        item.hash = hash;
        item.key = k;
        item.val = v;
        self->items[n] = item;
        self->len += 1;
        return;
    }
    tp_raise(,"_tp_dict_hash_set(%d,%d,%s,%s)",self,hash,TP_CSTR(k),TP_CSTR(v));
}

void _tp_dict_tp_realloc(TP,_tp_dict *self,int len) {
    tp_item *items = self->items;
    int i,alloc = self->alloc;
    len = _tp_max(8,len);

    self->items = (tp_item*)tp_malloc(len*sizeof(tp_item));
    self->alloc = len; self->mask = len-1;
    self->len = 0; self->used = 0;

    for (i=0; i<alloc; i++) {
        if (items[i].used != 1) { continue; }
        _tp_dict_hash_set(tp,self,items[i].hash,items[i].key,items[i].val);
    }
    tp_free(items);
}

int _tp_dict_hash_find(TP,_tp_dict *self, int hash, tp_obj k) {
    int i,idx = hash&self->mask;
    for (i=idx; i<idx+self->alloc; i++) {
        int n = i&self->mask;
        if (self->items[n].used == 0) { break; }
        if (self->items[n].used < 0) { continue; }
        if (self->items[n].hash != hash) { continue; }
        if (tp_cmp(tp,self->items[n].key,k) != 0) { continue; }
        return n;
    }
    return -1;
}
int _tp_dict_find(TP,_tp_dict *self,tp_obj k) {
    return _tp_dict_hash_find(tp,self,tp_hash(tp,k),k);
}

void _tp_dict_setx(TP,_tp_dict *self,tp_obj k, tp_obj v) {
    int hash = tp_hash(tp,k); int n = _tp_dict_hash_find(tp,self,hash,k);
    if (n == -1) {
        if (self->len >= (self->alloc/2)) {
            _tp_dict_tp_realloc(tp,self,self->alloc*2);
        } else if (self->used >= (self->alloc*3/4)) {
            _tp_dict_tp_realloc(tp,self,self->alloc);
        }
        _tp_dict_hash_set(tp,self,hash,k,v);
    } else {
        self->items[n].val = v;
    }
}

void _tp_dict_set(TP,_tp_dict *self,tp_obj k, tp_obj v) {
    _tp_dict_setx(tp,self,k,v);
    tp_grey(tp,k); tp_grey(tp,v);
}

tp_obj _tp_dict_get(TP,_tp_dict *self,tp_obj k, const char *error) {
    int n = _tp_dict_find(tp,self,k);
    if (n < 0) {
        tp_raise(tp_None,"%s: KeyError: %s\n",error,TP_CSTR(k));
    }
    return self->items[n].val;
}

void _tp_dict_del(TP,_tp_dict *self,tp_obj k, const char *error) {
    int n = _tp_dict_find(tp,self,k);
    if (n < 0) { tp_raise(,"%s: KeyError: %s\n",error,TP_CSTR(k)); }
    self->items[n].used = -1;
    self->len -= 1;
}

_tp_dict *_tp_dict_new(void) {
    _tp_dict *self = (_tp_dict*)tp_malloc(sizeof(_tp_dict));
    return self;
}
tp_obj _tp_dict_copy(TP,tp_obj rr) {
    tp_obj obj = {TP_DICT};
    _tp_dict *o = rr.dict.val;
    _tp_dict *r = _tp_dict_new();
    *r = *o; r->gci = 0;
    r->items = (tp_item*)tp_malloc(sizeof(tp_item)*o->alloc);
    memcpy(r->items,o->items,sizeof(tp_item)*o->alloc);
    obj.dict.val = r;
    return tp_track(tp,obj);
}

int _tp_dict_next(TP,_tp_dict *self) {
    if (!self->len) { tp_raise(0,"_tp_dict_next(...)",0); }
    while (1) {
        self->cur = ((self->cur + 1) & self->mask);
        if (self->items[self->cur].used > 0) {
            return self->cur;
        }
    }
}

tp_obj tp_merge(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    int i; for (i=0; i<v.dict.val->len; i++) {
        int n = _tp_dict_next(tp,v.dict.val);
        _tp_dict_set(tp,self.dict.val,
            v.dict.val->items[n].key,v.dict.val->items[n].val);
    }
    return tp_None;
}

tp_obj tp_dict(TP) {
    tp_obj r = {TP_DICT};
    r.dict.val = _tp_dict_new();
    return tp ? tp_track(tp,r) : r;
}

tp_obj tp_dict_n(TP,int n, tp_obj* argv) {
    tp_obj r = tp_dict(tp);
    int i; for (i=0; i<n; i++) { tp_set(tp,r,argv[i*2],argv[i*2+1]); }
    return r;
}


tp_obj *tp_ptr(tp_obj o) {
    tp_obj *ptr = (tp_obj*)tp_malloc(sizeof(tp_obj)); *ptr = o;
    return ptr;
}

tp_obj _tp_dcall(TP,tp_obj fnc(TP)) {
    return fnc(tp);
}
tp_obj _tp_tcall(TP,tp_obj fnc) {
    if (fnc.fnc.ftype&2) {
        _tp_list_insert(tp,tp->params.list.val,0,fnc.fnc.info->self);
    }
    return _tp_dcall(tp,(tp_obj (*)(tp_vm *))fnc.fnc.val);
}

tp_obj tp_fnc_new(TP,int t, void *v, tp_obj s, tp_obj g) {
    tp_obj r = {TP_FNC};
    _tp_fnc *info = (_tp_fnc*)tp_malloc(sizeof(_tp_fnc));
    info->self = s;
    info->globals = g;
    r.fnc.ftype = t;
    r.fnc.info = info;
    r.fnc.val = v;
    return tp_track(tp,r);
}

tp_obj tp_def(TP,void *v, tp_obj g) {
    return tp_fnc_new(tp,1,v,tp_None,g);
}

tp_obj tp_fnc(TP,tp_obj v(TP)) {
    return tp_fnc_new(tp,0,v,tp_None,tp_None);
}

tp_obj tp_method(TP,tp_obj self,tp_obj v(TP)) {
    return tp_fnc_new(tp,2,v,self,tp_None);
}

tp_obj tp_data(TP,int magic,void *v) {
    tp_obj r = {TP_DATA};
    r.data.info = (_tp_data*)tp_malloc(sizeof(_tp_data));
    r.data.val = v;
    r.data.magic = magic;
    return tp_track(tp,r);
}

tp_obj tp_params(TP) {
    tp_obj r;
    tp->params = tp->_params.list.val->items[tp->cur];
    r = tp->_params.list.val->items[tp->cur];
    r.list.val->len = 0;
    return r;
}
tp_obj tp_params_n(TP,int n, tp_obj argv[]) {
    tp_obj r = tp_params(tp);
    int i; for (i=0; i<n; i++) { _tp_list_append(tp,r.list.val,argv[i]); }
    return r;
}
tp_obj tp_params_v(TP,int n,...) {
    int i;
    tp_obj r = tp_params(tp);
    va_list a; va_start(a,n);
    for (i=0; i<n; i++) {
        _tp_list_append(tp,r.list.val,va_arg(a,tp_obj));
    }
    va_end(a);
    return r;
}
tp_obj tp_string_t(TP, int n) {
    tp_obj r = tp_string_n(0,n);
    r.string.info = (_tp_string*)tp_malloc(sizeof(_tp_string)+n);
    r.string.val = r.string.info->s;
    return r;
}

tp_obj tp_printf(TP, char const *fmt,...) {
    int l;
    tp_obj r;
    char *s;
    va_list arg;
    va_start(arg, fmt);
    l = vsnprintf(NULL, 0, fmt,arg);
    r = tp_string_t(tp,l);
    s = r.string.info->s;
    va_end(arg);
    va_start(arg, fmt);
    vsprintf(s,fmt,arg);
    va_end(arg);
    return tp_track(tp,r);
}

int _tp_str_index(tp_obj s, tp_obj k) {
    int i=0;
    while ((s.string.len - i) >= k.string.len) {
        if (memcmp(s.string.val+i,k.string.val,k.string.len) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

tp_obj tp_join(TP) {
    tp_obj delim = TP_OBJ();
    tp_obj val = TP_OBJ();
    int l=0,i;
    tp_obj r;
    char *s;
    for (i=0; i<val.list.val->len; i++) {
        if (i!=0) { l += delim.string.len; }
        l += tp_str(tp,val.list.val->items[i]).string.len;
    }
    r = tp_string_t(tp,l);
    s = r.string.info->s;
    l = 0;
    for (i=0; i<val.list.val->len; i++) {
        tp_obj e;
        if (i!=0) {
            memcpy(s+l,delim.string.val,delim.string.len); l += delim.string.len;
        }
        e = tp_str(tp,val.list.val->items[i]);
        memcpy(s+l,e.string.val,e.string.len); l += e.string.len;
    }
    return tp_track(tp,r);
}

tp_obj tp_string_slice(TP,tp_obj s, int a, int b) {
    tp_obj r = tp_string_t(tp,b-a);
    char *m = r.string.info->s;
    memcpy(m,s.string.val+a,b-a);
    return tp_track(tp,r);
}

tp_obj tp_split(TP) {
    tp_obj v = TP_OBJ();
    tp_obj d = TP_OBJ();
    tp_obj r = tp_list(tp);

    int i;
    while ((i=_tp_str_index(v,d))!=-1) {
        _tp_list_append(tp,r.list.val,tp_string_slice(tp,v,0,i));
        v.string.val += i + d.string.len; v.string.len -= i + d.string.len;
/*         tp_grey(tp,r); // should stop gc or something instead*/
    }
    _tp_list_append(tp,r.list.val,tp_string_slice(tp,v,0,v.string.len));
/*     tp_grey(tp,r); // should stop gc or something instead*/
    return r;
}


tp_obj tp_find(TP) {
    tp_obj s = TP_OBJ();
    tp_obj v = TP_OBJ();
    return tp_number(_tp_str_index(s,v));
}

tp_obj tp_str_index(TP) {
    tp_obj s = TP_OBJ();
    tp_obj v = TP_OBJ();
    int n = _tp_str_index(s,v);
    if (n >= 0) { return tp_number(n); }
    tp_raise(tp_None,"tp_str_index(%s,%s)",s,v);
}

tp_obj tp_str2(TP) {
    tp_obj v = TP_OBJ();
    return tp_str(tp,v);
}

tp_obj tp_chr(TP) {
    int v = TP_NUM();
    return tp_string_n(tp->chars[(unsigned char)v],1);
}
tp_obj tp_ord(TP) {
    char const *s = TP_STR();
    return tp_number((unsigned char)s[0]);
}

tp_obj tp_strip(TP) {
    char const *v = TP_STR();
    int i, l = strlen(v); int a = l, b = 0;
    tp_obj r;
    char *s;
    for (i=0; i<l; i++) {
        if (v[i] != ' ' && v[i] != '\n' && v[i] != '\t' && v[i] != '\r') {
            a = _tp_min(a,i); b = _tp_max(b,i+1);
        }
    }
    if ((b-a) < 0) { return tp_string(""); }
    r = tp_string_t(tp,b-a);
    s = r.string.info->s;
    memcpy(s,v+a,b-a);
    return tp_track(tp,r);
}


tp_obj tp_replace(TP) {
    tp_obj s = TP_OBJ();
    tp_obj k = TP_OBJ();
    tp_obj v = TP_OBJ();
    tp_obj p = s;
    int i,n = 0;
    int c;
    int l;
    tp_obj rr;
    char *r;
    char *d;
    tp_obj z;
    while ((i = _tp_str_index(p,k)) != -1) {
        n += 1;
        p.string.val += i + k.string.len; p.string.len -= i + k.string.len;
    }
/*     fprintf(stderr,"ns: %d\n",n); */
    l = s.string.len + n * (v.string.len-k.string.len);
    rr = tp_string_t(tp,l);
    r = rr.string.info->s;
    d = r;
    z = p = s;
    while ((i = _tp_str_index(p,k)) != -1) {
        p.string.val += i; p.string.len -= i;
        memcpy(d,z.string.val,c=(p.string.val-z.string.val)); d += c;
        p.string.val += k.string.len; p.string.len -= k.string.len;
        memcpy(d,v.string.val,v.string.len); d += v.string.len;
        z = p;
    }
    memcpy(d,z.string.val,(s.string.val + s.string.len) - z.string.val);

    return tp_track(tp,rr);
}

tp_obj tp_print(TP) {
    int n = 0;
    tp_obj e;
    TP_LOOP(e)
        if (n) { printf(" "); }
        printf("%s",TP_CSTR(e));
        n += 1;
    TP_END;
    printf("\n");
    return tp_None;
}

tp_obj tp_bind(TP) {
    tp_obj r = TP_OBJ();
    tp_obj self = TP_OBJ();
    return tp_fnc_new(tp,r.fnc.ftype|2,r.fnc.val,self,r.fnc.info->globals);
}

tp_obj tp_min(TP) {
    tp_obj r = TP_OBJ();
    tp_obj e;
    TP_LOOP(e)
        if (tp_cmp(tp,r,e) > 0) { r = e; }
    TP_END;
    return r;
}

tp_obj tp_max(TP) {
    tp_obj r = TP_OBJ();
    tp_obj e;
    TP_LOOP(e)
        if (tp_cmp(tp,r,e) < 0) { r = e; }
    TP_END;
    return r;
}

tp_obj tp_copy(TP) {
    tp_obj r = TP_OBJ();
    int type = r.type;
    if (type == TP_LIST) {
        return _tp_list_copy(tp,r);
    } else if (type == TP_DICT) {
        return _tp_dict_copy(tp,r);
    }
    tp_raise(tp_None,"tp_copy(%s)",TP_CSTR(r));
}


tp_obj tp_len_(TP) {
    tp_obj e = TP_OBJ();
    return tp_len(tp,e);
}


tp_obj tp_assert(TP) {
    int a = TP_NUM();
    if (a) { return tp_None; }
    tp_raise(tp_None,"%s","assert failed");
}

tp_obj tp_range(TP) {
    int a,b,c,i;
    tp_obj r = tp_list(tp);
    switch (tp->params.list.val->len) {
        case 1: a = 0; b = TP_NUM(); c = 1; break;
        case 2:
        case 3: a = TP_NUM(); b = TP_NUM(); c = TP_DEFAULT(tp_number(1)).number.val; break;
        default: return r;
    }
    if (c != 0) {
        for (i=a; (c>0) ? i<b : i>b; i+=c) {
            _tp_list_append(tp,r.list.val,tp_number(i));
        }
    }
    return r;
}


tp_obj tp_system(TP) {
    char const *s = TP_STR();
    int r = system(s);
    return tp_number(r);
}

tp_obj tp_istype(TP) {
    tp_obj v = TP_OBJ();
    char const *t = TP_STR();
    if (strcmp("string",t) == 0) { return tp_number(v.type == TP_STRING); }
    if (strcmp("list",t) == 0) { return tp_number(v.type == TP_LIST); }
    if (strcmp("dict",t) == 0) { return tp_number(v.type == TP_DICT); }
    if (strcmp("number",t) == 0) { return tp_number(v.type == TP_NUMBER); }
    tp_raise(tp_None,"is_type(%s,%s)",TP_CSTR(v),t);
}


tp_obj tp_float(TP) {
    tp_obj v = TP_OBJ();
    int ord = TP_DEFAULT(tp_number(0)).number.val;
    int type = v.type;
    if (type == TP_NUMBER) { return v; }
    if (type == TP_STRING) {
        if (strchr(TP_CSTR(v),'.')) { return tp_number(atof(TP_CSTR(v))); }
        return(tp_number(strtol(TP_CSTR(v),0,ord)));
    }
    tp_raise(tp_None,"tp_float(%s)",TP_CSTR(v));
}


tp_obj tp_save(TP) {
    char const *fname = TP_STR();
    tp_obj v = TP_OBJ();
    FILE *f;
    f = fopen(fname,"wb");
    if (!f) { tp_raise(tp_None,"tp_save(%s,...)",fname); }
    fwrite(v.string.val,v.string.len,1,f);
    fclose(f);
    return tp_None;
}

tp_obj tp_load(TP) {
    FILE *f;
    long l;
    tp_obj r;
    char *s;
    char const *fname = TP_STR();
    struct stat stbuf;
    stat(fname, &stbuf);
    l = stbuf.st_size;
    f = fopen(fname,"rb");
    if (!f) {
        tp_raise(tp_None,"tp_load(%s)",fname);
    }
    r = tp_string_t(tp,l);
    s = r.string.info->s;
    fread(s,1,l,f);
    fclose(f);
    return tp_track(tp,r);
}


tp_obj tp_fpack(TP) {
    tp_num v = TP_NUM();
    tp_obj r = tp_string_t(tp,sizeof(tp_num));
    *(tp_num*)r.string.val = v;
    return tp_track(tp,r);
}

tp_obj tp_abs(TP) {
    return tp_number(fabs(tp_float(tp).number.val));
}
tp_obj tp_int(TP) {
    return tp_number((long)tp_float(tp).number.val);
}
tp_num _roundf(tp_num v) {
    tp_num av = fabs(v); tp_num iv = (long)av;
    av = (av-iv < 0.5?iv:iv+1);
    return (v<0?-av:av);
}
tp_obj tp_round(TP) {
    return tp_number(_roundf(tp_float(tp).number.val));
}

tp_obj tp_exists(TP) {
    char const *s = TP_STR();
    struct stat stbuf;
    return tp_number(!stat(s,&stbuf));
}
tp_obj tp_mtime(TP) {
    char const *s = TP_STR();
    struct stat stbuf;
    if (!stat(s,&stbuf)) { return tp_number(stbuf.st_mtime); }
    tp_raise(tp_None,"tp_mtime(%s)",s);
}
/* tp_obj tp_track(TP,tp_obj v) { return v; }
   void tp_grey(TP,tp_obj v) { }
   void tp_full(TP) { }
   void tp_gc_init(TP) { }
   void tp_gc_deinit(TP) { }
   void tp_delete(TP,tp_obj v) { }*/

void tp_grey(TP,tp_obj v) {
    if (v.type < TP_STRING || (!v.gci.data) || *v.gci.data) { return; }
    *v.gci.data = 1;
    if (v.type == TP_STRING || v.type == TP_DATA) {
        _tp_list_appendx(tp,tp->black,v);
        return;
    }
    _tp_list_appendx(tp,tp->grey,v);
}

void tp_follow(TP,tp_obj v) {
    int type = v.type;
    if (type == TP_LIST) {
        int n;
        for (n=0; n<v.list.val->len; n++) {
            tp_grey(tp,v.list.val->items[n]);
        }
    }
    if (type == TP_DICT) {
        int i;
        for (i=0; i<v.dict.val->len; i++) {
            int n = _tp_dict_next(tp,v.dict.val);
            tp_grey(tp,v.dict.val->items[n].key);
            tp_grey(tp,v.dict.val->items[n].val);
        }
    }
    if (type == TP_FNC) {
        tp_grey(tp,v.fnc.info->self);
        tp_grey(tp,v.fnc.info->globals);
    }
}

void tp_reset(TP) {
    int n;
    _tp_list *tmp;
    for (n=0; n<tp->black->len; n++) {
        *tp->black->items[n].gci.data = 0;
    }
    tmp = tp->white;
    tp->white = tp->black;
    tp->black = tmp;
}

void tp_gc_init(TP) {
    tp->white = _tp_list_new();
    tp->strings = _tp_dict_new();
    tp->grey = _tp_list_new();
    tp->black = _tp_list_new();
    tp->steps = 0;
}

void tp_gc_deinit(TP) {
    _tp_list_free(tp->white);
    _tp_dict_free(tp->strings);
    _tp_list_free(tp->grey);
    _tp_list_free(tp->black);
}

void tp_delete(TP,tp_obj v) {
    int type = v.type;
    if (type == TP_LIST) {
        _tp_list_free(v.list.val);
        return;
    } else if (type == TP_DICT) {
        _tp_dict_free(v.dict.val);
        return;
    } else if (type == TP_STRING) {
        tp_free(v.string.info);
        return;
    } else if (type == TP_DATA) {
        if (v.data.info->free) {
            v.data.info->free(tp,v);
        }
        tp_free(v.data.info);
        return;
    } else if (type == TP_FNC) {
        tp_free(v.fnc.info);
        return;
    }
    tp_raise(,"tp_delete(%s)",TP_CSTR(v));
}

void tp_collect(TP) {
    int n;
    for (n=0; n<tp->white->len; n++) {
        tp_obj r = tp->white->items[n];
        if (*r.gci.data) { continue; }
        if (r.type == TP_STRING) {
            /*this can't be moved into tp_delete, because tp_delete is
               also used by tp_track_s to delete redundant strings*/
            _tp_dict_del(tp,tp->strings,r,"tp_collect");
        }
        tp_delete(tp,r);
    }
    tp->white->len = 0;
    tp_reset(tp);
}

void _tp_gcinc(TP) {
    tp_obj v;
    if (!tp->grey->len) {
        return;
    }
    v = _tp_list_pop(tp,tp->grey,tp->grey->len-1,"_tp_gcinc");
    tp_follow(tp,v);
    _tp_list_appendx(tp,tp->black,v);
}

void tp_full(TP) {
    while (tp->grey->len) {
        _tp_gcinc(tp);
    }
    tp_collect(tp);
    tp_follow(tp,tp->root);
}

void tp_gcinc(TP) {
    tp->steps += 1;
    if (tp->steps < TP_GCMAX || tp->grey->len > 0) {
        _tp_gcinc(tp); _tp_gcinc(tp);
    }
    if (tp->steps < TP_GCMAX || tp->grey->len > 0) { return; }
    tp->steps = 0;
    tp_full(tp);
    return;
}

tp_obj tp_track(TP,tp_obj v) {
    if (v.type == TP_STRING) {
        int i = _tp_dict_find(tp,tp->strings,v);
        if (i != -1) {
            tp_delete(tp,v);
            v = tp->strings->items[i].key;
            tp_grey(tp,v);
            return v;
        }
        _tp_dict_setx(tp,tp->strings,v,tp_True);
    }
    tp_gcinc(tp);
    tp_grey(tp,v);
    return v;
}

/**/


tp_obj tp_str(TP,tp_obj self) {
    int type = self.type;
    if (type == TP_STRING) { return self; }
    if (type == TP_NUMBER) {
        tp_num v = self.number.val;
        if ((fabs(v)-fabs((long)v)) < 0.000001) { return tp_printf(tp,"%ld",(long)v); }
        return tp_printf(tp,"%f",v);
    } else if(type == TP_DICT) {
        return tp_printf(tp,"<dict 0x%x>",self.dict.val);
    } else if(type == TP_LIST) {
        return tp_printf(tp,"<list 0x%x>",self.list.val);
    } else if (type == TP_NONE) {
        return tp_string("None");
    } else if (type == TP_DATA) {
        return tp_printf(tp,"<data 0x%x>",self.data.val);
    } else if (type == TP_FNC) {
        return tp_printf(tp,"<fnc 0x%x>",self.fnc.info);
    }
    return tp_string("<?>");
}

int tp_bool(TP,tp_obj v) {
    switch(v.type) {
        case TP_NUMBER: return v.number.val != 0;
        case TP_NONE: return 0;
        case TP_STRING: return v.string.len != 0;
        case TP_LIST: return v.list.val->len != 0;
        case TP_DICT: return v.dict.val->len != 0;
    }
    return 1;
}


tp_obj tp_has(TP,tp_obj self, tp_obj k) {
    int type = self.type;
    if (type == TP_DICT) {
        if (_tp_dict_find(tp,self.dict.val,k) != -1) { return tp_True; }
        return tp_False;
    } else if (type == TP_STRING && k.type == TP_STRING) {
        char *p = strstr(TP_CSTR(self),TP_CSTR(k));
        return tp_number(p != 0);
    } else if (type == TP_LIST) {
        return tp_number(_tp_list_find(tp,self.list.val,k)!=-1);
    }
    tp_raise(tp_None,"tp_has(%s,%s)",TP_CSTR(self),TP_CSTR(k));
}

void tp_del(TP,tp_obj self, tp_obj k) {
    int type = self.type;
    if (type == TP_DICT) {
        _tp_dict_del(tp,self.dict.val,k,"tp_del");
        return;
    }
    tp_raise(,"tp_del(%s,%s)",TP_CSTR(self),TP_CSTR(k));
}


tp_obj tp_iter(TP,tp_obj self, tp_obj k) {
    int type = self.type;
    if (type == TP_LIST || type == TP_STRING) { return tp_get(tp,self,k); }
    if (type == TP_DICT && k.type == TP_NUMBER) {
        return self.dict.val->items[_tp_dict_next(tp,self.dict.val)].key;
    }
    tp_raise(tp_None,"tp_iter(%s,%s)",TP_CSTR(self),TP_CSTR(k));
}

tp_obj tp_get(TP,tp_obj self, tp_obj k) {
    int type = self.type;
    tp_obj r;
    if (type == TP_DICT) {
        return _tp_dict_get(tp,self.dict.val,k,"tp_get");
    } else if (type == TP_LIST) {
        if (k.type == TP_NUMBER) {
            int l = tp_len(tp,self).number.val;
            int n = k.number.val;
            n = (n<0?l+n:n);
            return _tp_list_get(tp,self.list.val,n,"tp_get");
        } else if (k.type == TP_STRING) {
            if (strcmp("append",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_append);
            } else if (strcmp("pop",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_pop);
            } else if (strcmp("index",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_index);
            } else if (strcmp("sort",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_sort);
            } else if (strcmp("extend",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_extend);
            } else if (strcmp("*",TP_CSTR(k)) == 0) {
                tp_params_v(tp,1,self);
                r = tp_copy(tp);
                self.list.val->len=0;
                return r;
            }
        } else if (k.type == TP_NONE) {
            return _tp_list_pop(tp,self.list.val,0,"tp_get");
        }
    } else if (type == TP_STRING) {
        if (k.type == TP_NUMBER) {
            int l = self.string.len;
            int n = k.number.val;
            n = (n<0?l+n:n);
            if (n >= 0 && n < l) { return tp_string_n(tp->chars[(unsigned char)self.string.val[n]],1); }
        } else if (k.type == TP_STRING) {
            if (strcmp("join",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_join);
            } else if (strcmp("split",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_split);
            } else if (strcmp("index",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_str_index);
            } else if (strcmp("strip",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_strip);
            } else if (strcmp("replace",TP_CSTR(k)) == 0) {
                return tp_method(tp,self,tp_replace);
            }
        }
    }

    if (k.type == TP_LIST) {
        int a,b,l;
        tp_obj tmp;
        l = tp_len(tp,self).number.val;
        tmp = tp_get(tp,k,tp_number(0));
        if (tmp.type == TP_NUMBER) { a = tmp.number.val; }
        else if(tmp.type == TP_NONE) { a = 0; }
        else { tp_raise(tp_None,"%s is not a number",TP_CSTR(tmp)); }
        tmp = tp_get(tp,k,tp_number(1));
        if (tmp.type == TP_NUMBER) { b = tmp.number.val; }
        else if(tmp.type == TP_NONE) { b = l; }
        else { tp_raise(tp_None,"%s is not a number",TP_CSTR(tmp)); }
        a = _tp_max(0,(a<0?l+a:a)); b = _tp_min(l,(b<0?l+b:b));
        if (type == TP_LIST) {
            return tp_list_n(tp,b-a,&self.list.val->items[a]);
        } else if (type == TP_STRING) {
            tp_obj r = tp_string_t(tp,b-a);
            char *ptr = r.string.info->s;
            memcpy(ptr,self.string.val+a,b-a); ptr[b-a]=0;
            return tp_track(tp,r);
        }
    }



    tp_raise(tp_None,"tp_get(%s,%s)",TP_CSTR(self),TP_CSTR(k));
}

int tp_iget(TP,tp_obj *r, tp_obj self, tp_obj k) {
    if (self.type == TP_DICT) {
        int n = _tp_dict_find(tp,self.dict.val,k);
        if (n == -1) { return 0; }
        *r = self.dict.val->items[n].val;
        tp_grey(tp,*r);
        return 1;
    }
    if (self.type == TP_LIST && !self.list.val->len) { return 0; }
    *r = tp_get(tp,self,k); tp_grey(tp,*r);
    return 1;
}

void tp_set(TP,tp_obj self, tp_obj k, tp_obj v) {
    int type = self.type;

    if (type == TP_DICT) {
        _tp_dict_set(tp,self.dict.val,k,v);
        return;
    } else if (type == TP_LIST) {
        if (k.type == TP_NUMBER) {
            _tp_list_set(tp,self.list.val,k.number.val,v,"tp_set");
            return;
        } else if (k.type == TP_NONE) {
            _tp_list_append(tp,self.list.val,v);
            return;
        } else if (k.type == TP_STRING) {
            if (strcmp("*",TP_CSTR(k)) == 0) {
                tp_params_v(tp,2,self,v); tp_extend(tp);
                return;
            }
        }
    }
    tp_raise(,"tp_set(%s,%s,%s)",TP_CSTR(self),TP_CSTR(k),TP_CSTR(v));
}

tp_obj tp_add(TP,tp_obj a, tp_obj b) {
    if (a.type == TP_NUMBER && a.type == b.type) {
        return tp_number(a.number.val+b.number.val);
    } else if (a.type == TP_STRING && a.type == b.type) {
        int al = a.string.len, bl = b.string.len;
        tp_obj r = tp_string_t(tp,al+bl);
        char *s = r.string.info->s;
        memcpy(s,a.string.val,al); memcpy(s+al,b.string.val,bl);
        return tp_track(tp,r);
    } else if (a.type == TP_LIST && a.type == b.type) {
        tp_obj r;
        tp_params_v(tp,1,a);
        r = tp_copy(tp);
        tp_params_v(tp,2,r,b);
        tp_extend(tp);
        return r;
    }
    tp_raise(tp_None,"tp_add(%s,%s)",TP_CSTR(a),TP_CSTR(b));
}

tp_obj tp_mul(TP,tp_obj a, tp_obj b) {
    if (a.type == TP_NUMBER && a.type == b.type) {
        return tp_number(a.number.val*b.number.val);
    } else if (a.type == TP_STRING && b.type == TP_NUMBER) {
        int al = a.string.len; int n = b.number.val;
        tp_obj r = tp_string_t(tp,al*n);
        char *s = r.string.info->s;
        int i; for (i=0; i<n; i++) { memcpy(s+al*i,a.string.val,al); }
        return tp_track(tp,r);
    }
    tp_raise(tp_None,"tp_mul(%s,%s)",TP_CSTR(a),TP_CSTR(b));
}


tp_obj tp_len(TP,tp_obj self) {
    int type = self.type;
    if (type == TP_STRING) {
        return tp_number(self.string.len);
    } else if (type == TP_DICT) {
        return tp_number(self.dict.val->len);
    } else if (type == TP_LIST) {
        return tp_number(self.list.val->len);
    }
    tp_raise(tp_None,"tp_len(%s)",TP_CSTR(self));
}

int tp_cmp(TP,tp_obj a, tp_obj b) {
    if (a.type != b.type) { return a.type-b.type; }
    switch(a.type) {
        case TP_NONE: return 0;
        case TP_NUMBER: return _tp_sign(a.number.val-b.number.val);
        case TP_STRING: {
            int v = memcmp(a.string.val,b.string.val,_tp_min(a.string.len,b.string.len));
            if (v == 0) { v = a.string.len-b.string.len; }
            return v;
        }
        case TP_LIST: {
            int n,v; for(n=0;n<_tp_min(a.list.val->len,b.list.val->len);n++) {
        tp_obj aa = a.list.val->items[n]; tp_obj bb = b.list.val->items[n];
            if (aa.type == TP_LIST && bb.type == TP_LIST) { v = aa.list.val-bb.list.val; } else { v = tp_cmp(tp,aa,bb); }
            if (v) { return v; } }
            return a.list.val->len-b.list.val->len;
        }
        case TP_DICT: return a.dict.val - b.dict.val;
        case TP_FNC: return a.fnc.info - b.fnc.info;
        case TP_DATA: return (char*)a.data.val - (char*)b.data.val;
    }
    tp_raise(0,"tp_cmp(%s,%s)",TP_CSTR(a),TP_CSTR(b));
}

#define TP_OP(name,expr) \
    tp_obj name(TP,tp_obj _a,tp_obj _b) { \
    if (_a.type == TP_NUMBER && _a.type == _b.type) { \
        tp_num a = _a.number.val; tp_num b = _b.number.val; \
        return tp_number(expr); \
    } \
    tp_raise(tp_None,"%s(%s,%s)",#name,TP_CSTR(_a),TP_CSTR(_b)); \
}

TP_OP(tp_and,((long)a)&((long)b));
TP_OP(tp_or,((long)a)|((long)b));
TP_OP(tp_mod,((long)a)%((long)b));
TP_OP(tp_lsh,((long)a)<<((long)b));
TP_OP(tp_rsh,((long)a)>>((long)b));
TP_OP(tp_sub,a-b);
TP_OP(tp_div,a/b);
TP_OP(tp_pow,pow(a,b));


/**/

tp_vm *_tp_init(void) {
    int i;
    tp_vm *tp = (tp_vm*)tp_malloc(sizeof(tp_vm));
    tp->cur = 0;
    tp->jmp = 0;
    tp->ex = tp_None;
    tp->root = tp_list(0);
    for (i=0; i<256; i++) { tp->chars[i][0]=i; }
    tp_gc_init(tp);
    tp->_regs = tp_list(tp);
    for (i=0; i<TP_REGS; i++) { tp_set(tp,tp->_regs,tp_None,tp_None); }
    tp->builtins = tp_dict(tp);
    tp->modules = tp_dict(tp);
    tp->_params = tp_list(tp);
    for (i=0; i<TP_FRAMES; i++) { tp_set(tp,tp->_params,tp_None,tp_list(tp)); }
    tp_set(tp,tp->root,tp_None,tp->builtins);
    tp_set(tp,tp->root,tp_None,tp->modules);
    tp_set(tp,tp->root,tp_None,tp->_regs);
    tp_set(tp,tp->root,tp_None,tp->_params);
    tp_set(tp,tp->builtins,tp_string("MODULES"),tp->modules);
    tp_set(tp,tp->modules,tp_string("BUILTINS"),tp->builtins);
    tp_set(tp,tp->builtins,tp_string("BUILTINS"),tp->builtins);
    tp->regs = tp->_regs.list.val->items;
    tp_full(tp);
    return tp;
}

void tp_deinit(TP) {
    while (tp->root.list.val->len) {
        _tp_list_pop(tp,tp->root.list.val,0,"tp_deinit");
    }
    tp_full(tp); tp_full(tp);
    tp_delete(tp,tp->root);
    tp_gc_deinit(tp);
    tp_free(tp);
}


/* tp_frame_*/
void tp_frame(TP,tp_obj globals,tp_code *codes,tp_obj *ret_dest) {
    tp_frame_ f;
    f.globals = globals;
    f.codes = codes;
    f.cur = f.codes;
    f.jmp = 0;
/*     fprintf(stderr,"tp->cur: %d\n",tp->cur);*/
    f.regs = (tp->cur <= 0?tp->regs:tp->frames[tp->cur].regs+tp->frames[tp->cur].cregs);
    f.ret_dest = ret_dest;
    f.lineno = 0;
    f.line = tp_string("");
    f.name = tp_string("?");
    f.fname = tp_string("?");
    f.cregs = 0;
/*     return f;*/
    if (f.regs+256 >= tp->regs+TP_REGS || tp->cur >= TP_FRAMES-1) { tp_raise(,"tp_frame: stack overflow %d",tp->cur); }
    tp->cur += 1;
    tp->frames[tp->cur] = f;
}

void _tp_raise(TP,tp_obj e) {
    if (!tp || !tp->jmp) {
        printf("\nException:\n%s\n",TP_CSTR(e));
        exit(-1);
        return;
    }
    if (e.type != TP_NONE) { tp->ex = e; }
    tp_grey(tp,e);
    longjmp(tp->buf,1);
}

void tp_print_stack(TP) {
    int i;
    printf("\n");
    for (i=0; i<=tp->cur; i++) {
        if (!tp->frames[i].lineno) { continue; }
        printf("File \"%s\", line %d, in %s\n  %s\n",
            TP_CSTR(tp->frames[i].fname),tp->frames[i].lineno,
            TP_CSTR(tp->frames[i].name),TP_CSTR(tp->frames[i].line));
    }
    printf("\nException:\n%s\n",TP_CSTR(tp->ex));
}



void tp_handle(TP) {
    int i;
    for (i=tp->cur; i>=0; i--) {
        if (tp->frames[i].jmp) { break; }
    }
    if (i >= 0) {
        tp->cur = i;
        tp->frames[i].cur = tp->frames[i].jmp;
        tp->frames[i].jmp = 0;
        return;
    }
	else
	{
		tp->cur = 0;
		tp->frames[0].cur = 0;
		tp->frames[0].jmp = 0;
	}

    tp_print_stack(tp);
    //exit(-1);
}

void _tp_call(TP,tp_obj *dest, tp_obj fnc, tp_obj params) {
    if (fnc.type == TP_DICT) {
        _tp_call(tp,dest,tp_get(tp,fnc,tp_string("__call__")),params);
        return;
    }
    if (fnc.type == TP_FNC && !(fnc.fnc.ftype&1)) {
        *dest = _tp_tcall(tp,fnc);
        tp_grey(tp,*dest);
        return;
    }
    if (fnc.type == TP_FNC) {
        tp_frame(tp,fnc.fnc.info->globals,(tp_code*)fnc.fnc.val,dest);
        if ((fnc.fnc.ftype&2)) {
            tp->frames[tp->cur].regs[0] = params;
            _tp_list_insert(tp,params.list.val,0,fnc.fnc.info->self);
        } else {
            tp->frames[tp->cur].regs[0] = params;
        }
        return;
    }
    tp_params_v(tp,1,fnc); tp_print(tp);
    tp_raise(,"tp_call: %s is not callable",TP_CSTR(fnc));
}


void tp_return(TP, tp_obj v) {
    tp_obj *dest = tp->frames[tp->cur].ret_dest;
    if (dest) { *dest = v; tp_grey(tp,v); }
/*     memset(tp->frames[tp->cur].regs,0,TP_REGS_PER_FRAME*sizeof(tp_obj));
       fprintf(stderr,"regs:%d\n",(tp->frames[tp->cur].cregs+1));*/
    memset(tp->frames[tp->cur].regs,0,tp->frames[tp->cur].cregs*sizeof(tp_obj));
    tp->cur -= 1;
}

enum {
    TP_IEOF,TP_IADD,TP_ISUB,TP_IMUL,TP_IDIV,TP_IPOW,TP_IAND,TP_IOR,TP_ICMP,TP_IGET,TP_ISET,
    TP_INUMBER,TP_ISTRING,TP_IGGET,TP_IGSET,TP_IMOVE,TP_IDEF,TP_IPASS,TP_IJUMP,TP_ICALL,
    TP_IRETURN,TP_IIF,TP_IDEBUG,TP_IEQ,TP_ILE,TP_ILT,TP_IDICT,TP_ILIST,TP_INONE,TP_ILEN,
    TP_ILINE,TP_IPARAMS,TP_IIGET,TP_IFILE,TP_INAME,TP_INE,TP_IHAS,TP_IRAISE,TP_ISETJMP,
    TP_IMOD,TP_ILSH,TP_IRSH,TP_IITER,TP_IDEL,TP_IREGS,
    TP_ITOTAL
};

/* char *tp_strings[TP_ITOTAL] = {
       "EOF","ADD","SUB","MUL","DIV","POW","AND","OR","CMP","GET","SET","NUM",
       "STR","GGET","GSET","MOVE","DEF","PASS","JUMP","CALL","RETURN","IF","DEBUG",
       "EQ","LE","LT","DICT","LIST","NONE","LEN","LINE","PARAMS","IGET","FILE",
       "NAME","NE","HAS","RAISE","SETJMP","MOD","LSH","RSH","ITER","DEL","REGS",
   };*/

#define VA ((int)e.regs.a)
#define VB ((int)e.regs.b)
#define VC ((int)e.regs.c)
#define RA regs[e.regs.a]
#define RB regs[e.regs.b]
#define RC regs[e.regs.c]
#define UVBC (unsigned short)(((VB<<8)+VC))
#define SVBC (short)(((VB<<8)+VC))
#define GA tp_grey(tp,RA)
#define SR(v) f->cur = cur; return(v);

int tp_step(TP) {
    tp_frame_ *f = &tp->frames[tp->cur];
    tp_obj *regs = f->regs;
    tp_code *cur = f->cur;
    while(1) {
    tp_code e = *cur;
/*     fprintf(stderr,"%2d.%4d: %-6s %3d %3d %3d\n",tp->cur,cur-f->codes,tp_strings[e.i],VA,VB,VC);
       int i; for(i=0;i<16;i++) { fprintf(stderr,"%d: %s\n",i,TP_CSTR(regs[i])); }*/
    switch (e.i) {
        case TP_IEOF: tp_return(tp,tp_None); SR(0); break;
        case TP_IADD: RA = tp_add(tp,RB,RC); break;
        case TP_ISUB: RA = tp_sub(tp,RB,RC); break;
        case TP_IMUL: RA = tp_mul(tp,RB,RC); break;
        case TP_IDIV: RA = tp_div(tp,RB,RC); break;
        case TP_IPOW: RA = tp_pow(tp,RB,RC); break;
        case TP_IAND: RA = tp_and(tp,RB,RC); break;
        case TP_IOR:  RA = tp_or(tp,RB,RC); break;
        case TP_IMOD:  RA = tp_mod(tp,RB,RC); break;
        case TP_ILSH:  RA = tp_lsh(tp,RB,RC); break;
        case TP_IRSH:  RA = tp_rsh(tp,RB,RC); break;
        case TP_ICMP: RA = tp_number(tp_cmp(tp,RB,RC)); break;
        case TP_INE: RA = tp_number(tp_cmp(tp,RB,RC)!=0); break;
        case TP_IEQ: RA = tp_number(tp_cmp(tp,RB,RC)==0); break;
        case TP_ILE: RA = tp_number(tp_cmp(tp,RB,RC)<=0); break;
        case TP_ILT: RA = tp_number(tp_cmp(tp,RB,RC)<0); break;
        case TP_IPASS: break;
        case TP_IIF: if (tp_bool(tp,RA)) { cur += 1; } break;
        case TP_IGET: RA = tp_get(tp,RB,RC); GA; break;
        case TP_IITER:
            if (RC.number.val < tp_len(tp,RB).number.val) {
                RA = tp_iter(tp,RB,RC); GA;
                RC.number.val += 1;
                cur += 1;
            }
            break;
        case TP_IHAS: RA = tp_has(tp,RB,RC); break;
        case TP_IIGET: tp_iget(tp,&RA,RB,RC); break;
        case TP_ISET: tp_set(tp,RA,RB,RC); break;
        case TP_IDEL: tp_del(tp,RA,RB); break;
        case TP_IMOVE: RA = RB; break;
        case TP_INUMBER:
            RA = tp_number(*(tp_num*)(*++cur).string.val);
            cur += sizeof(tp_num)/4;
            continue;
        case TP_ISTRING:
            RA = tp_string_n((*(cur+1)).string.val,UVBC);
            cur += (UVBC/4)+1;
            break;
        case TP_IDICT: RA = tp_dict_n(tp,VC/2,&RB); break;
        case TP_ILIST: RA = tp_list_n(tp,VC,&RB); break;
        case TP_IPARAMS: RA = tp_params_n(tp,VC,&RB); break;
        case TP_ILEN: RA = tp_len(tp,RB); break;
        case TP_IJUMP: cur += SVBC; continue; break;
        case TP_ISETJMP: f->jmp = cur+SVBC; break;
        case TP_ICALL: _tp_call(tp,&RA,RB,RC); cur++; SR(0); break;
        case TP_IGGET:
            if (!tp_iget(tp,&RA,f->globals,RB)) {
                RA = tp_get(tp,tp->builtins,RB); GA;
            }
            break;
        case TP_IGSET: tp_set(tp,f->globals,RA,RB); break;
        case TP_IDEF:
            RA = tp_def(tp,(*(cur+1)).string.val,f->globals);
            cur += SVBC; continue;
            break;
        case TP_IRETURN: tp_return(tp,RA); SR(0); break;
        case TP_IRAISE: _tp_raise(tp,RA); SR(0); break;
        case TP_IDEBUG:
            tp_params_v(tp,3,tp_string("DEBUG:"),tp_number(VA),RA); tp_print(tp);
            break;
        case TP_INONE: RA = tp_None; break;
        case TP_ILINE:
            f->line = tp_string_n((*(cur+1)).string.val,VA*4-1);
/*             fprintf(stderr,"%7d: %s\n",UVBC,f->line.string.val);*/
            cur += VA; f->lineno = UVBC;
            break;
        case TP_IFILE: f->fname = RA; break;
        case TP_INAME: f->name = RA; break;
        case TP_IREGS: f->cregs = VA; break;
        default: tp_raise(0,"tp_step: invalid instruction %d",e.i); break;
    }
    cur += 1;
    }
    SR(0);
}

void tp_run(TP,int cur) {
    if (tp->jmp) { tp_raise(,"tp_run(%d) called recusively",cur); }

	tp->jmp = 1; 

	if (setjmp(tp->buf)) 
	{ 
		tp_handle(tp); 
	}

    while (tp->cur >= cur && tp_step(tp) != -1);
    tp->cur = cur-1; tp->jmp = 0;
}


tp_obj tp_call(TP, const char *mod, const char *fnc, tp_obj params) {
    tp_obj tmp;
    tp_obj r = tp_None;
    tmp = tp_get(tp,tp->modules,tp_string(mod));
    tmp = tp_get(tp,tmp,tp_string(fnc));
    _tp_call(tp,&r,tmp,params);
    tp_run(tp,tp->cur);
    return r;
}

tp_obj tp_import(TP, char const *fname, char const *name, void *codes) {
    tp_obj code = tp_None;
    tp_obj g;

    if (!((fname && strstr(fname,".tpc")) || codes)) {
        return tp_call(tp,"py2bc","import_fname",tp_params_v(tp,2,tp_string(fname),tp_string(name)));
    }

    if (!codes) {
        tp_params_v(tp,1,tp_string(fname));
        code = tp_load(tp);
        /* We cast away the constness. */
        codes = (void *)code.string.val;
    } else {
        code = tp_data(tp,0,codes);
    }

    g = tp_dict(tp);
    tp_set(tp,g,tp_string("__name__"),tp_string(name));
    tp_set(tp,g,tp_string("__code__"),code);
    tp_set(tp,g,tp_string("__dict__"),g);
    tp_frame(tp,g,(tp_code*)codes,0);
    tp_set(tp,tp->modules,tp_string(name),g);

    if (!tp->jmp) { tp_run(tp,tp->cur); }

    return g;
}



tp_obj tp_exec_(TP) {
    tp_obj code = TP_OBJ();
    tp_obj globals = TP_OBJ();
    tp_frame(tp,globals,(tp_code*)code.string.val,0);
    return tp_None;
}


tp_obj tp_import_(TP) {
    tp_obj mod = TP_OBJ();
    char const *s;
    tp_obj r;

    if (tp_has(tp,tp->modules,mod).number.val) {
        return tp_get(tp,tp->modules,mod);
    }

    s = TP_CSTR(mod);
    r = tp_import(tp,TP_CSTR(tp_add(tp,mod,tp_string(".tpc"))),s,0);
    return r;
}

void tp_builtins(TP) {
    struct {const char *s;void *f;} b[] = {
    {"print",tp_print}, {"range",tp_range}, {"min",tp_min},
    {"max",tp_max}, {"bind",tp_bind}, {"copy",tp_copy},
    {"import",tp_import_}, {"len",tp_len_}, {"assert",tp_assert},
    {"str",tp_str2}, {"float",tp_float}, {"system",tp_system},
    {"istype",tp_istype}, {"chr",tp_chr}, {"save",tp_save},
    {"load",tp_load}, {"fpack",tp_fpack}, {"abs",tp_abs},
    {"int",tp_int}, {"exec",tp_exec_}, {"exists",tp_exists},
    {"mtime",tp_mtime}, {"number",tp_float}, {"round",tp_round},
    {"ord",tp_ord}, {"merge",tp_merge}, {0,0},
    };
    int i; for(i=0; b[i].s; i++) {
        tp_set(tp,tp->builtins,tp_string(b[i].s),tp_fnc(tp,(tp_obj (*)(tp_vm *))b[i].f));
    }
}


void tp_args(TP,int argc, char *argv[]) {
    tp_obj self = tp_list(tp);
    int i;
    for (i=1; i<argc; i++) { _tp_list_append(tp,self.list.val,tp_string(argv[i])); }
    tp_set(tp,tp->builtins,tp_string("ARGV"),self);
}


tp_obj tp_main(TP,char *fname, void *code) {
    return tp_import(tp,fname,"__main__",code);
}
tp_obj tp_compile(TP, tp_obj text, tp_obj fname) {
    return tp_call(tp,"BUILTINS","compile",tp_params_v(tp,2,text,fname));
}

tp_obj tp_exec(TP,tp_obj code, tp_obj globals) {
    tp_obj r=tp_None;
    tp_frame(tp,globals,(tp_code*)code.string.val,&r);
    tp_run(tp,tp->cur);
    return r;
}

tp_obj tp_eval(TP, char *text, tp_obj globals) {
    tp_obj code = tp_compile(tp,tp_string(text),tp_string("<eval>"));
    return tp_exec(tp,code,globals);
}

tp_vm *tp_init(int argc, char *argv[]) {
    tp_vm *tp = _tp_init();
    tp_builtins(tp);
    tp_args(tp,argc,argv);
    tp_compiler(tp);
    return tp;
}


/**/
#ifndef TP_COMPILER
#define TP_COMPILER 1
#endif

void tp_compiler(TP);

tp_obj tp_None = {TP_NONE};

#if TP_COMPILER
void tp_compiler(TP) {
    tp_import(tp,0,"tokenize",tp_tokenize);
    tp_import(tp,0,"parse",tp_parse);
    tp_import(tp,0,"encode",tp_encode);
    tp_import(tp,0,"py2bc",tp_py2bc);
    tp_call(tp,"py2bc","_init",tp_None);
}
#else
void tp_compiler(TP) { }
#endif

/**/
unsigned char tp_tokenize[] = {
44,65,0,0,26,0,0,0,12,1,0,5,84,111,107,101,
110,0,0,0,14,1,0,0,16,1,0,43,44,11,0,0,
28,2,0,0,9,1,0,2,11,3,0,0,0,0,0,0,
0,0,0,0,11,4,0,0,0,0,0,0,0,0,0,0,
27,2,3,2,28,3,0,0,32,2,0,3,12,3,0,6,
115,121,109,98,111,108,0,0,28,4,0,0,32,3,0,4,
28,4,0,0,28,5,0,0,32,4,0,5,28,5,0,0,
28,6,0,0,32,5,0,6,15,6,2,0,15,7,3,0,
15,8,4,0,15,9,5,0,12,10,0,3,112,111,115,0,
10,1,10,6,12,6,0,4,116,121,112,101,0,0,0,0,
10,1,6,7,12,6,0,3,118,97,108,0,10,1,6,8,
12,6,0,5,105,116,101,109,115,0,0,0,10,1,6,9,
0,0,0,0,12,2,0,8,95,95,105,110,105,116,95,95,
0,0,0,0,10,0,2,1,16,2,0,26,44,7,0,0,
28,2,0,0,9,1,0,2,12,4,0,4,98,105,110,100,
0,0,0,0,13,3,4,0,12,6,0,5,84,111,107,101,
110,0,0,0,13,4,6,0,12,6,0,8,95,95,105,110,
105,116,95,95,0,0,0,0,9,4,4,6,15,5,1,0,
31,2,4,2,19,2,3,2,12,3,0,8,95,95,105,110,
105,116,95,95,0,0,0,0,10,1,3,2,0,0,0,0,
12,3,0,7,95,95,110,101,119,95,95,0,10,0,3,2,
16,3,0,22,44,5,0,0,26,1,0,0,12,4,0,5,
84,111,107,101,110,0,0,0,13,3,4,0,12,4,0,7,
95,95,110,101,119,95,95,0,9,3,3,4,15,4,1,0,
31,2,4,1,19,2,3,2,12,3,0,8,95,95,105,110,
105,116,95,95,0,0,0,0,9,2,1,3,19,4,2,0,
20,1,0,0,0,0,0,0,12,4,0,8,95,95,99,97,
108,108,95,95,0,0,0,0,10,0,4,3,16,0,0,97,
44,12,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,28,4,0,0,9,3,0,4,11,6,0,0,
0,0,0,0,0,0,0,0,9,5,3,6,15,4,5,0,
11,7,0,0,0,0,0,0,0,0,240,63,9,6,3,7,
15,5,6,0,12,8,0,5,115,112,108,105,116,0,0,0,
9,7,2,8,12,8,0,1,10,0,0,0,31,6,8,1,
19,6,7,6,11,8,0,0,0,0,0,0,0,0,240,63,
2,7,4,8,9,6,6,7,15,3,6,0,12,7,0,0,
0,0,0,0,15,6,7,0,11,8,0,0,0,0,0,0,
0,0,36,64,25,7,4,8,21,7,0,0,18,0,0,6,
12,8,0,1,32,0,0,0,1,7,6,8,15,6,7,0,
18,0,0,1,11,8,0,0,0,0,0,0,0,0,89,64,
25,7,4,8,21,7,0,0,18,0,0,6,12,8,0,2,
32,32,0,0,1,7,6,8,15,6,7,0,18,0,0,1,
12,11,0,3,115,116,114,0,13,10,11,0,15,11,4,0,
31,9,11,1,19,9,10,9,1,8,6,9,12,9,0,2,
58,32,0,0,1,8,8,9,1,8,8,3,12,9,0,1,
10,0,0,0,1,8,8,9,15,7,8,0,12,9,0,5,
32,32,32,32,32,0,0,0,12,10,0,1,32,0,0,0,
3,10,10,5,1,9,9,10,12,10,0,1,94,0,0,0,
1,9,9,10,12,10,0,1,10,0,0,0,1,9,9,10,
1,8,7,9,15,7,8,0,12,8,0,7,101,114,114,111,
114,58,32,0,1,8,8,1,12,9,0,1,10,0,0,0,
1,8,8,9,1,8,8,7,37,8,0,0,0,0,0,0,
12,4,0,7,117,95,101,114,114,111,114,0,14,4,0,0,
12,4,0,8,73,83,89,77,66,79,76,83,0,0,0,0,
12,5,0,26,96,45,61,91,93,59,44,46,47,126,33,64,
36,37,94,38,42,40,41,43,123,125,58,60,62,63,0,0,
14,4,5,0,12,4,0,7,83,89,77,66,79,76,83,0,
12,6,0,3,100,101,102,0,12,7,0,5,99,108,97,115,
115,0,0,0,12,8,0,5,121,105,101,108,100,0,0,0,
12,9,0,6,114,101,116,117,114,110,0,0,12,10,0,4,
112,97,115,115,0,0,0,0,12,11,0,3,97,110,100,0,
12,12,0,2,111,114,0,0,12,13,0,3,110,111,116,0,
12,14,0,2,105,110,0,0,12,15,0,6,105,109,112,111,
114,116,0,0,12,16,0,2,105,115,0,0,12,17,0,5,
119,104,105,108,101,0,0,0,12,18,0,5,98,114,101,97,
107,0,0,0,12,19,0,3,102,111,114,0,12,20,0,8,
99,111,110,116,105,110,117,101,0,0,0,0,12,21,0,2,
105,102,0,0,12,22,0,4,101,108,115,101,0,0,0,0,
12,23,0,4,101,108,105,102,0,0,0,0,12,24,0,3,
116,114,121,0,12,25,0,6,101,120,99,101,112,116,0,0,
12,26,0,5,114,97,105,115,101,0,0,0,12,27,0,4,
84,114,117,101,0,0,0,0,12,28,0,5,70,97,108,115,
101,0,0,0,12,29,0,4,78,111,110,101,0,0,0,0,
12,30,0,6,103,108,111,98,97,108,0,0,12,31,0,3,
100,101,108,0,12,32,0,4,102,114,111,109,0,0,0,0,
12,33,0,1,45,0,0,0,12,34,0,1,43,0,0,0,
12,35,0,1,42,0,0,0,12,36,0,2,42,42,0,0,
12,37,0,1,47,0,0,0,12,38,0,1,37,0,0,0,
12,39,0,2,60,60,0,0,12,40,0,2,62,62,0,0,
12,41,0,2,45,61,0,0,12,42,0,2,43,61,0,0,
12,43,0,2,42,61,0,0,12,44,0,2,47,61,0,0,
12,45,0,1,61,0,0,0,12,46,0,2,61,61,0,0,
12,47,0,2,33,61,0,0,12,48,0,1,60,0,0,0,
12,49,0,1,62,0,0,0,12,50,0,2,60,61,0,0,
12,51,0,2,62,61,0,0,12,52,0,1,91,0,0,0,
12,53,0,1,93,0,0,0,12,54,0,1,123,0,0,0,
12,55,0,1,125,0,0,0,12,56,0,1,40,0,0,0,
12,57,0,1,41,0,0,0,12,58,0,1,46,0,0,0,
12,59,0,1,58,0,0,0,12,60,0,1,44,0,0,0,
12,61,0,1,59,0,0,0,12,62,0,1,38,0,0,0,
12,63,0,1,124,0,0,0,12,64,0,1,33,0,0,0,
27,5,6,59,14,4,5,0,12,6,0,1,91,0,0,0,
12,7,0,1,40,0,0,0,12,8,0,1,123,0,0,0,
27,5,6,3,15,4,5,0,12,7,0,1,93,0,0,0,
12,8,0,1,41,0,0,0,12,9,0,1,125,0,0,0,
27,6,7,3,15,5,6,0,12,6,0,7,66,95,66,69,
71,73,78,0,14,6,4,0,12,4,0,5,66,95,69,78,
68,0,0,0,14,4,5,0,26,4,0,0,12,5,0,5,
84,68,97,116,97,0,0,0,14,5,4,0,16,5,0,48,
44,6,0,0,28,2,0,0,9,1,0,2,11,3,0,0,
0,0,0,0,0,0,240,63,15,2,3,0,11,4,0,0,
0,0,0,0,0,0,0,0,15,3,4,0,11,5,0,0,
0,0,0,0,0,0,240,63,15,4,5,0,12,5,0,1,
121,0,0,0,10,1,5,2,12,2,0,2,121,105,0,0,
10,1,2,3,12,2,0,2,110,108,0,0,10,1,2,4,
27,3,0,0,15,2,3,0,11,5,0,0,0,0,0,0,
0,0,0,0,27,4,5,1,15,3,4,0,11,5,0,0,
0,0,0,0,0,0,0,0,15,4,5,0,12,5,0,3,
114,101,115,0,10,1,5,2,12,2,0,6,105,110,100,101,
110,116,0,0,10,1,2,3,12,2,0,6,98,114,97,99,
101,115,0,0,10,1,2,4,0,0,0,0,12,6,0,8,
95,95,105,110,105,116,95,95,0,0,0,0,10,4,6,5,
16,6,0,29,44,12,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,28,4,0,0,9,3,0,4,
12,6,0,3,114,101,115,0,9,5,1,6,12,6,0,6,
97,112,112,101,110,100,0,0,9,5,5,6,12,8,0,5,
84,111,107,101,110,0,0,0,13,7,8,0,12,11,0,1,
102,0,0,0,9,8,1,11,15,9,2,0,15,10,3,0,
31,6,8,3,19,6,7,6,31,4,6,1,19,4,5,4,
0,0,0,0,12,7,0,3,97,100,100,0,10,4,7,6,
16,7,0,43,44,7,0,0,28,2,0,0,9,1,0,2,
12,4,0,4,98,105,110,100,0,0,0,0,13,3,4,0,
12,6,0,5,84,68,97,116,97,0,0,0,13,4,6,0,
12,6,0,8,95,95,105,110,105,116,95,95,0,0,0,0,
9,4,4,6,15,5,1,0,31,2,4,2,19,2,3,2,
12,3,0,8,95,95,105,110,105,116,95,95,0,0,0,0,
10,1,3,2,12,4,0,4,98,105,110,100,0,0,0,0,
13,3,4,0,12,6,0,5,84,68,97,116,97,0,0,0,
13,4,6,0,12,6,0,3,97,100,100,0,9,4,4,6,
15,5,1,0,31,2,4,2,19,2,3,2,12,3,0,3,
97,100,100,0,10,1,3,2,0,0,0,0,12,8,0,7,
95,95,110,101,119,95,95,0,10,4,8,7,16,8,0,22,
44,5,0,0,26,1,0,0,12,4,0,5,84,68,97,116,
97,0,0,0,13,3,4,0,12,4,0,7,95,95,110,101,
119,95,95,0,9,3,3,4,15,4,1,0,31,2,4,1,
19,2,3,2,12,3,0,8,95,95,105,110,105,116,95,95,
0,0,0,0,9,2,1,3,19,4,2,0,20,1,0,0,
0,0,0,0,12,9,0,8,95,95,99,97,108,108,95,95,
0,0,0,0,10,4,9,8,16,4,0,28,44,6,0,0,
28,2,0,0,9,1,0,2,12,4,0,7,114,101,112,108,
97,99,101,0,9,3,1,4,12,4,0,2,13,10,0,0,
12,5,0,1,10,0,0,0,31,2,4,2,19,2,3,2,
15,1,2,0,12,4,0,7,114,101,112,108,97,99,101,0,
9,3,1,4,12,4,0,1,13,0,0,0,12,5,0,1,
10,0,0,0,31,2,4,2,19,2,3,2,15,1,2,0,
20,1,0,0,0,0,0,0,12,9,0,5,99,108,101,97,
110,0,0,0,14,9,4,0,16,9,0,41,44,8,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,99,108,101,97,
110,0,0,0,13,3,4,0,15,4,1,0,31,2,4,1,
19,2,3,2,15,1,2,0,38,0,0,11,12,4,0,11,
100,111,95,116,111,107,101,110,105,122,101,0,13,3,4,0,
15,4,1,0,31,2,4,1,19,2,3,2,20,2,0,0,
18,0,0,18,12,4,0,7,117,95,101,114,114,111,114,0,
13,3,4,0,12,4,0,8,116,111,107,101,110,105,122,101,
0,0,0,0,15,5,1,0,12,7,0,1,84,0,0,0,
13,6,7,0,12,7,0,1,102,0,0,0,9,6,6,7,
31,2,4,3,19,2,3,2,0,0,0,0,12,10,0,8,
116,111,107,101,110,105,122,101,0,0,0,0,14,10,9,0,
16,10,1,128,44,13,0,0,28,2,0,0,9,1,0,2,
12,5,0,5,84,68,97,116,97,0,0,0,13,4,5,0,
31,3,0,0,19,3,4,3,15,2,3,0,11,4,0,0,
0,0,0,0,0,0,0,0,15,3,4,0,12,7,0,3,
108,101,110,0,13,6,7,0,15,7,1,0,31,5,7,1,
19,5,6,5,15,4,5,0,12,5,0,1,84,0,0,0,
14,5,2,0,15,2,3,0,15,3,4,0,12,5,0,1,
84,0,0,0,13,4,5,0,12,8,0,1,84,0,0,0,
13,6,8,0,12,8,0,1,121,0,0,0,9,6,6,8,
12,9,0,1,84,0,0,0,13,8,9,0,12,9,0,2,
121,105,0,0,9,8,8,9,2,7,2,8,11,8,0,0,
0,0,0,0,0,0,240,63,1,7,7,8,27,5,6,2,
12,6,0,1,102,0,0,0,10,4,6,5,25,4,2,3,
21,4,0,0,18,0,1,53,9,5,1,2,15,4,5,0,
12,6,0,1,84,0,0,0,13,5,6,0,12,9,0,1,
84,0,0,0,13,7,9,0,12,9,0,1,121,0,0,0,
9,7,7,9,12,10,0,1,84,0,0,0,13,9,10,0,
12,10,0,2,121,105,0,0,9,9,9,10,2,8,2,9,
11,9,0,0,0,0,0,0,0,0,240,63,1,8,8,9,
27,6,7,2,12,7,0,1,102,0,0,0,10,5,7,6,
12,6,0,1,84,0,0,0,13,5,6,0,12,6,0,2,
110,108,0,0,9,5,5,6,21,5,0,0,18,0,0,22,
12,6,0,1,84,0,0,0,13,5,6,0,11,6,0,0,
0,0,0,0,0,0,0,0,12,7,0,2,110,108,0,0,
10,5,7,6,12,7,0,9,100,111,95,105,110,100,101,110,
116,0,0,0,13,6,7,0,15,7,1,0,15,8,2,0,
15,9,3,0,31,5,7,3,19,5,6,5,15,2,5,0,
18,0,0,253,12,6,0,1,10,0,0,0,23,5,4,6,
21,5,0,0,18,0,0,12,12,7,0,5,100,111,95,110,
108,0,0,0,13,6,7,0,15,7,1,0,15,8,2,0,
15,9,3,0,31,5,7,3,19,5,6,5,15,2,5,0,
18,0,0,237,12,6,0,8,73,83,89,77,66,79,76,83,
0,0,0,0,13,5,6,0,36,5,5,4,21,5,0,0,
18,0,0,13,12,7,0,9,100,111,95,115,121,109,98,111,
108,0,0,0,13,6,7,0,15,7,1,0,15,8,2,0,
15,9,3,0,31,5,7,3,19,5,6,5,15,2,5,0,
18,0,0,217,11,7,0,0,0,0,0,0,0,0,0,0,
12,5,0,1,48,0,0,0,24,5,5,4,23,6,5,7,
21,6,0,0,18,0,0,2,18,0,0,4,12,8,0,1,
57,0,0,0,24,5,4,8,21,5,0,0,18,0,0,13,
12,7,0,9,100,111,95,110,117,109,98,101,114,0,0,0,
13,6,7,0,15,7,1,0,15,8,2,0,15,9,3,0,
31,5,7,3,19,5,6,5,15,2,5,0,18,0,0,190,
11,7,0,0,0,0,0,0,0,0,240,63,11,9,0,0,
0,0,0,0,0,0,240,63,11,11,0,0,0,0,0,0,
0,0,0,0,12,5,0,1,97,0,0,0,24,5,5,4,
23,10,5,11,21,10,0,0,18,0,0,2,18,0,0,4,
12,12,0,1,122,0,0,0,24,5,4,12,23,8,5,9,
21,8,0,0,18,0,0,2,18,0,0,14,11,11,0,0,
0,0,0,0,0,0,0,0,12,5,0,1,65,0,0,0,
24,5,5,4,23,10,5,11,21,10,0,0,18,0,0,2,
18,0,0,4,12,12,0,1,90,0,0,0,24,5,4,12,
23,6,5,7,21,6,0,0,18,0,0,2,18,0,0,4,
12,8,0,1,95,0,0,0,23,5,4,8,21,5,0,0,
18,0,0,12,12,7,0,7,100,111,95,110,97,109,101,0,
13,6,7,0,15,7,1,0,15,8,2,0,15,9,3,0,
31,5,7,3,19,5,6,5,15,2,5,0,18,0,0,134,
11,7,0,0,0,0,0,0,0,0,240,63,12,8,0,1,
34,0,0,0,23,5,4,8,23,6,5,7,21,6,0,0,
18,0,0,2,18,0,0,4,12,8,0,1,39,0,0,0,
23,5,4,8,21,5,0,0,18,0,0,13,12,7,0,9,
100,111,95,115,116,114,105,110,103,0,0,0,13,6,7,0,
15,7,1,0,15,8,2,0,15,9,3,0,31,5,7,3,
19,5,6,5,15,2,5,0,18,0,0,107,12,6,0,1,
35,0,0,0,23,5,4,6,21,5,0,0,18,0,0,13,
12,7,0,10,100,111,95,99,111,109,109,101,110,116,0,0,
13,6,7,0,15,7,1,0,15,8,2,0,15,9,3,0,
31,5,7,3,19,5,6,5,15,2,5,0,18,0,0,90,
11,7,0,0,0,0,0,0,0,0,0,0,12,8,0,1,
92,0,0,0,23,5,4,8,23,6,5,7,21,6,0,0,
18,0,0,2,18,0,0,9,11,9,0,0,0,0,0,0,
0,0,240,63,1,8,2,9,9,5,1,8,12,8,0,1,
10,0,0,0,23,5,5,8,21,5,0,0,18,0,0,31,
11,6,0,0,0,0,0,0,0,0,0,64,1,5,2,6,
15,2,5,0,12,7,0,1,84,0,0,0,13,6,7,0,
12,7,0,1,121,0,0,0,9,6,6,7,11,7,0,0,
0,0,0,0,0,0,240,63,1,6,6,7,15,5,6,0,
15,6,2,0,12,8,0,1,84,0,0,0,13,7,8,0,
12,8,0,1,121,0,0,0,10,7,8,5,12,7,0,1,
84,0,0,0,13,5,7,0,12,7,0,2,121,105,0,0,
10,5,7,6,18,0,0,40,11,7,0,0,0,0,0,0,
0,0,240,63,12,8,0,1,32,0,0,0,23,5,4,8,
23,6,5,7,21,6,0,0,18,0,0,2,18,0,0,4,
12,8,0,1,9,0,0,0,23,5,4,8,21,5,0,0,
18,0,0,7,11,6,0,0,0,0,0,0,0,0,240,63,
1,5,2,6,15,2,5,0,18,0,0,19,12,7,0,7,
117,95,101,114,114,111,114,0,13,6,7,0,12,7,0,8,
116,111,107,101,110,105,122,101,0,0,0,0,15,8,1,0,
12,10,0,1,84,0,0,0,13,9,10,0,12,10,0,1,
102,0,0,0,9,9,9,10,31,5,7,3,19,5,6,5,
18,0,0,1,18,0,254,202,12,7,0,6,105,110,100,101,
110,116,0,0,13,6,7,0,11,7,0,0,0,0,0,0,
0,0,0,0,31,5,7,1,19,5,6,5,12,7,0,1,
84,0,0,0,13,6,7,0,12,7,0,3,114,101,115,0,
9,6,6,7,15,5,6,0,12,6,0,1,84,0,0,0,
28,7,0,0,14,6,7,0,20,5,0,0,0,0,0,0,
12,11,0,11,100,111,95,116,111,107,101,110,105,122,101,0,
14,11,10,0,16,11,0,75,44,8,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,28,4,0,0,
9,3,0,4,11,4,0,0,0,0,0,0,0,0,0,0,
12,6,0,1,84,0,0,0,13,5,6,0,12,6,0,6,
98,114,97,99,101,115,0,0,9,5,5,6,23,4,4,5,
21,4,0,0,18,0,0,13,12,6,0,1,84,0,0,0,
13,5,6,0,12,6,0,3,97,100,100,0,9,5,5,6,
12,6,0,2,110,108,0,0,28,7,0,0,31,4,6,2,
19,4,5,4,18,0,0,1,11,6,0,0,0,0,0,0,
0,0,240,63,1,5,2,6,15,4,5,0,11,6,0,0,
0,0,0,0,0,0,240,63,15,5,6,0,15,2,4,0,
12,6,0,1,84,0,0,0,13,4,6,0,12,6,0,2,
110,108,0,0,10,4,6,5,12,6,0,1,84,0,0,0,
13,5,6,0,12,6,0,1,121,0,0,0,9,5,5,6,
11,6,0,0,0,0,0,0,0,0,240,63,1,5,5,6,
15,4,5,0,15,5,2,0,12,7,0,1,84,0,0,0,
13,6,7,0,12,7,0,1,121,0,0,0,10,6,7,4,
12,6,0,1,84,0,0,0,13,4,6,0,12,6,0,2,
121,105,0,0,10,4,6,5,20,2,0,0,0,0,0,0,
12,12,0,5,100,111,95,110,108,0,0,0,14,12,11,0,
16,12,0,90,44,12,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,28,4,0,0,9,3,0,4,
11,5,0,0,0,0,0,0,0,0,0,0,15,4,5,0,
25,5,2,3,21,5,0,0,18,0,0,33,9,6,1,2,
15,5,6,0,11,8,0,0,0,0,0,0,0,0,0,0,
12,9,0,1,32,0,0,0,35,6,5,9,23,7,6,8,
21,7,0,0,18,0,0,2,18,0,0,4,12,9,0,1,
9,0,0,0,35,6,5,9,21,6,0,0,18,0,0,3,
18,0,0,15,18,0,0,1,11,8,0,0,0,0,0,0,
0,0,240,63,1,7,2,8,15,6,7,0,11,9,0,0,
0,0,0,0,0,0,240,63,1,8,4,9,15,7,8,0,
15,2,6,0,15,4,7,0,18,0,255,222,11,8,0,0,
0,0,0,0,0,0,0,0,11,10,0,0,0,0,0,0,
0,0,0,0,12,11,0,1,10,0,0,0,35,6,5,11,
23,9,6,10,21,9,0,0,18,0,0,2,18,0,0,4,
12,11,0,1,35,0,0,0,35,6,5,11,23,7,6,8,
21,7,0,0,18,0,0,2,18,0,0,12,11,6,0,0,
0,0,0,0,0,0,0,0,12,10,0,1,84,0,0,0,
13,9,10,0,12,10,0,6,98,114,97,99,101,115,0,0,
9,9,9,10,23,6,6,9,21,6,0,0,18,0,0,9,
12,8,0,6,105,110,100,101,110,116,0,0,13,7,8,0,
15,8,4,0,31,6,8,1,19,6,7,6,18,0,0,1,
20,2,0,0,0,0,0,0,12,13,0,9,100,111,95,105,
110,100,101,110,116,0,0,0,14,13,12,0,16,13,0,137,
44,8,0,0,28,2,0,0,9,1,0,2,12,4,0,1,
84,0,0,0,13,3,4,0,12,4,0,6,105,110,100,101,
110,116,0,0,9,3,3,4,11,4,0,0,0,0,0,0,
0,0,240,191,9,3,3,4,23,2,1,3,21,2,0,0,
18,0,0,3,17,0,0,0,18,0,0,117,12,3,0,1,
84,0,0,0,13,2,3,0,12,3,0,6,105,110,100,101,
110,116,0,0,9,2,2,3,11,3,0,0,0,0,0,0,
0,0,240,191,9,2,2,3,25,2,2,1,21,2,0,0,
18,0,0,28,12,4,0,1,84,0,0,0,13,3,4,0,
12,4,0,6,105,110,100,101,110,116,0,0,9,3,3,4,
12,4,0,6,97,112,112,101,110,100,0,0,9,3,3,4,
15,4,1,0,31,2,4,1,19,2,3,2,12,4,0,1,
84,0,0,0,13,3,4,0,12,4,0,3,97,100,100,0,
9,3,3,4,12,4,0,6,105,110,100,101,110,116,0,0,
15,5,1,0,31,2,4,2,19,2,3,2,18,0,0,76,
12,4,0,1,84,0,0,0,13,3,4,0,12,4,0,6,
105,110,100,101,110,116,0,0,9,3,3,4,11,4,0,0,
0,0,0,0,0,0,240,191,9,3,3,4,25,2,1,3,
21,2,0,0,18,0,0,62,12,5,0,1,84,0,0,0,
13,4,5,0,12,5,0,6,105,110,100,101,110,116,0,0,
9,4,4,5,12,5,0,5,105,110,100,101,120,0,0,0,
9,4,4,5,15,5,1,0,31,3,5,1,19,3,4,3,
15,2,3,0,11,4,0,0,0,0,0,0,0,0,240,63,
1,3,2,4,12,6,0,3,108,101,110,0,13,5,6,0,
12,7,0,1,84,0,0,0,13,6,7,0,12,7,0,6,
105,110,100,101,110,116,0,0,9,6,6,7,31,4,6,1,
19,4,5,4,25,3,3,4,21,3,0,0,18,0,0,27,
12,5,0,1,84,0,0,0,13,4,5,0,12,5,0,6,
105,110,100,101,110,116,0,0,9,4,4,5,12,5,0,3,
112,111,112,0,9,4,4,5,31,3,0,0,19,3,4,3,
15,1,3,0,12,5,0,1,84,0,0,0,13,4,5,0,
12,5,0,3,97,100,100,0,9,4,4,5,12,5,0,6,
100,101,100,101,110,116,0,0,15,6,1,0,31,3,5,2,
19,3,4,3,18,0,255,212,18,0,0,1,0,0,0,0,
12,14,0,6,105,110,100,101,110,116,0,0,14,14,13,0,
16,14,0,161,44,13,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,28,4,0,0,9,3,0,4,
27,5,0,0,15,4,5,0,9,6,1,2,15,5,6,0,
15,6,2,0,11,9,0,0,0,0,0,0,0,0,240,63,
1,8,2,9,15,7,8,0,15,8,5,0,15,5,6,0,
15,2,7,0,12,7,0,7,83,89,77,66,79,76,83,0,
13,6,7,0,36,6,6,8,21,6,0,0,18,0,0,9,
12,9,0,6,97,112,112,101,110,100,0,0,9,7,4,9,
15,9,8,0,31,6,9,1,19,6,7,6,18,0,0,1,
25,6,2,3,21,6,0,0,18,0,0,42,9,7,1,2,
15,6,7,0,11,7,0,0,0,0,0,0,0,0,0,0,
12,10,0,8,73,83,89,77,66,79,76,83,0,0,0,0,
13,9,10,0,36,9,9,6,23,7,7,9,21,7,0,0,
18,0,0,3,18,0,0,27,18,0,0,1,1,9,8,6,
15,7,9,0,11,11,0,0,0,0,0,0,0,0,240,63,
1,10,2,11,15,9,10,0,15,8,7,0,15,2,9,0,
12,9,0,7,83,89,77,66,79,76,83,0,13,7,9,0,
36,7,7,8,21,7,0,0,18,0,0,9,12,10,0,6,
97,112,112,101,110,100,0,0,9,9,4,10,15,10,8,0,
31,7,10,1,19,7,9,7,18,0,0,1,18,0,255,213,
12,10,0,3,112,111,112,0,9,9,4,10,31,7,0,0,
19,7,9,7,15,8,7,0,12,11,0,3,108,101,110,0,
13,10,11,0,15,11,8,0,31,9,11,1,19,9,10,9,
15,7,9,0,1,9,5,7,15,2,9,0,12,11,0,1,
84,0,0,0,13,10,11,0,12,11,0,3,97,100,100,0,
9,10,10,11,12,11,0,6,115,121,109,98,111,108,0,0,
15,12,8,0,31,9,11,2,19,9,10,9,12,10,0,7,
66,95,66,69,71,73,78,0,13,9,10,0,36,9,9,8,
21,9,0,0,18,0,0,20,12,10,0,1,84,0,0,0,
13,9,10,0,12,11,0,1,84,0,0,0,13,10,11,0,
12,11,0,6,98,114,97,99,101,115,0,0,9,10,10,11,
11,11,0,0,0,0,0,0,0,0,240,63,1,10,10,11,
12,11,0,6,98,114,97,99,101,115,0,0,10,9,11,10,
18,0,0,1,12,10,0,5,66,95,69,78,68,0,0,0,
13,9,10,0,36,9,9,8,21,9,0,0,18,0,0,20,
12,10,0,1,84,0,0,0,13,9,10,0,12,11,0,1,
84,0,0,0,13,10,11,0,12,11,0,6,98,114,97,99,
101,115,0,0,9,10,10,11,11,11,0,0,0,0,0,0,
0,0,240,63,2,10,10,11,12,11,0,6,98,114,97,99,
101,115,0,0,10,9,11,10,18,0,0,1,20,2,0,0,
0,0,0,0,12,15,0,9,100,111,95,115,121,109,98,111,
108,0,0,0,14,15,14,0,16,15,0,143,44,14,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
28,4,0,0,9,3,0,4,9,5,1,2,15,4,5,0,
11,7,0,0,0,0,0,0,0,0,240,63,1,6,2,7,
15,5,6,0,9,7,1,2,15,6,7,0,15,7,4,0,
15,2,5,0,15,4,6,0,25,5,2,3,21,5,0,0,
18,0,0,60,9,5,1,2,15,4,5,0,11,8,0,0,
0,0,0,0,0,0,0,0,11,10,0,0,0,0,0,0,
0,0,0,0,11,12,0,0,0,0,0,0,0,0,240,63,
12,13,0,1,48,0,0,0,25,5,4,13,23,11,5,12,
21,11,0,0,18,0,0,2,18,0,0,4,12,5,0,1,
57,0,0,0,25,5,5,4,23,9,5,10,21,9,0,0,
18,0,0,2,18,0,0,14,11,12,0,0,0,0,0,0,
0,0,240,63,12,13,0,1,97,0,0,0,25,5,4,13,
23,11,5,12,21,11,0,0,18,0,0,2,18,0,0,4,
12,5,0,1,102,0,0,0,25,5,5,4,23,6,5,8,
21,6,0,0,18,0,0,2,18,0,0,4,12,9,0,1,
120,0,0,0,35,5,4,9,21,5,0,0,18,0,0,3,
18,0,0,12,18,0,0,1,1,6,7,4,15,5,6,0,
11,9,0,0,0,0,0,0,0,0,240,63,1,8,2,9,
15,6,8,0,15,7,5,0,15,2,6,0,18,0,255,195,
12,6,0,1,46,0,0,0,23,5,4,6,21,5,0,0,
18,0,0,43,1,6,7,4,15,5,6,0,11,9,0,0,
0,0,0,0,0,0,240,63,1,8,2,9,15,6,8,0,
15,7,5,0,15,2,6,0,25,5,2,3,21,5,0,0,
18,0,0,30,9,5,1,2,15,4,5,0,11,8,0,0,
0,0,0,0,0,0,240,63,12,9,0,1,48,0,0,0,
25,5,4,9,23,6,5,8,21,6,0,0,18,0,0,2,
18,0,0,4,12,5,0,1,57,0,0,0,25,5,5,4,
21,5,0,0,18,0,0,3,18,0,0,12,18,0,0,1,
1,6,7,4,15,5,6,0,11,9,0,0,0,0,0,0,
0,0,240,63,1,8,2,9,15,6,8,0,15,7,5,0,
15,2,6,0,18,0,255,225,18,0,0,1,12,8,0,1,
84,0,0,0,13,6,8,0,12,8,0,3,97,100,100,0,
9,6,6,8,12,8,0,6,110,117,109,98,101,114,0,0,
15,9,7,0,31,5,8,2,19,5,6,5,20,2,0,0,
0,0,0,0,12,16,0,9,100,111,95,110,117,109,98,101,
114,0,0,0,14,16,15,0,16,16,0,134,44,16,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
28,4,0,0,9,3,0,4,9,5,1,2,15,4,5,0,
11,7,0,0,0,0,0,0,0,0,240,63,1,6,2,7,
15,5,6,0,15,6,4,0,15,2,5,0,25,4,2,3,
21,4,0,0,18,0,0,80,9,5,1,2,15,4,5,0,
11,8,0,0,0,0,0,0,0,0,0,0,11,10,0,0,
0,0,0,0,0,0,0,0,11,12,0,0,0,0,0,0,
0,0,0,0,11,14,0,0,0,0,0,0,0,0,240,63,
12,15,0,1,97,0,0,0,25,5,4,15,23,13,5,14,
21,13,0,0,18,0,0,2,18,0,0,4,12,5,0,1,
122,0,0,0,25,5,5,4,23,11,5,12,21,11,0,0,
18,0,0,2,18,0,0,14,11,14,0,0,0,0,0,0,
0,0,240,63,12,15,0,1,65,0,0,0,25,5,4,15,
23,13,5,14,21,13,0,0,18,0,0,2,18,0,0,4,
12,5,0,1,90,0,0,0,25,5,5,4,23,9,5,10,
21,9,0,0,18,0,0,2,18,0,0,14,11,12,0,0,
0,0,0,0,0,0,240,63,12,13,0,1,48,0,0,0,
25,5,4,13,23,11,5,12,21,11,0,0,18,0,0,2,
18,0,0,4,12,5,0,1,57,0,0,0,25,5,5,4,
23,7,5,8,21,7,0,0,18,0,0,2,18,0,0,4,
12,9,0,1,95,0,0,0,35,5,4,9,21,5,0,0,
18,0,0,3,18,0,0,12,18,0,0,1,1,7,6,4,
15,5,7,0,11,9,0,0,0,0,0,0,0,0,240,63,
1,8,2,9,15,7,8,0,15,6,5,0,15,2,7,0,
18,0,255,175,12,7,0,7,83,89,77,66,79,76,83,0,
13,5,7,0,36,5,5,6,21,5,0,0,18,0,0,14,
12,8,0,1,84,0,0,0,13,7,8,0,12,8,0,3,
97,100,100,0,9,7,7,8,12,8,0,6,115,121,109,98,
111,108,0,0,15,9,6,0,31,5,8,2,19,5,7,5,
18,0,0,14,12,8,0,1,84,0,0,0,13,7,8,0,
12,8,0,3,97,100,100,0,9,7,7,8,12,8,0,4,
110,97,109,101,0,0,0,0,15,9,6,0,31,5,8,2,
19,5,7,5,18,0,0,1,20,2,0,0,0,0,0,0,
12,17,0,7,100,111,95,110,97,109,101,0,14,17,16,0,
16,17,1,3,44,14,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,28,4,0,0,9,3,0,4,
12,5,0,0,0,0,0,0,15,4,5,0,9,6,1,2,
15,5,6,0,11,8,0,0,0,0,0,0,0,0,240,63,
1,7,2,8,15,6,7,0,15,7,4,0,15,4,5,0,
15,2,6,0,11,8,0,0,0,0,0,0,0,0,0,0,
11,10,0,0,0,0,0,0,0,0,0,0,11,5,0,0,
0,0,0,0,0,0,20,64,2,11,3,2,24,5,5,11,
23,9,5,10,21,9,0,0,18,0,0,2,18,0,0,3,
9,5,1,2,23,5,5,4,23,6,5,8,21,6,0,0,
18,0,0,2,18,0,0,7,11,10,0,0,0,0,0,0,
0,0,240,63,1,9,2,10,9,5,1,9,23,5,5,4,
21,5,0,0,18,0,0,105,11,6,0,0,0,0,0,0,
0,0,0,64,1,5,2,6,15,2,5,0,11,8,0,0,
0,0,0,0,0,0,0,64,2,6,3,8,25,5,2,6,
21,5,0,0,18,0,0,92,9,6,1,2,15,5,6,0,
11,9,0,0,0,0,0,0,0,0,0,0,11,11,0,0,
0,0,0,0,0,0,0,0,23,6,5,4,23,10,6,11,
21,10,0,0,18,0,0,2,18,0,0,7,11,13,0,0,
0,0,0,0,0,0,240,63,1,12,2,13,9,6,1,12,
23,6,6,4,23,8,6,9,21,8,0,0,18,0,0,2,
18,0,0,7,11,11,0,0,0,0,0,0,0,0,0,64,
1,10,2,11,9,6,1,10,23,6,6,4,21,6,0,0,
18,0,0,20,11,8,0,0,0,0,0,0,0,0,8,64,
1,6,2,8,15,2,6,0,12,9,0,1,84,0,0,0,
13,8,9,0,12,9,0,3,97,100,100,0,9,8,8,9,
12,9,0,6,115,116,114,105,110,103,0,0,15,10,7,0,
31,6,9,2,19,6,8,6,18,0,0,43,18,0,0,41,
1,8,7,5,15,6,8,0,11,10,0,0,0,0,0,0,
0,0,240,63,1,9,2,10,15,8,9,0,15,7,6,0,
15,2,8,0,12,8,0,1,10,0,0,0,23,6,5,8,
21,6,0,0,18,0,0,26,12,9,0,1,84,0,0,0,
13,8,9,0,12,9,0,1,121,0,0,0,9,8,8,9,
11,9,0,0,0,0,0,0,0,0,240,63,1,8,8,9,
15,6,8,0,15,8,2,0,12,10,0,1,84,0,0,0,
13,9,10,0,12,10,0,1,121,0,0,0,10,9,10,6,
12,9,0,1,84,0,0,0,13,6,9,0,12,9,0,2,
121,105,0,0,10,6,9,8,18,0,0,1,18,0,0,1,
18,0,255,159,18,0,0,104,25,6,2,3,21,6,0,0,
18,0,0,100,9,6,1,2,15,5,6,0,12,8,0,1,
92,0,0,0,23,6,5,8,21,6,0,0,18,0,0,60,
11,8,0,0,0,0,0,0,0,0,240,63,1,6,2,8,
15,2,6,0,9,6,1,2,15,5,6,0,12,8,0,1,
110,0,0,0,23,6,5,8,21,6,0,0,18,0,0,5,
12,6,0,1,10,0,0,0,15,5,6,0,18,0,0,1,
12,8,0,1,114,0,0,0,23,6,5,8,21,6,0,0,
18,0,0,11,12,9,0,3,99,104,114,0,13,8,9,0,
11,9,0,0,0,0,0,0,0,0,42,64,31,6,9,1,
19,6,8,6,15,5,6,0,18,0,0,1,12,8,0,1,
116,0,0,0,23,6,5,8,21,6,0,0,18,0,0,5,
12,6,0,1,9,0,0,0,15,5,6,0,18,0,0,1,
12,8,0,1,48,0,0,0,23,6,5,8,21,6,0,0,
18,0,0,5,12,6,0,1,0,0,0,0,15,5,6,0,
18,0,0,1,1,8,7,5,15,6,8,0,11,10,0,0,
0,0,0,0,0,0,240,63,1,9,2,10,15,8,9,0,
15,7,6,0,15,2,8,0,18,0,0,33,23,6,5,4,
21,6,0,0,18,0,0,20,11,8,0,0,0,0,0,0,
0,0,240,63,1,6,2,8,15,2,6,0,12,9,0,1,
84,0,0,0,13,8,9,0,12,9,0,3,97,100,100,0,
9,8,8,9,12,9,0,6,115,116,114,105,110,103,0,0,
15,10,7,0,31,6,9,2,19,6,8,6,18,0,0,13,
18,0,0,11,1,8,7,5,15,6,8,0,11,10,0,0,
0,0,0,0,0,0,240,63,1,9,2,10,15,8,9,0,
15,7,6,0,15,2,8,0,18,0,0,1,18,0,255,155,
18,0,0,1,20,2,0,0,0,0,0,0,12,18,0,9,
100,111,95,115,116,114,105,110,103,0,0,0,14,18,17,0,
16,18,0,33,44,7,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,28,4,0,0,9,3,0,4,
11,5,0,0,0,0,0,0,0,0,240,63,1,4,2,5,
15,2,4,0,25,4,2,3,21,4,0,0,18,0,0,16,
9,5,1,2,15,4,5,0,12,6,0,1,10,0,0,0,
23,5,4,6,21,5,0,0,18,0,0,3,18,0,0,8,
18,0,0,1,11,6,0,0,0,0,0,0,0,0,240,63,
1,5,2,6,15,2,5,0,18,0,255,239,20,2,0,0,
0,0,0,0,12,19,0,10,100,111,95,99,111,109,109,101,
110,116,0,0,14,19,18,0,0,0,0,0,
};
unsigned char tp_parse[] = {
44,114,0,0,12,2,0,6,105,109,112,111,114,116,0,0,
13,1,2,0,12,2,0,8,116,111,107,101,110,105,122,101,
0,0,0,0,31,0,2,1,19,0,1,0,12,1,0,8,
116,111,107,101,110,105,122,101,0,0,0,0,14,1,0,0,
12,2,0,6,105,109,112,111,114,116,0,0,13,1,2,0,
12,2,0,8,116,111,107,101,110,105,122,101,0,0,0,0,
31,0,2,1,19,0,1,0,12,2,0,8,95,95,100,105,
99,116,95,95,0,0,0,0,13,1,2,0,12,3,0,5,
84,111,107,101,110,0,0,0,9,2,0,3,12,0,0,5,
84,111,107,101,110,0,0,0,10,1,0,2,12,2,0,3,
115,116,114,0,13,1,2,0,11,2,0,0,0,0,0,0,
0,0,240,63,31,0,2,1,19,0,1,0,12,1,0,1,
46,0,0,0,36,0,0,1,21,0,0,0,18,0,0,23,
12,2,0,6,105,109,112,111,114,116,0,0,13,1,2,0,
12,2,0,4,98,111,111,116,0,0,0,0,31,0,2,1,
19,0,1,0,12,3,0,5,109,101,114,103,101,0,0,0,
13,2,3,0,12,5,0,8,95,95,100,105,99,116,95,95,
0,0,0,0,13,3,5,0,15,4,0,0,31,1,3,2,
19,1,2,1,18,0,0,1,16,0,0,63,44,8,0,0,
28,2,0,0,9,1,0,2,12,3,0,1,42,0,0,0,
9,2,0,3,11,4,0,0,0,0,0,0,0,0,0,0,
9,3,2,4,28,4,0,0,23,3,3,4,21,3,0,0,
18,0,0,6,11,3,0,0,0,0,0,0,0,0,240,63,
20,3,0,0,18,0,0,1,12,5,0,4,116,121,112,101,
0,0,0,0,9,4,1,5,36,3,2,4,21,3,0,0,
18,0,0,6,11,3,0,0,0,0,0,0,0,0,240,63,
20,3,0,0,18,0,0,1,11,5,0,0,0,0,0,0,
0,0,0,0,12,6,0,4,116,121,112,101,0,0,0,0,
9,3,1,6,12,6,0,6,115,121,109,98,111,108,0,0,
23,3,3,6,23,4,3,5,21,4,0,0,18,0,0,2,
18,0,0,5,12,7,0,3,118,97,108,0,9,6,1,7,
36,3,2,6,21,3,0,0,18,0,0,6,11,3,0,0,
0,0,0,0,0,0,240,63,20,3,0,0,18,0,0,1,
11,3,0,0,0,0,0,0,0,0,0,0,20,3,0,0,
0,0,0,0,12,1,0,5,99,104,101,99,107,0,0,0,
14,1,0,0,16,1,0,58,44,10,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,12,5,0,1,
80,0,0,0,13,4,5,0,12,5,0,5,115,116,97,99,
107,0,0,0,9,4,4,5,12,5,0,6,97,112,112,101,
110,100,0,0,9,4,4,5,15,6,1,0,12,8,0,4,
100,109,97,112,0,0,0,0,13,7,8,0,9,7,7,1,
27,5,6,2,31,3,5,1,19,3,4,3,21,2,0,0,
18,0,0,12,12,4,0,4,100,109,97,112,0,0,0,0,
13,3,4,0,12,5,0,4,111,109,97,112,0,0,0,0,
13,4,5,0,9,4,4,1,10,3,1,4,18,0,0,19,
12,4,0,4,100,109,97,112,0,0,0,0,13,3,4,0,
12,5,0,3,108,98,112,0,11,6,0,0,0,0,0,0,
0,0,0,0,12,7,0,3,110,117,100,0,12,9,0,6,
105,116,115,101,108,102,0,0,13,8,9,0,26,4,5,4,
10,3,1,4,18,0,0,1,0,0,0,0,12,2,0,5,
116,119,101,97,107,0,0,0,14,2,1,0,16,2,0,30,
44,6,0,0,12,3,0,1,80,0,0,0,13,2,3,0,
12,3,0,5,115,116,97,99,107,0,0,0,9,2,2,3,
12,3,0,3,112,111,112,0,9,2,2,3,31,1,0,0,
19,1,2,1,11,4,0,0,0,0,0,0,0,0,0,0,
9,3,1,4,15,2,3,0,11,5,0,0,0,0,0,0,
0,0,240,63,9,4,1,5,15,3,4,0,12,4,0,4,
100,109,97,112,0,0,0,0,13,1,4,0,10,1,2,3,
0,0,0,0,12,3,0,7,114,101,115,116,111,114,101,0,
14,3,2,0,16,3,0,16,44,6,0,0,28,2,0,0,
9,1,0,2,26,3,0,0,15,2,3,0,11,4,0,0,
0,0,0,0,0,0,0,0,42,3,1,4,18,0,0,4,
9,5,1,3,10,2,3,5,18,0,255,252,20,2,0,0,
0,0,0,0,12,4,0,3,99,112,121,0,14,4,3,0,
26,4,0,0,12,5,0,5,80,68,97,116,97,0,0,0,
14,5,4,0,16,5,0,32,44,6,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,28,4,0,0,
9,3,0,4,12,4,0,1,115,0,0,0,10,1,4,2,
12,4,0,6,116,111,107,101,110,115,0,0,10,1,4,3,
11,4,0,0,0,0,0,0,0,0,0,0,12,5,0,3,
112,111,115,0,10,1,5,4,28,4,0,0,12,5,0,5,
116,111,107,101,110,0,0,0,10,1,5,4,27,4,0,0,
12,5,0,5,115,116,97,99,107,0,0,0,10,1,5,4,
0,0,0,0,12,6,0,8,95,95,105,110,105,116,95,95,
0,0,0,0,10,4,6,5,16,6,0,39,44,7,0,0,
28,2,0,0,9,1,0,2,12,2,0,4,111,109,97,112,
0,0,0,0,12,5,0,3,99,112,121,0,13,4,5,0,
12,6,0,9,98,97,115,101,95,100,109,97,112,0,0,0,
13,5,6,0,31,3,5,1,19,3,4,3,14,2,3,0,
12,2,0,4,100,109,97,112,0,0,0,0,12,5,0,3,
99,112,121,0,13,4,5,0,12,6,0,9,98,97,115,101,
95,100,109,97,112,0,0,0,13,5,6,0,31,3,5,1,
19,3,4,3,14,2,3,0,12,4,0,7,97,100,118,97,
110,99,101,0,9,3,1,4,31,2,0,0,19,2,3,2,
0,0,0,0,12,7,0,4,105,110,105,116,0,0,0,0,
10,4,7,6,16,7,0,106,44,11,0,0,28,2,0,0,
9,1,0,2,28,2,0,0,28,3,0,0,32,2,0,3,
11,3,0,0,0,0,0,0,0,0,0,0,12,6,0,5,
99,104,101,99,107,0,0,0,13,5,6,0,12,8,0,5,
116,111,107,101,110,0,0,0,9,6,1,8,15,7,2,0,
31,4,6,2,19,4,5,4,23,3,3,4,21,3,0,0,
18,0,0,17,12,5,0,5,101,114,114,111,114,0,0,0,
13,4,5,0,12,5,0,9,101,120,112,101,99,116,101,100,
32,0,0,0,1,5,5,2,12,7,0,5,116,111,107,101,
110,0,0,0,9,6,1,7,31,3,5,2,19,3,4,3,
18,0,0,1,12,4,0,3,112,111,115,0,9,3,1,4,
12,6,0,3,108,101,110,0,13,5,6,0,12,7,0,6,
116,111,107,101,110,115,0,0,9,6,1,7,31,4,6,1,
19,4,5,4,25,3,3,4,21,3,0,0,18,0,0,21,
12,5,0,6,116,111,107,101,110,115,0,0,9,4,1,5,
12,6,0,3,112,111,115,0,9,5,1,6,9,4,4,5,
15,3,4,0,12,5,0,3,112,111,115,0,9,4,1,5,
11,5,0,0,0,0,0,0,0,0,240,63,1,4,4,5,
12,5,0,3,112,111,115,0,10,1,5,4,18,0,0,20,
12,6,0,5,84,111,107,101,110,0,0,0,13,5,6,0,
11,9,0,0,0,0,0,0,0,0,0,0,11,10,0,0,
0,0,0,0,0,0,0,0,27,6,9,2,12,7,0,3,
101,111,102,0,12,8,0,3,101,111,102,0,31,4,6,3,
19,4,5,4,15,3,4,0,18,0,0,1,12,6,0,2,
100,111,0,0,13,5,6,0,15,6,3,0,31,4,6,1,
19,4,5,4,12,5,0,5,116,111,107,101,110,0,0,0,
10,1,5,4,20,3,0,0,0,0,0,0,12,8,0,7,
97,100,118,97,110,99,101,0,10,4,8,7,16,8,0,64,
44,7,0,0,28,2,0,0,9,1,0,2,12,4,0,4,
98,105,110,100,0,0,0,0,13,3,4,0,12,6,0,5,
80,68,97,116,97,0,0,0,13,4,6,0,12,6,0,8,
95,95,105,110,105,116,95,95,0,0,0,0,9,4,4,6,
15,5,1,0,31,2,4,2,19,2,3,2,12,3,0,8,
95,95,105,110,105,116,95,95,0,0,0,0,10,1,3,2,
12,4,0,4,98,105,110,100,0,0,0,0,13,3,4,0,
12,6,0,5,80,68,97,116,97,0,0,0,13,4,6,0,
12,6,0,4,105,110,105,116,0,0,0,0,9,4,4,6,
15,5,1,0,31,2,4,2,19,2,3,2,12,3,0,4,
105,110,105,116,0,0,0,0,10,1,3,2,12,4,0,4,
98,105,110,100,0,0,0,0,13,3,4,0,12,6,0,5,
80,68,97,116,97,0,0,0,13,4,6,0,12,6,0,7,
97,100,118,97,110,99,101,0,9,4,4,6,15,5,1,0,
31,2,4,2,19,2,3,2,12,3,0,7,97,100,118,97,
110,99,101,0,10,1,3,2,0,0,0,0,12,9,0,7,
95,95,110,101,119,95,95,0,10,4,9,8,16,9,0,22,
44,5,0,0,26,1,0,0,12,4,0,5,80,68,97,116,
97,0,0,0,13,3,4,0,12,4,0,7,95,95,110,101,
119,95,95,0,9,3,3,4,15,4,1,0,31,2,4,1,
19,2,3,2,12,3,0,8,95,95,105,110,105,116,95,95,
0,0,0,0,9,2,1,3,19,4,2,0,20,1,0,0,
0,0,0,0,12,10,0,8,95,95,99,97,108,108,95,95,
0,0,0,0,10,4,10,9,16,4,0,32,44,9,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
12,4,0,5,112,114,105,110,116,0,0,0,13,3,4,0,
12,5,0,8,116,111,107,101,110,105,122,101,0,0,0,0,
13,4,5,0,12,5,0,7,117,95,101,114,114,111,114,0,
9,4,4,5,15,5,1,0,12,8,0,1,80,0,0,0,
13,6,8,0,12,8,0,1,115,0,0,0,9,6,6,8,
12,8,0,3,112,111,115,0,9,7,2,8,31,3,5,3,
19,3,4,3,0,0,0,0,12,10,0,5,101,114,114,111,
114,0,0,0,14,10,4,0,16,10,0,12,44,5,0,0,
28,2,0,0,9,1,0,2,12,4,0,3,110,117,100,0,
9,3,1,4,15,4,1,0,31,2,4,1,19,2,3,2,
20,2,0,0,0,0,0,0,12,11,0,3,110,117,100,0,
14,11,10,0,16,11,0,15,44,7,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,12,5,0,3,
108,101,100,0,9,4,1,5,15,5,1,0,15,6,2,0,
31,3,5,2,19,3,4,3,20,3,0,0,0,0,0,0,
12,12,0,3,108,101,100,0,14,12,11,0,16,12,0,9,
44,4,0,0,28,2,0,0,9,1,0,2,12,3,0,3,
108,98,112,0,9,2,1,3,20,2,0,0,0,0,0,0,
12,13,0,7,103,101,116,95,108,98,112,0,14,13,12,0,
16,13,0,10,44,4,0,0,28,2,0,0,9,1,0,2,
12,3,0,5,105,116,101,109,115,0,0,0,9,2,1,3,
20,2,0,0,0,0,0,0,12,14,0,9,103,101,116,95,
105,116,101,109,115,0,0,0,14,14,13,0,16,14,0,66,
44,9,0,0,28,2,0,0,9,1,0,2,12,4,0,1,
80,0,0,0,13,3,4,0,12,4,0,5,116,111,107,101,
110,0,0,0,9,3,3,4,15,2,3,0,12,5,0,7,
97,100,118,97,110,99,101,0,13,4,5,0,31,3,0,0,
19,3,4,3,12,6,0,3,110,117,100,0,13,5,6,0,
15,6,2,0,31,4,6,1,19,4,5,4,15,3,4,0,
12,7,0,7,103,101,116,95,108,98,112,0,13,6,7,0,
12,8,0,1,80,0,0,0,13,7,8,0,12,8,0,5,
116,111,107,101,110,0,0,0,9,7,7,8,31,5,7,1,
19,5,6,5,25,4,1,5,21,4,0,0,18,0,0,24,
12,5,0,1,80,0,0,0,13,4,5,0,12,5,0,5,
116,111,107,101,110,0,0,0,9,4,4,5,15,2,4,0,
12,6,0,7,97,100,118,97,110,99,101,0,13,5,6,0,
31,4,0,0,19,4,5,4,12,6,0,3,108,101,100,0,
13,5,6,0,15,6,2,0,15,7,3,0,31,4,6,2,
19,4,5,4,15,3,4,0,18,0,255,218,20,3,0,0,
0,0,0,0,12,15,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,14,15,14,0,16,15,0,24,44,9,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
15,4,2,0,12,7,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,6,7,0,12,8,0,2,98,112,0,0,
9,7,1,8,31,5,7,1,19,5,6,5,27,3,4,2,
12,4,0,5,105,116,101,109,115,0,0,0,10,1,4,3,
20,1,0,0,0,0,0,0,12,16,0,9,105,110,102,105,
120,95,108,101,100,0,0,0,14,16,15,0,16,16,0,56,
44,9,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,12,5,0,5,99,104,101,99,107,0,0,0,
13,4,5,0,12,7,0,1,80,0,0,0,13,5,7,0,
12,7,0,5,116,111,107,101,110,0,0,0,9,5,5,7,
12,6,0,3,110,111,116,0,31,3,5,2,19,3,4,3,
21,3,0,0,18,0,0,16,12,3,0,5,105,115,110,111,
116,0,0,0,12,4,0,3,118,97,108,0,10,1,4,3,
12,5,0,7,97,100,118,97,110,99,101,0,13,4,5,0,
12,5,0,3,110,111,116,0,31,3,5,1,19,3,4,3,
18,0,0,1,15,4,2,0,12,7,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,6,7,0,12,8,0,2,
98,112,0,0,9,7,1,8,31,5,7,1,19,5,6,5,
27,3,4,2,12,4,0,5,105,116,101,109,115,0,0,0,
10,1,4,3,20,1,0,0,0,0,0,0,12,17,0,8,
105,110,102,105,120,95,105,115,0,0,0,0,14,17,16,0,
16,17,0,38,44,9,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,12,5,0,7,97,100,118,97,
110,99,101,0,13,4,5,0,12,5,0,2,105,110,0,0,
31,3,5,1,19,3,4,3,12,3,0,5,110,111,116,105,
110,0,0,0,12,4,0,3,118,97,108,0,10,1,4,3,
15,4,2,0,12,7,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,6,7,0,12,8,0,2,98,112,0,0,
9,7,1,8,31,5,7,1,19,5,6,5,27,3,4,2,
12,4,0,5,105,116,101,109,115,0,0,0,10,1,4,3,
20,1,0,0,0,0,0,0,12,18,0,9,105,110,102,105,
120,95,110,111,116,0,0,0,14,18,17,0,16,18,0,54,
44,8,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,12,6,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,5,6,0,12,7,0,2,98,112,0,0,
9,6,1,7,31,4,6,1,19,4,5,4,15,3,4,0,
12,5,0,3,118,97,108,0,9,4,2,5,12,5,0,1,
44,0,0,0,23,4,4,5,21,4,0,0,18,0,0,14,
12,6,0,5,105,116,101,109,115,0,0,0,9,5,2,6,
12,6,0,6,97,112,112,101,110,100,0,0,9,5,5,6,
15,6,3,0,31,4,6,1,19,4,5,4,20,2,0,0,
18,0,0,1,15,5,2,0,15,6,3,0,27,4,5,2,
12,5,0,5,105,116,101,109,115,0,0,0,10,1,5,4,
12,4,0,5,116,117,112,108,101,0,0,0,12,5,0,4,
116,121,112,101,0,0,0,0,10,1,5,4,20,1,0,0,
0,0,0,0,12,19,0,11,105,110,102,105,120,95,116,117,
112,108,101,0,14,19,18,0,16,19,0,43,44,8,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,23,2,1,3,
21,2,0,0,18,0,0,4,27,2,0,0,20,2,0,0,
18,0,0,1,12,4,0,5,99,104,101,99,107,0,0,0,
13,3,4,0,15,4,1,0,12,5,0,1,44,0,0,0,
12,6,0,5,116,117,112,108,101,0,0,0,12,7,0,10,
115,116,97,116,101,109,101,110,116,115,0,0,31,2,4,4,
19,2,3,2,21,2,0,0,18,0,0,11,12,4,0,9,
103,101,116,95,105,116,101,109,115,0,0,0,13,3,4,0,
15,4,1,0,31,2,4,1,19,2,3,2,20,2,0,0,
18,0,0,1,15,3,1,0,27,2,3,1,20,2,0,0,
0,0,0,0,12,20,0,3,108,115,116,0,14,20,19,0,
16,20,0,25,44,11,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,12,5,0,5,84,111,107,101,
110,0,0,0,13,4,5,0,12,9,0,3,112,111,115,0,
9,5,2,9,15,6,1,0,15,7,1,0,12,10,0,3,
108,115,116,0,13,9,10,0,15,10,2,0,31,8,10,1,
19,8,9,8,31,3,5,4,19,3,4,3,20,3,0,0,
0,0,0,0,12,21,0,4,105,108,115,116,0,0,0,0,
14,21,20,0,16,21,0,116,44,11,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,12,6,0,5,
84,111,107,101,110,0,0,0,13,5,6,0,12,10,0,3,
112,111,115,0,9,6,1,10,12,7,0,4,99,97,108,108,
0,0,0,0,12,8,0,1,36,0,0,0,15,10,2,0,
27,9,10,1,31,4,6,4,19,4,5,4,15,3,4,0,
11,4,0,0,0,0,0,0,0,0,0,0,12,7,0,5,
99,104,101,99,107,0,0,0,13,6,7,0,12,9,0,1,
80,0,0,0,13,7,9,0,12,9,0,5,116,111,107,101,
110,0,0,0,9,7,7,9,12,8,0,1,41,0,0,0,
31,5,7,2,19,5,6,5,23,4,4,5,21,4,0,0,
18,0,0,63,12,6,0,5,116,119,101,97,107,0,0,0,
13,5,6,0,12,6,0,1,44,0,0,0,11,7,0,0,
0,0,0,0,0,0,0,0,31,4,6,2,19,4,5,4,
12,6,0,5,105,116,101,109,115,0,0,0,9,5,3,6,
12,6,0,6,97,112,112,101,110,100,0,0,9,5,5,6,
12,8,0,10,101,120,112,114,101,115,115,105,111,110,0,0,
13,7,8,0,11,8,0,0,0,0,0,0,0,0,0,0,
31,6,8,1,19,6,7,6,31,4,6,1,19,4,5,4,
12,5,0,1,80,0,0,0,13,4,5,0,12,5,0,5,
116,111,107,101,110,0,0,0,9,4,4,5,12,5,0,3,
118,97,108,0,9,4,4,5,12,5,0,1,44,0,0,0,
23,4,4,5,21,4,0,0,18,0,0,10,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,1,
44,0,0,0,31,4,6,1,19,4,5,4,18,0,0,1,
12,6,0,7,114,101,115,116,111,114,101,0,13,5,6,0,
31,4,0,0,19,4,5,4,18,0,255,174,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,1,
41,0,0,0,31,4,6,1,19,4,5,4,20,3,0,0,
0,0,0,0,12,22,0,8,99,97,108,108,95,108,101,100,
0,0,0,0,14,22,21,0,16,22,0,229,44,17,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
12,6,0,5,84,111,107,101,110,0,0,0,13,5,6,0,
12,10,0,3,112,111,115,0,9,6,1,10,12,7,0,3,
103,101,116,0,12,8,0,1,46,0,0,0,15,10,2,0,
27,9,10,1,31,4,6,4,19,4,5,4,15,3,4,0,
15,6,2,0,27,5,6,1,15,4,5,0,11,6,0,0,
0,0,0,0,0,0,0,0,15,5,6,0,11,6,0,0,
0,0,0,0,0,0,0,0,12,9,0,5,99,104,101,99,
107,0,0,0,13,8,9,0,12,11,0,1,80,0,0,0,
13,9,11,0,12,11,0,5,116,111,107,101,110,0,0,0,
9,9,9,11,12,10,0,1,93,0,0,0,31,7,9,2,
19,7,8,7,23,6,6,7,21,6,0,0,18,0,0,99,
11,6,0,0,0,0,0,0,0,0,0,0,15,5,6,0,
12,8,0,5,99,104,101,99,107,0,0,0,13,7,8,0,
12,10,0,1,80,0,0,0,13,8,10,0,12,10,0,5,
116,111,107,101,110,0,0,0,9,8,8,10,12,9,0,1,
58,0,0,0,31,6,8,2,19,6,7,6,21,6,0,0,
18,0,0,30,12,8,0,6,97,112,112,101,110,100,0,0,
9,7,4,8,12,10,0,5,84,111,107,101,110,0,0,0,
13,9,10,0,12,13,0,1,80,0,0,0,13,10,13,0,
12,13,0,5,116,111,107,101,110,0,0,0,9,10,10,13,
12,13,0,3,112,111,115,0,9,10,10,13,12,11,0,6,
115,121,109,98,111,108,0,0,12,12,0,4,78,111,110,101,
0,0,0,0,31,8,10,3,19,8,9,8,31,6,8,1,
19,6,7,6,18,0,0,18,12,8,0,6,97,112,112,101,
110,100,0,0,9,7,4,8,12,10,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,9,10,0,11,10,0,0,
0,0,0,0,0,0,0,0,31,8,10,1,19,8,9,8,
31,6,8,1,19,6,7,6,18,0,0,1,12,8,0,5,
99,104,101,99,107,0,0,0,13,7,8,0,12,10,0,1,
80,0,0,0,13,8,10,0,12,10,0,5,116,111,107,101,
110,0,0,0,9,8,8,10,12,9,0,1,58,0,0,0,
31,6,8,2,19,6,7,6,21,6,0,0,18,0,0,14,
12,8,0,7,97,100,118,97,110,99,101,0,13,7,8,0,
12,8,0,1,58,0,0,0,31,6,8,1,19,6,7,6,
11,6,0,0,0,0,0,0,0,0,240,63,15,5,6,0,
18,0,0,1,18,0,255,138,21,5,0,0,18,0,0,30,
12,8,0,6,97,112,112,101,110,100,0,0,9,7,4,8,
12,10,0,5,84,111,107,101,110,0,0,0,13,9,10,0,
12,13,0,1,80,0,0,0,13,10,13,0,12,13,0,5,
116,111,107,101,110,0,0,0,9,10,10,13,12,13,0,3,
112,111,115,0,9,10,10,13,12,11,0,6,115,121,109,98,
111,108,0,0,12,12,0,4,78,111,110,101,0,0,0,0,
31,8,10,3,19,8,9,8,31,6,8,1,19,6,7,6,
18,0,0,1,11,6,0,0,0,0,0,0,0,0,0,64,
12,9,0,3,108,101,110,0,13,8,9,0,15,9,4,0,
31,7,9,1,19,7,8,7,25,6,6,7,21,6,0,0,
18,0,0,25,15,7,2,0,12,10,0,5,84,111,107,101,
110,0,0,0,13,9,10,0,12,14,0,3,112,111,115,0,
9,10,1,14,12,11,0,5,115,108,105,99,101,0,0,0,
12,12,0,1,58,0,0,0,11,15,0,0,0,0,0,0,
0,0,240,63,28,16,0,0,27,14,15,2,9,13,4,14,
31,8,10,4,19,8,9,8,27,6,7,2,15,4,6,0,
18,0,0,1,12,6,0,5,105,116,101,109,115,0,0,0,
10,3,6,4,12,8,0,7,97,100,118,97,110,99,101,0,
13,7,8,0,12,8,0,1,93,0,0,0,31,6,8,1,
19,6,7,6,20,3,0,0,0,0,0,0,12,23,0,7,
103,101,116,95,108,101,100,0,14,23,22,0,16,23,0,33,
44,8,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,12,6,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,5,6,0,12,7,0,2,98,112,0,0,
9,6,1,7,31,4,6,1,19,4,5,4,15,3,4,0,
12,4,0,6,115,116,114,105,110,103,0,0,12,5,0,4,
116,121,112,101,0,0,0,0,10,3,5,4,15,5,2,0,
15,6,3,0,27,4,5,2,12,5,0,5,105,116,101,109,
115,0,0,0,10,1,5,4,20,1,0,0,0,0,0,0,
12,24,0,7,100,111,116,95,108,101,100,0,14,24,23,0,
16,24,0,6,44,3,0,0,28,2,0,0,9,1,0,2,
20,1,0,0,0,0,0,0,12,25,0,6,105,116,115,101,
108,102,0,0,14,25,24,0,16,25,0,42,44,6,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,116,119,101,97,
107,0,0,0,13,3,4,0,12,4,0,1,44,0,0,0,
11,5,0,0,0,0,0,0,0,0,240,63,31,2,4,2,
19,2,3,2,12,5,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,4,5,0,11,5,0,0,0,0,0,0,
0,0,0,0,31,3,5,1,19,3,4,3,15,2,3,0,
12,5,0,7,114,101,115,116,111,114,101,0,13,4,5,0,
31,3,0,0,19,3,4,3,12,5,0,7,97,100,118,97,
110,99,101,0,13,4,5,0,12,5,0,1,41,0,0,0,
31,3,5,1,19,3,4,3,20,2,0,0,0,0,0,0,
12,26,0,9,112,97,114,101,110,95,110,117,100,0,0,0,
14,26,25,0,16,26,0,224,44,10,0,0,28,2,0,0,
9,1,0,2,12,2,0,4,108,105,115,116,0,0,0,0,
12,3,0,4,116,121,112,101,0,0,0,0,10,1,3,2,
12,2,0,2,91,93,0,0,12,3,0,3,118,97,108,0,
10,1,3,2,27,2,0,0,12,3,0,5,105,116,101,109,
115,0,0,0,10,1,3,2,12,4,0,1,80,0,0,0,
13,3,4,0,12,4,0,5,116,111,107,101,110,0,0,0,
9,3,3,4,15,2,3,0,12,5,0,5,116,119,101,97,
107,0,0,0,13,4,5,0,12,5,0,1,44,0,0,0,
11,6,0,0,0,0,0,0,0,0,0,0,31,3,5,2,
19,3,4,3,11,3,0,0,0,0,0,0,0,0,0,0,
12,6,0,5,99,104,101,99,107,0,0,0,13,5,6,0,
12,9,0,1,80,0,0,0,13,6,9,0,12,9,0,5,
116,111,107,101,110,0,0,0,9,6,6,9,12,7,0,3,
102,111,114,0,12,8,0,1,93,0,0,0,31,4,6,3,
19,4,5,4,23,3,3,4,21,3,0,0,18,0,0,48,
12,6,0,10,101,120,112,114,101,115,115,105,111,110,0,0,
13,5,6,0,11,6,0,0,0,0,0,0,0,0,0,0,
31,4,6,1,19,4,5,4,15,3,4,0,12,6,0,5,
105,116,101,109,115,0,0,0,9,5,1,6,12,6,0,6,
97,112,112,101,110,100,0,0,9,5,5,6,15,6,3,0,
31,4,6,1,19,4,5,4,12,5,0,1,80,0,0,0,
13,4,5,0,12,5,0,5,116,111,107,101,110,0,0,0,
9,4,4,5,12,5,0,3,118,97,108,0,9,4,4,5,
12,5,0,1,44,0,0,0,23,4,4,5,21,4,0,0,
18,0,0,10,12,6,0,7,97,100,118,97,110,99,101,0,
13,5,6,0,12,6,0,1,44,0,0,0,31,4,6,1,
19,4,5,4,18,0,0,1,18,0,255,187,12,6,0,5,
99,104,101,99,107,0,0,0,13,5,6,0,12,8,0,1,
80,0,0,0,13,6,8,0,12,8,0,5,116,111,107,101,
110,0,0,0,9,6,6,8,12,7,0,3,102,111,114,0,
31,4,6,2,19,4,5,4,21,4,0,0,18,0,0,82,
12,4,0,4,99,111,109,112,0,0,0,0,12,5,0,4,
116,121,112,101,0,0,0,0,10,1,5,4,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,3,
102,111,114,0,31,4,6,1,19,4,5,4,12,6,0,5,
116,119,101,97,107,0,0,0,13,5,6,0,12,6,0,2,
105,110,0,0,11,7,0,0,0,0,0,0,0,0,0,0,
31,4,6,2,19,4,5,4,12,6,0,5,105,116,101,109,
115,0,0,0,9,5,1,6,12,6,0,6,97,112,112,101,
110,100,0,0,9,5,5,6,12,8,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,7,8,0,11,8,0,0,
0,0,0,0,0,0,0,0,31,6,8,1,19,6,7,6,
31,4,6,1,19,4,5,4,12,6,0,7,97,100,118,97,
110,99,101,0,13,5,6,0,12,6,0,2,105,110,0,0,
31,4,6,1,19,4,5,4,12,6,0,5,105,116,101,109,
115,0,0,0,9,5,1,6,12,6,0,6,97,112,112,101,
110,100,0,0,9,5,5,6,12,8,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,7,8,0,11,8,0,0,
0,0,0,0,0,0,0,0,31,6,8,1,19,6,7,6,
31,4,6,1,19,4,5,4,12,6,0,7,114,101,115,116,
111,114,101,0,13,5,6,0,31,4,0,0,19,4,5,4,
18,0,0,1,12,6,0,7,114,101,115,116,111,114,101,0,
13,5,6,0,31,4,0,0,19,4,5,4,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,1,
93,0,0,0,31,4,6,1,19,4,5,4,20,1,0,0,
0,0,0,0,12,27,0,8,108,105,115,116,95,110,117,100,
0,0,0,0,14,27,26,0,16,27,0,116,44,8,0,0,
28,2,0,0,9,1,0,2,12,2,0,4,100,105,99,116,
0,0,0,0,12,3,0,4,116,121,112,101,0,0,0,0,
10,1,3,2,12,2,0,2,123,125,0,0,12,3,0,3,
118,97,108,0,10,1,3,2,27,2,0,0,12,3,0,5,
105,116,101,109,115,0,0,0,10,1,3,2,12,4,0,5,
116,119,101,97,107,0,0,0,13,3,4,0,12,4,0,1,
44,0,0,0,11,5,0,0,0,0,0,0,0,0,0,0,
31,2,4,2,19,2,3,2,11,2,0,0,0,0,0,0,
0,0,0,0,12,5,0,5,99,104,101,99,107,0,0,0,
13,4,5,0,12,7,0,1,80,0,0,0,13,5,7,0,
12,7,0,5,116,111,107,101,110,0,0,0,9,5,5,7,
12,6,0,1,125,0,0,0,31,3,5,2,19,3,4,3,
23,2,2,3,21,2,0,0,18,0,0,48,12,4,0,5,
105,116,101,109,115,0,0,0,9,3,1,4,12,4,0,6,
97,112,112,101,110,100,0,0,9,3,3,4,12,6,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,5,6,0,
11,6,0,0,0,0,0,0,0,0,0,0,31,4,6,1,
19,4,5,4,31,2,4,1,19,2,3,2,12,4,0,5,
99,104,101,99,107,0,0,0,13,3,4,0,12,7,0,1,
80,0,0,0,13,4,7,0,12,7,0,5,116,111,107,101,
110,0,0,0,9,4,4,7,12,5,0,1,58,0,0,0,
12,6,0,1,44,0,0,0,31,2,4,3,19,2,3,2,
21,2,0,0,18,0,0,8,12,4,0,7,97,100,118,97,
110,99,101,0,13,3,4,0,31,2,0,0,19,2,3,2,
18,0,0,1,18,0,255,189,12,4,0,7,114,101,115,116,
111,114,101,0,13,3,4,0,31,2,0,0,19,2,3,2,
12,4,0,7,97,100,118,97,110,99,101,0,13,3,4,0,
12,4,0,1,125,0,0,0,31,2,4,1,19,2,3,2,
20,1,0,0,0,0,0,0,12,28,0,8,100,105,99,116,
95,110,117,100,0,0,0,0,14,28,27,0,16,28,0,17,
44,5,0,0,28,1,0,0,28,2,0,0,32,1,0,2,
12,4,0,1,80,0,0,0,13,3,4,0,12,4,0,7,
97,100,118,97,110,99,101,0,9,3,3,4,15,4,1,0,
31,2,4,1,19,2,3,2,20,2,0,0,0,0,0,0,
12,29,0,7,97,100,118,97,110,99,101,0,14,29,28,0,
16,29,1,2,44,10,0,0,27,2,0,0,15,1,2,0,
12,4,0,1,80,0,0,0,13,3,4,0,12,4,0,5,
116,111,107,101,110,0,0,0,9,3,3,4,15,2,3,0,
12,5,0,5,99,104,101,99,107,0,0,0,13,4,5,0,
12,7,0,1,80,0,0,0,13,5,7,0,12,7,0,5,
116,111,107,101,110,0,0,0,9,5,5,7,12,6,0,2,
110,108,0,0,31,3,5,2,19,3,4,3,21,3,0,0,
18,0,0,8,12,5,0,7,97,100,118,97,110,99,101,0,
13,4,5,0,31,3,0,0,19,3,4,3,18,0,255,233,
12,5,0,5,99,104,101,99,107,0,0,0,13,4,5,0,
12,7,0,1,80,0,0,0,13,5,7,0,12,7,0,5,
116,111,107,101,110,0,0,0,9,5,5,7,12,6,0,6,
105,110,100,101,110,116,0,0,31,3,5,2,19,3,4,3,
21,3,0,0,18,0,0,85,12,5,0,7,97,100,118,97,
110,99,101,0,13,4,5,0,12,5,0,6,105,110,100,101,
110,116,0,0,31,3,5,1,19,3,4,3,11,3,0,0,
0,0,0,0,0,0,0,0,12,6,0,5,99,104,101,99,
107,0,0,0,13,5,6,0,12,8,0,1,80,0,0,0,
13,6,8,0,12,8,0,5,116,111,107,101,110,0,0,0,
9,6,6,8,12,7,0,6,100,101,100,101,110,116,0,0,
31,4,6,2,19,4,5,4,23,3,3,4,21,3,0,0,
18,0,0,44,12,5,0,6,97,112,112,101,110,100,0,0,
9,4,1,5,12,7,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,6,7,0,11,7,0,0,0,0,0,0,
0,0,0,0,31,5,7,1,19,5,6,5,31,3,5,1,
19,3,4,3,12,5,0,5,99,104,101,99,107,0,0,0,
13,4,5,0,12,8,0,1,80,0,0,0,13,5,8,0,
12,8,0,5,116,111,107,101,110,0,0,0,9,5,5,8,
12,6,0,1,59,0,0,0,12,7,0,2,110,108,0,0,
31,3,5,3,19,3,4,3,21,3,0,0,18,0,0,8,
12,5,0,7,97,100,118,97,110,99,101,0,13,4,5,0,
31,3,0,0,19,3,4,3,18,0,255,231,18,0,255,192,
12,5,0,7,97,100,118,97,110,99,101,0,13,4,5,0,
12,5,0,6,100,101,100,101,110,116,0,0,31,3,5,1,
19,3,4,3,18,0,0,60,12,5,0,6,97,112,112,101,
110,100,0,0,9,4,1,5,12,7,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,6,7,0,11,7,0,0,
0,0,0,0,0,0,0,0,31,5,7,1,19,5,6,5,
31,3,5,1,19,3,4,3,12,5,0,5,99,104,101,99,
107,0,0,0,13,4,5,0,12,7,0,1,80,0,0,0,
13,5,7,0,12,7,0,5,116,111,107,101,110,0,0,0,
9,5,5,7,12,6,0,1,59,0,0,0,31,3,5,2,
19,3,4,3,21,3,0,0,18,0,0,26,12,5,0,7,
97,100,118,97,110,99,101,0,13,4,5,0,12,5,0,1,
59,0,0,0,31,3,5,1,19,3,4,3,12,5,0,6,
97,112,112,101,110,100,0,0,9,4,1,5,12,7,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,6,7,0,
11,7,0,0,0,0,0,0,0,0,0,0,31,5,7,1,
19,5,6,5,31,3,5,1,19,3,4,3,18,0,255,215,
18,0,0,1,12,5,0,5,99,104,101,99,107,0,0,0,
13,4,5,0,12,7,0,1,80,0,0,0,13,5,7,0,
12,7,0,5,116,111,107,101,110,0,0,0,9,5,5,7,
12,6,0,2,110,108,0,0,31,3,5,2,19,3,4,3,
21,3,0,0,18,0,0,8,12,5,0,7,97,100,118,97,
110,99,101,0,13,4,5,0,31,3,0,0,19,3,4,3,
18,0,255,233,11,3,0,0,0,0,0,0,0,0,240,63,
12,6,0,3,108,101,110,0,13,5,6,0,15,6,1,0,
31,4,6,1,19,4,5,4,25,3,3,4,21,3,0,0,
18,0,0,19,12,5,0,5,84,111,107,101,110,0,0,0,
13,4,5,0,12,9,0,3,112,111,115,0,9,5,2,9,
12,6,0,10,115,116,97,116,101,109,101,110,116,115,0,0,
12,7,0,1,59,0,0,0,15,8,1,0,31,3,5,4,
19,3,4,3,20,3,0,0,18,0,0,1,12,5,0,3,
112,111,112,0,9,4,1,5,31,3,0,0,19,3,4,3,
20,3,0,0,0,0,0,0,12,30,0,5,98,108,111,99,
107,0,0,0,14,30,29,0,16,30,0,175,44,11,0,0,
28,2,0,0,9,1,0,2,27,3,0,0,12,4,0,5,
105,116,101,109,115,0,0,0,10,1,4,3,15,2,3,0,
12,5,0,6,97,112,112,101,110,100,0,0,9,4,2,5,
12,6,0,1,80,0,0,0,13,5,6,0,12,6,0,5,
116,111,107,101,110,0,0,0,9,5,5,6,31,3,5,1,
19,3,4,3,12,5,0,7,97,100,118,97,110,99,101,0,
13,4,5,0,31,3,0,0,19,3,4,3,12,5,0,7,
97,100,118,97,110,99,101,0,13,4,5,0,12,5,0,1,
40,0,0,0,31,3,5,1,19,3,4,3,12,6,0,5,
84,111,107,101,110,0,0,0,13,5,6,0,12,10,0,3,
112,111,115,0,9,6,1,10,12,7,0,6,115,121,109,98,
111,108,0,0,12,8,0,3,40,41,58,0,27,9,0,0,
31,4,6,4,19,4,5,4,15,3,4,0,12,6,0,6,
97,112,112,101,110,100,0,0,9,5,2,6,15,6,3,0,
31,4,6,1,19,4,5,4,11,4,0,0,0,0,0,0,
0,0,0,0,12,7,0,5,99,104,101,99,107,0,0,0,
13,6,7,0,12,9,0,1,80,0,0,0,13,7,9,0,
12,9,0,5,116,111,107,101,110,0,0,0,9,7,7,9,
12,8,0,1,41,0,0,0,31,5,7,2,19,5,6,5,
23,4,4,5,21,4,0,0,18,0,0,65,12,6,0,5,
116,119,101,97,107,0,0,0,13,5,6,0,12,6,0,1,
44,0,0,0,11,7,0,0,0,0,0,0,0,0,0,0,
31,4,6,2,19,4,5,4,12,6,0,5,105,116,101,109,
115,0,0,0,9,5,3,6,12,6,0,6,97,112,112,101,
110,100,0,0,9,5,5,6,12,8,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,7,8,0,11,8,0,0,
0,0,0,0,0,0,0,0,31,6,8,1,19,6,7,6,
31,4,6,1,19,4,5,4,12,6,0,5,99,104,101,99,
107,0,0,0,13,5,6,0,12,8,0,1,80,0,0,0,
13,6,8,0,12,8,0,5,116,111,107,101,110,0,0,0,
9,6,6,8,12,7,0,1,44,0,0,0,31,4,6,2,
19,4,5,4,21,4,0,0,18,0,0,10,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,1,
44,0,0,0,31,4,6,1,19,4,5,4,18,0,0,1,
12,6,0,7,114,101,115,116,111,114,101,0,13,5,6,0,
31,4,0,0,19,4,5,4,18,0,255,172,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,1,
41,0,0,0,31,4,6,1,19,4,5,4,12,6,0,7,
97,100,118,97,110,99,101,0,13,5,6,0,12,6,0,1,
58,0,0,0,31,4,6,1,19,4,5,4,12,6,0,6,
97,112,112,101,110,100,0,0,9,5,2,6,12,8,0,5,
98,108,111,99,107,0,0,0,13,7,8,0,31,6,0,0,
19,6,7,6,31,4,6,1,19,4,5,4,20,1,0,0,
0,0,0,0,12,31,0,7,100,101,102,95,110,117,100,0,
14,31,30,0,16,31,0,48,44,8,0,0,28,2,0,0,
9,1,0,2,27,3,0,0,12,4,0,5,105,116,101,109,
115,0,0,0,10,1,4,3,15,2,3,0,12,5,0,6,
97,112,112,101,110,100,0,0,9,4,2,5,12,7,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,6,7,0,
11,7,0,0,0,0,0,0,0,0,0,0,31,5,7,1,
19,5,6,5,31,3,5,1,19,3,4,3,12,5,0,7,
97,100,118,97,110,99,101,0,13,4,5,0,12,5,0,1,
58,0,0,0,31,3,5,1,19,3,4,3,12,5,0,6,
97,112,112,101,110,100,0,0,9,4,2,5,12,7,0,5,
98,108,111,99,107,0,0,0,13,6,7,0,31,5,0,0,
19,5,6,5,31,3,5,1,19,3,4,3,20,1,0,0,
0,0,0,0,12,32,0,9,119,104,105,108,101,95,110,117,
100,0,0,0,14,32,31,0,16,32,0,57,44,11,0,0,
28,2,0,0,9,1,0,2,27,3,0,0,12,4,0,5,
105,116,101,109,115,0,0,0,10,1,4,3,15,2,3,0,
12,5,0,6,97,112,112,101,110,100,0,0,9,4,2,5,
12,7,0,10,101,120,112,114,101,115,115,105,111,110,0,0,
13,6,7,0,11,7,0,0,0,0,0,0,0,0,0,0,
31,5,7,1,19,5,6,5,31,3,5,1,19,3,4,3,
12,5,0,7,97,100,118,97,110,99,101,0,13,4,5,0,
12,5,0,1,58,0,0,0,31,3,5,1,19,3,4,3,
12,5,0,6,97,112,112,101,110,100,0,0,9,4,2,5,
12,7,0,4,105,108,115,116,0,0,0,0,13,6,7,0,
12,7,0,7,109,101,116,104,111,100,115,0,12,10,0,5,
98,108,111,99,107,0,0,0,13,9,10,0,31,8,0,0,
19,8,9,8,31,5,7,2,19,5,6,5,31,3,5,1,
19,3,4,3,20,1,0,0,0,0,0,0,12,33,0,9,
99,108,97,115,115,95,110,117,100,0,0,0,14,33,32,0,
16,33,0,53,44,8,0,0,28,2,0,0,9,1,0,2,
27,3,0,0,12,4,0,5,105,116,101,109,115,0,0,0,
10,1,4,3,15,2,3,0,12,5,0,6,97,112,112,101,
110,100,0,0,9,4,2,5,12,7,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,6,7,0,11,7,0,0,
0,0,0,0,0,0,0,0,31,5,7,1,19,5,6,5,
31,3,5,1,19,3,4,3,12,5,0,7,97,100,118,97,
110,99,101,0,13,4,5,0,12,5,0,6,105,109,112,111,
114,116,0,0,31,3,5,1,19,3,4,3,12,5,0,6,
97,112,112,101,110,100,0,0,9,4,2,5,12,7,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,6,7,0,
11,7,0,0,0,0,0,0,0,0,0,0,31,5,7,1,
19,5,6,5,31,3,5,1,19,3,4,3,20,1,0,0,
0,0,0,0,12,34,0,8,102,114,111,109,95,110,117,100,
0,0,0,0,14,34,33,0,16,34,0,89,44,8,0,0,
28,2,0,0,9,1,0,2,27,3,0,0,12,4,0,5,
105,116,101,109,115,0,0,0,10,1,4,3,15,2,3,0,
12,5,0,5,116,119,101,97,107,0,0,0,13,4,5,0,
12,5,0,2,105,110,0,0,11,6,0,0,0,0,0,0,
0,0,0,0,31,3,5,2,19,3,4,3,12,5,0,6,
97,112,112,101,110,100,0,0,9,4,2,5,12,7,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,6,7,0,
11,7,0,0,0,0,0,0,0,0,0,0,31,5,7,1,
19,5,6,5,31,3,5,1,19,3,4,3,12,5,0,7,
97,100,118,97,110,99,101,0,13,4,5,0,12,5,0,2,
105,110,0,0,31,3,5,1,19,3,4,3,12,5,0,6,
97,112,112,101,110,100,0,0,9,4,2,5,12,7,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,6,7,0,
11,7,0,0,0,0,0,0,0,0,0,0,31,5,7,1,
19,5,6,5,31,3,5,1,19,3,4,3,12,5,0,7,
114,101,115,116,111,114,101,0,13,4,5,0,31,3,0,0,
19,3,4,3,12,5,0,7,97,100,118,97,110,99,101,0,
13,4,5,0,12,5,0,1,58,0,0,0,31,3,5,1,
19,3,4,3,12,5,0,6,97,112,112,101,110,100,0,0,
9,4,2,5,12,7,0,5,98,108,111,99,107,0,0,0,
13,6,7,0,31,5,0,0,19,5,6,5,31,3,5,1,
19,3,4,3,20,1,0,0,0,0,0,0,12,35,0,7,
102,111,114,95,110,117,100,0,14,35,34,0,16,35,0,222,
44,16,0,0,28,2,0,0,9,1,0,2,27,3,0,0,
12,4,0,5,105,116,101,109,115,0,0,0,10,1,4,3,
15,2,3,0,12,6,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,5,6,0,11,6,0,0,0,0,0,0,
0,0,0,0,31,4,6,1,19,4,5,4,15,3,4,0,
12,6,0,7,97,100,118,97,110,99,101,0,13,5,6,0,
12,6,0,1,58,0,0,0,31,4,6,1,19,4,5,4,
12,7,0,5,98,108,111,99,107,0,0,0,13,6,7,0,
31,5,0,0,19,5,6,5,15,4,5,0,12,7,0,6,
97,112,112,101,110,100,0,0,9,6,2,7,12,9,0,5,
84,111,107,101,110,0,0,0,13,8,9,0,12,13,0,3,
112,111,115,0,9,9,1,13,12,10,0,4,101,108,105,102,
0,0,0,0,12,11,0,4,101,108,105,102,0,0,0,0,
15,13,3,0,15,14,4,0,27,12,13,2,31,7,9,4,
19,7,8,7,31,5,7,1,19,5,6,5,12,7,0,5,
99,104,101,99,107,0,0,0,13,6,7,0,12,9,0,1,
80,0,0,0,13,7,9,0,12,9,0,5,116,111,107,101,
110,0,0,0,9,7,7,9,12,8,0,4,101,108,105,102,
0,0,0,0,31,5,7,2,19,5,6,5,21,5,0,0,
18,0,0,69,12,7,0,1,80,0,0,0,13,6,7,0,
12,7,0,5,116,111,107,101,110,0,0,0,9,6,6,7,
15,5,6,0,12,8,0,7,97,100,118,97,110,99,101,0,
13,7,8,0,12,8,0,4,101,108,105,102,0,0,0,0,
31,6,8,1,19,6,7,6,12,8,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,7,8,0,11,8,0,0,
0,0,0,0,0,0,0,0,31,6,8,1,19,6,7,6,
15,3,6,0,12,8,0,7,97,100,118,97,110,99,101,0,
13,7,8,0,12,8,0,1,58,0,0,0,31,6,8,1,
19,6,7,6,12,8,0,5,98,108,111,99,107,0,0,0,
13,7,8,0,31,6,0,0,19,6,7,6,15,4,6,0,
12,8,0,6,97,112,112,101,110,100,0,0,9,7,2,8,
12,10,0,5,84,111,107,101,110,0,0,0,13,9,10,0,
12,14,0,3,112,111,115,0,9,10,5,14,12,11,0,4,
101,108,105,102,0,0,0,0,12,12,0,4,101,108,105,102,
0,0,0,0,15,14,3,0,15,15,4,0,27,13,14,2,
31,8,10,4,19,8,9,8,31,6,8,1,19,6,7,6,
18,0,255,171,12,8,0,5,99,104,101,99,107,0,0,0,
13,7,8,0,12,10,0,1,80,0,0,0,13,8,10,0,
12,10,0,5,116,111,107,101,110,0,0,0,9,8,8,10,
12,9,0,4,101,108,115,101,0,0,0,0,31,6,8,2,
19,6,7,6,21,6,0,0,18,0,0,57,12,7,0,1,
80,0,0,0,13,6,7,0,12,7,0,5,116,111,107,101,
110,0,0,0,9,6,6,7,15,5,6,0,12,8,0,7,
97,100,118,97,110,99,101,0,13,7,8,0,12,8,0,4,
101,108,115,101,0,0,0,0,31,6,8,1,19,6,7,6,
12,8,0,7,97,100,118,97,110,99,101,0,13,7,8,0,
12,8,0,1,58,0,0,0,31,6,8,1,19,6,7,6,
12,8,0,5,98,108,111,99,107,0,0,0,13,7,8,0,
31,6,0,0,19,6,7,6,15,4,6,0,12,8,0,6,
97,112,112,101,110,100,0,0,9,7,2,8,12,10,0,5,
84,111,107,101,110,0,0,0,13,9,10,0,12,14,0,3,
112,111,115,0,9,10,5,14,12,11,0,4,101,108,115,101,
0,0,0,0,12,12,0,4,101,108,115,101,0,0,0,0,
15,14,4,0,27,13,14,1,31,8,10,4,19,8,9,8,
31,6,8,1,19,6,7,6,18,0,0,1,20,1,0,0,
0,0,0,0,12,36,0,6,105,102,95,110,117,100,0,0,
14,36,35,0,16,36,0,233,44,16,0,0,28,2,0,0,
9,1,0,2,27,3,0,0,12,4,0,5,105,116,101,109,
115,0,0,0,10,1,4,3,15,2,3,0,12,5,0,7,
97,100,118,97,110,99,101,0,13,4,5,0,12,5,0,1,
58,0,0,0,31,3,5,1,19,3,4,3,12,6,0,5,
98,108,111,99,107,0,0,0,13,5,6,0,31,4,0,0,
19,4,5,4,15,3,4,0,12,6,0,6,97,112,112,101,
110,100,0,0,9,5,2,6,15,6,3,0,31,4,6,1,
19,4,5,4,12,6,0,5,99,104,101,99,107,0,0,0,
13,5,6,0,12,8,0,1,80,0,0,0,13,6,8,0,
12,8,0,5,116,111,107,101,110,0,0,0,9,6,6,8,
12,7,0,6,101,120,99,101,112,116,0,0,31,4,6,2,
19,4,5,4,21,4,0,0,18,0,0,108,12,6,0,1,
80,0,0,0,13,5,6,0,12,6,0,5,116,111,107,101,
110,0,0,0,9,5,5,6,15,4,5,0,12,7,0,7,
97,100,118,97,110,99,101,0,13,6,7,0,12,7,0,6,
101,120,99,101,112,116,0,0,31,5,7,1,19,5,6,5,
11,5,0,0,0,0,0,0,0,0,0,0,12,8,0,5,
99,104,101,99,107,0,0,0,13,7,8,0,12,10,0,1,
80,0,0,0,13,8,10,0,12,10,0,5,116,111,107,101,
110,0,0,0,9,8,8,10,12,9,0,1,58,0,0,0,
31,6,8,2,19,6,7,6,23,5,5,6,21,5,0,0,
18,0,0,13,12,8,0,10,101,120,112,114,101,115,115,105,
111,110,0,0,13,7,8,0,11,8,0,0,0,0,0,0,
0,0,0,0,31,6,8,1,19,6,7,6,15,5,6,0,
18,0,0,18,12,8,0,5,84,111,107,101,110,0,0,0,
13,7,8,0,12,11,0,3,112,111,115,0,9,8,4,11,
12,9,0,6,115,121,109,98,111,108,0,0,12,10,0,4,
78,111,110,101,0,0,0,0,31,6,8,3,19,6,7,6,
15,5,6,0,18,0,0,1,12,8,0,7,97,100,118,97,
110,99,101,0,13,7,8,0,12,8,0,1,58,0,0,0,
31,6,8,1,19,6,7,6,12,8,0,5,98,108,111,99,
107,0,0,0,13,7,8,0,31,6,0,0,19,6,7,6,
15,3,6,0,12,8,0,6,97,112,112,101,110,100,0,0,
9,7,2,8,12,10,0,5,84,111,107,101,110,0,0,0,
13,9,10,0,12,14,0,3,112,111,115,0,9,10,4,14,
12,11,0,6,101,120,99,101,112,116,0,0,12,12,0,6,
101,120,99,101,112,116,0,0,15,14,5,0,15,15,3,0,
27,13,14,2,31,8,10,4,19,8,9,8,31,6,8,1,
19,6,7,6,18,0,255,132,12,8,0,5,99,104,101,99,
107,0,0,0,13,7,8,0,12,10,0,1,80,0,0,0,
13,8,10,0,12,10,0,5,116,111,107,101,110,0,0,0,
9,8,8,10,12,9,0,4,101,108,115,101,0,0,0,0,
31,6,8,2,19,6,7,6,21,6,0,0,18,0,0,57,
12,7,0,1,80,0,0,0,13,6,7,0,12,7,0,5,
116,111,107,101,110,0,0,0,9,6,6,7,15,4,6,0,
12,8,0,7,97,100,118,97,110,99,101,0,13,7,8,0,
12,8,0,4,101,108,115,101,0,0,0,0,31,6,8,1,
19,6,7,6,12,8,0,7,97,100,118,97,110,99,101,0,
13,7,8,0,12,8,0,1,58,0,0,0,31,6,8,1,
19,6,7,6,12,8,0,5,98,108,111,99,107,0,0,0,
13,7,8,0,31,6,0,0,19,6,7,6,15,3,6,0,
12,8,0,6,97,112,112,101,110,100,0,0,9,7,2,8,
12,10,0,5,84,111,107,101,110,0,0,0,13,9,10,0,
12,14,0,3,112,111,115,0,9,10,4,14,12,11,0,4,
101,108,115,101,0,0,0,0,12,12,0,4,101,108,115,101,
0,0,0,0,15,14,3,0,27,13,14,1,31,8,10,4,
19,8,9,8,31,6,8,1,19,6,7,6,18,0,0,1,
20,1,0,0,0,0,0,0,12,37,0,7,116,114,121,95,
110,117,100,0,14,37,36,0,16,37,0,23,44,7,0,0,
28,2,0,0,9,1,0,2,12,4,0,2,98,112,0,0,
9,3,1,4,15,2,3,0,12,6,0,10,101,120,112,114,
101,115,115,105,111,110,0,0,13,5,6,0,15,6,2,0,
31,4,6,1,19,4,5,4,27,3,4,1,12,4,0,5,
105,116,101,109,115,0,0,0,10,1,4,3,20,1,0,0,
0,0,0,0,12,38,0,10,112,114,101,102,105,120,95,110,
117,100,0,0,14,38,37,0,16,38,0,40,44,10,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,99,104,101,99,
107,0,0,0,13,3,4,0,12,9,0,1,80,0,0,0,
13,4,9,0,12,9,0,5,116,111,107,101,110,0,0,0,
9,4,4,9,12,5,0,2,110,108,0,0,12,6,0,1,
59,0,0,0,12,7,0,3,101,111,102,0,12,8,0,6,
100,101,100,101,110,116,0,0,31,2,4,5,19,2,3,2,
21,2,0,0,18,0,0,3,20,1,0,0,18,0,0,1,
12,4,0,10,112,114,101,102,105,120,95,110,117,100,0,0,
13,3,4,0,15,4,1,0,31,2,4,1,19,2,3,2,
20,2,0,0,0,0,0,0,12,39,0,11,112,114,101,102,
105,120,95,110,117,100,48,0,14,39,38,0,16,39,0,28,
44,8,0,0,28,2,0,0,9,1,0,2,12,5,0,10,
101,120,112,114,101,115,115,105,111,110,0,0,13,4,5,0,
11,5,0,0,0,0,0,0,0,0,0,0,31,3,5,1,
19,3,4,3,15,2,3,0,12,5,0,4,105,108,115,116,
0,0,0,0,13,4,5,0,12,7,0,4,116,121,112,101,
0,0,0,0,9,5,1,7,15,6,2,0,31,3,5,2,
19,3,4,3,20,3,0,0,0,0,0,0,12,40,0,11,
112,114,101,102,105,120,95,110,117,100,115,0,14,40,39,0,
16,40,0,70,44,11,0,0,28,2,0,0,9,1,0,2,
12,5,0,10,101,120,112,114,101,115,115,105,111,110,0,0,
13,4,5,0,11,5,0,0,0,0,0,0,0,0,73,64,
31,3,5,1,19,3,4,3,15,2,3,0,12,4,0,4,
116,121,112,101,0,0,0,0,9,3,2,4,12,4,0,6,
110,117,109,98,101,114,0,0,23,3,3,4,21,3,0,0,
18,0,0,24,12,5,0,3,115,116,114,0,13,4,5,0,
11,5,0,0,0,0,0,0,0,0,0,0,12,8,0,5,
102,108,111,97,116,0,0,0,13,7,8,0,12,9,0,3,
118,97,108,0,9,8,2,9,31,6,8,1,19,6,7,6,
2,5,5,6,31,3,5,1,19,3,4,3,12,4,0,3,
118,97,108,0,10,2,4,3,20,2,0,0,18,0,0,1,
12,7,0,5,84,111,107,101,110,0,0,0,13,6,7,0,
12,10,0,3,112,111,115,0,9,7,1,10,12,8,0,6,
110,117,109,98,101,114,0,0,12,9,0,1,48,0,0,0,
31,4,7,3,19,4,6,4,15,5,2,0,27,3,4,2,
12,4,0,5,105,116,101,109,115,0,0,0,10,1,4,3,
20,1,0,0,0,0,0,0,12,41,0,10,112,114,101,102,
105,120,95,110,101,103,0,0,14,41,40,0,16,41,0,27,
44,6,0,0,28,2,0,0,9,1,0,2,12,5,0,10,
112,114,101,102,105,120,95,110,117,100,0,0,13,4,5,0,
15,5,1,0,31,3,5,1,19,3,4,3,15,2,3,0,
12,3,0,4,97,114,103,115,0,0,0,0,12,4,0,4,
116,121,112,101,0,0,0,0,10,1,4,3,12,3,0,1,
42,0,0,0,12,4,0,3,118,97,108,0,10,1,4,3,
20,1,0,0,0,0,0,0,12,42,0,9,118,97,114,103,
115,95,110,117,100,0,0,0,14,42,41,0,16,42,0,27,
44,6,0,0,28,2,0,0,9,1,0,2,12,5,0,10,
112,114,101,102,105,120,95,110,117,100,0,0,13,4,5,0,
15,5,1,0,31,3,5,1,19,3,4,3,15,2,3,0,
12,3,0,5,110,97,114,103,115,0,0,0,12,4,0,4,
116,121,112,101,0,0,0,0,10,1,4,3,12,3,0,2,
42,42,0,0,12,4,0,3,118,97,108,0,10,1,4,3,
20,1,0,0,0,0,0,0,12,43,0,9,110,97,114,103,
115,95,110,117,100,0,0,0,14,43,42,0,12,43,0,9,
98,97,115,101,95,100,109,97,112,0,0,0,12,45,0,1,
44,0,0,0,12,103,0,3,108,98,112,0,11,104,0,0,
0,0,0,0,0,0,52,64,12,105,0,2,98,112,0,0,
11,106,0,0,0,0,0,0,0,0,52,64,12,107,0,3,
108,101,100,0,12,109,0,11,105,110,102,105,120,95,116,117,
112,108,101,0,13,108,109,0,26,46,103,6,12,47,0,1,
43,0,0,0,12,103,0,3,108,98,112,0,11,104,0,0,
0,0,0,0,0,0,73,64,12,105,0,2,98,112,0,0,
11,106,0,0,0,0,0,0,0,0,73,64,12,107,0,3,
108,101,100,0,12,109,0,9,105,110,102,105,120,95,108,101,
100,0,0,0,13,108,109,0,26,48,103,6,12,49,0,1,
45,0,0,0,12,103,0,3,108,98,112,0,11,104,0,0,
0,0,0,0,0,0,73,64,12,105,0,3,110,117,100,0,
12,111,0,10,112,114,101,102,105,120,95,110,101,103,0,0,
13,106,111,0,12,107,0,2,98,112,0,0,11,108,0,0,
0,0,0,0,0,0,73,64,12,109,0,3,108,101,100,0,
12,111,0,9,105,110,102,105,120,95,108,101,100,0,0,0,
13,110,111,0,26,50,103,8,12,51,0,3,110,111,116,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,128,65,64,12,105,0,3,110,117,100,0,12,113,0,10,
112,114,101,102,105,120,95,110,117,100,0,0,13,106,113,0,
12,107,0,2,98,112,0,0,11,108,0,0,0,0,0,0,
0,128,65,64,12,109,0,2,98,112,0,0,11,110,0,0,
0,0,0,0,0,128,65,64,12,111,0,3,108,101,100,0,
12,113,0,9,105,110,102,105,120,95,110,111,116,0,0,0,
13,112,113,0,26,52,103,10,12,53,0,1,37,0,0,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,78,64,12,105,0,2,98,112,0,0,11,106,0,0,
0,0,0,0,0,0,78,64,12,107,0,3,108,101,100,0,
12,109,0,9,105,110,102,105,120,95,108,101,100,0,0,0,
13,108,109,0,26,54,103,6,12,55,0,1,42,0,0,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,78,64,12,105,0,3,110,117,100,0,12,111,0,9,
118,97,114,103,115,95,110,117,100,0,0,0,13,106,111,0,
12,107,0,2,98,112,0,0,11,108,0,0,0,0,0,0,
0,0,78,64,12,109,0,3,108,101,100,0,12,111,0,9,
105,110,102,105,120,95,108,101,100,0,0,0,13,110,111,0,
26,56,103,8,12,57,0,2,42,42,0,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,64,80,64,
12,105,0,3,110,117,100,0,12,111,0,9,110,97,114,103,
115,95,110,117,100,0,0,0,13,106,111,0,12,107,0,2,
98,112,0,0,11,108,0,0,0,0,0,0,0,64,80,64,
12,109,0,3,108,101,100,0,12,111,0,9,105,110,102,105,
120,95,108,101,100,0,0,0,13,110,111,0,26,58,103,8,
12,59,0,1,47,0,0,0,12,103,0,3,108,98,112,0,
11,104,0,0,0,0,0,0,0,0,78,64,12,105,0,2,
98,112,0,0,11,106,0,0,0,0,0,0,0,0,78,64,
12,107,0,3,108,101,100,0,12,109,0,9,105,110,102,105,
120,95,108,101,100,0,0,0,13,108,109,0,26,60,103,6,
12,61,0,1,40,0,0,0,12,103,0,3,108,98,112,0,
11,104,0,0,0,0,0,0,0,128,81,64,12,105,0,3,
110,117,100,0,12,111,0,9,112,97,114,101,110,95,110,117,
100,0,0,0,13,106,111,0,12,107,0,2,98,112,0,0,
11,108,0,0,0,0,0,0,0,0,84,64,12,109,0,3,
108,101,100,0,12,111,0,8,99,97,108,108,95,108,101,100,
0,0,0,0,13,110,111,0,26,62,103,8,12,63,0,1,
91,0,0,0,12,103,0,3,108,98,112,0,11,104,0,0,
0,0,0,0,0,128,81,64,12,105,0,3,110,117,100,0,
12,111,0,8,108,105,115,116,95,110,117,100,0,0,0,0,
13,106,111,0,12,107,0,2,98,112,0,0,11,108,0,0,
0,0,0,0,0,0,84,64,12,109,0,3,108,101,100,0,
12,111,0,7,103,101,116,95,108,101,100,0,13,110,111,0,
26,64,103,8,12,65,0,1,123,0,0,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,0,0,0,
12,105,0,3,110,117,100,0,12,107,0,8,100,105,99,116,
95,110,117,100,0,0,0,0,13,106,107,0,26,66,103,4,
12,67,0,1,46,0,0,0,12,103,0,3,108,98,112,0,
11,104,0,0,0,0,0,0,0,0,84,64,12,105,0,2,
98,112,0,0,11,106,0,0,0,0,0,0,0,0,84,64,
12,107,0,3,108,101,100,0,12,111,0,7,100,111,116,95,
108,101,100,0,13,108,111,0,12,109,0,4,116,121,112,101,
0,0,0,0,12,110,0,3,103,101,116,0,26,68,103,8,
12,69,0,5,98,114,101,97,107,0,0,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,0,0,0,
12,105,0,3,110,117,100,0,12,109,0,6,105,116,115,101,
108,102,0,0,13,106,109,0,12,107,0,4,116,121,112,101,
0,0,0,0,12,108,0,5,98,114,101,97,107,0,0,0,
26,70,103,6,12,71,0,4,112,97,115,115,0,0,0,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,0,0,12,105,0,3,110,117,100,0,12,109,0,6,
105,116,115,101,108,102,0,0,13,106,109,0,12,107,0,4,
116,121,112,101,0,0,0,0,12,108,0,4,112,97,115,115,
0,0,0,0,26,72,103,6,12,73,0,8,99,111,110,116,
105,110,117,101,0,0,0,0,12,103,0,3,108,98,112,0,
11,104,0,0,0,0,0,0,0,0,0,0,12,105,0,3,
110,117,100,0,12,109,0,6,105,116,115,101,108,102,0,0,
13,106,109,0,12,107,0,4,116,121,112,101,0,0,0,0,
12,108,0,8,99,111,110,116,105,110,117,101,0,0,0,0,
26,74,103,6,12,75,0,3,101,111,102,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,0,0,0,
12,105,0,4,116,121,112,101,0,0,0,0,12,106,0,3,
101,111,102,0,12,107,0,3,118,97,108,0,12,108,0,3,
101,111,102,0,26,76,103,6,12,77,0,3,100,101,102,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,0,0,12,105,0,3,110,117,100,0,12,109,0,7,
100,101,102,95,110,117,100,0,13,106,109,0,12,107,0,4,
116,121,112,101,0,0,0,0,12,108,0,3,100,101,102,0,
26,78,103,6,12,79,0,5,119,104,105,108,101,0,0,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,0,0,12,105,0,3,110,117,100,0,12,109,0,9,
119,104,105,108,101,95,110,117,100,0,0,0,13,106,109,0,
12,107,0,4,116,121,112,101,0,0,0,0,12,108,0,5,
119,104,105,108,101,0,0,0,26,80,103,6,12,81,0,3,
102,111,114,0,12,103,0,3,108,98,112,0,11,104,0,0,
0,0,0,0,0,0,0,0,12,105,0,3,110,117,100,0,
12,109,0,7,102,111,114,95,110,117,100,0,13,106,109,0,
12,107,0,4,116,121,112,101,0,0,0,0,12,108,0,3,
102,111,114,0,26,82,103,6,12,83,0,3,116,114,121,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,0,0,12,105,0,3,110,117,100,0,12,109,0,7,
116,114,121,95,110,117,100,0,13,106,109,0,12,107,0,4,
116,121,112,101,0,0,0,0,12,108,0,3,116,114,121,0,
26,84,103,6,12,85,0,2,105,102,0,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,0,0,0,
12,105,0,3,110,117,100,0,12,109,0,6,105,102,95,110,
117,100,0,0,13,106,109,0,12,107,0,4,116,121,112,101,
0,0,0,0,12,108,0,2,105,102,0,0,26,86,103,6,
12,87,0,5,99,108,97,115,115,0,0,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,0,0,0,
12,105,0,3,110,117,100,0,12,109,0,9,99,108,97,115,
115,95,110,117,100,0,0,0,13,106,109,0,12,107,0,4,
116,121,112,101,0,0,0,0,12,108,0,5,99,108,97,115,
115,0,0,0,26,88,103,6,12,89,0,5,114,97,105,115,
101,0,0,0,12,103,0,3,108,98,112,0,11,104,0,0,
0,0,0,0,0,0,0,0,12,105,0,3,110,117,100,0,
12,111,0,11,112,114,101,102,105,120,95,110,117,100,48,0,
13,106,111,0,12,107,0,4,116,121,112,101,0,0,0,0,
12,108,0,5,114,97,105,115,101,0,0,0,12,109,0,2,
98,112,0,0,11,110,0,0,0,0,0,0,0,0,52,64,
26,90,103,8,12,91,0,6,114,101,116,117,114,110,0,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,0,0,12,105,0,3,110,117,100,0,12,111,0,11,
112,114,101,102,105,120,95,110,117,100,48,0,13,106,111,0,
12,107,0,4,116,121,112,101,0,0,0,0,12,108,0,6,
114,101,116,117,114,110,0,0,12,109,0,2,98,112,0,0,
11,110,0,0,0,0,0,0,0,0,36,64,26,92,103,8,
12,93,0,6,105,109,112,111,114,116,0,0,12,103,0,3,
108,98,112,0,11,104,0,0,0,0,0,0,0,0,0,0,
12,105,0,3,110,117,100,0,12,111,0,11,112,114,101,102,
105,120,95,110,117,100,115,0,13,106,111,0,12,107,0,4,
116,121,112,101,0,0,0,0,12,108,0,6,105,109,112,111,
114,116,0,0,12,109,0,2,98,112,0,0,11,110,0,0,
0,0,0,0,0,0,52,64,26,94,103,8,12,95,0,4,
102,114,111,109,0,0,0,0,12,103,0,3,108,98,112,0,
11,104,0,0,0,0,0,0,0,0,0,0,12,105,0,3,
110,117,100,0,12,111,0,8,102,114,111,109,95,110,117,100,
0,0,0,0,13,106,111,0,12,107,0,4,116,121,112,101,
0,0,0,0,12,108,0,4,102,114,111,109,0,0,0,0,
12,109,0,2,98,112,0,0,11,110,0,0,0,0,0,0,
0,0,52,64,26,96,103,8,12,97,0,3,100,101,108,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,0,0,12,105,0,3,110,117,100,0,12,111,0,11,
112,114,101,102,105,120,95,110,117,100,115,0,13,106,111,0,
12,107,0,4,116,121,112,101,0,0,0,0,12,108,0,3,
100,101,108,0,12,109,0,2,98,112,0,0,11,110,0,0,
0,0,0,0,0,0,36,64,26,98,103,8,12,99,0,6,
103,108,111,98,97,108,0,0,12,103,0,3,108,98,112,0,
11,104,0,0,0,0,0,0,0,0,0,0,12,105,0,3,
110,117,100,0,12,111,0,11,112,114,101,102,105,120,95,110,
117,100,115,0,13,106,111,0,12,107,0,4,116,121,112,101,
0,0,0,0,12,108,0,7,103,108,111,98,97,108,115,0,
12,109,0,2,98,112,0,0,11,110,0,0,0,0,0,0,
0,0,52,64,26,100,103,8,12,101,0,1,61,0,0,0,
12,103,0,3,108,98,112,0,11,104,0,0,0,0,0,0,
0,0,36,64,12,105,0,2,98,112,0,0,11,106,0,0,
0,0,0,0,0,0,34,64,12,107,0,3,108,101,100,0,
12,109,0,9,105,110,102,105,120,95,108,101,100,0,0,0,
13,108,109,0,26,102,103,6,26,44,45,58,14,43,44,0,
16,43,0,32,44,14,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,12,4,0,1,42,0,0,0,
9,3,0,4,11,5,0,0,0,0,0,0,0,0,0,0,
42,4,3,5,18,0,0,18,12,7,0,9,98,97,115,101,
95,100,109,97,112,0,0,0,13,6,7,0,12,8,0,3,
108,98,112,0,15,9,1,0,12,10,0,2,98,112,0,0,
15,11,1,0,12,12,0,3,108,101,100,0,15,13,2,0,
26,7,8,6,10,6,4,7,18,0,255,238,0,0,0,0,
12,44,0,7,105,95,105,110,102,105,120,0,14,44,43,0,
12,46,0,7,105,95,105,110,102,105,120,0,13,45,46,0,
11,46,0,0,0,0,0,0,0,0,68,64,12,54,0,9,
105,110,102,105,120,95,108,101,100,0,0,0,13,47,54,0,
12,48,0,1,60,0,0,0,12,49,0,1,62,0,0,0,
12,50,0,2,60,61,0,0,12,51,0,2,62,61,0,0,
12,52,0,2,33,61,0,0,12,53,0,2,61,61,0,0,
31,44,46,8,19,44,45,44,12,46,0,7,105,95,105,110,
102,105,120,0,13,45,46,0,11,46,0,0,0,0,0,0,
0,0,68,64,12,50,0,8,105,110,102,105,120,95,105,115,
0,0,0,0,13,47,50,0,12,48,0,2,105,115,0,0,
12,49,0,2,105,110,0,0,31,44,46,4,19,44,45,44,
12,46,0,7,105,95,105,110,102,105,120,0,13,45,46,0,
11,46,0,0,0,0,0,0,0,0,36,64,12,52,0,9,
105,110,102,105,120,95,108,101,100,0,0,0,13,47,52,0,
12,48,0,2,43,61,0,0,12,49,0,2,45,61,0,0,
12,50,0,2,42,61,0,0,12,51,0,2,47,61,0,0,
31,44,46,6,19,44,45,44,12,46,0,7,105,95,105,110,
102,105,120,0,13,45,46,0,11,46,0,0,0,0,0,0,
0,0,63,64,12,50,0,9,105,110,102,105,120,95,108,101,
100,0,0,0,13,47,50,0,12,48,0,3,97,110,100,0,
12,49,0,1,38,0,0,0,31,44,46,4,19,44,45,44,
12,46,0,7,105,95,105,110,102,105,120,0,13,45,46,0,
11,46,0,0,0,0,0,0,0,0,62,64,12,50,0,9,
105,110,102,105,120,95,108,101,100,0,0,0,13,47,50,0,
12,48,0,2,111,114,0,0,12,49,0,1,124,0,0,0,
31,44,46,4,19,44,45,44,12,46,0,7,105,95,105,110,
102,105,120,0,13,45,46,0,11,46,0,0,0,0,0,0,
0,0,66,64,12,50,0,9,105,110,102,105,120,95,108,101,
100,0,0,0,13,47,50,0,12,48,0,2,60,60,0,0,
12,49,0,2,62,62,0,0,31,44,46,4,19,44,45,44,
16,44,0,30,44,11,0,0,12,2,0,1,42,0,0,0,
9,1,0,2,11,3,0,0,0,0,0,0,0,0,0,0,
42,2,1,3,18,0,0,20,12,5,0,9,98,97,115,101,
95,100,109,97,112,0,0,0,13,4,5,0,12,6,0,3,
108,98,112,0,11,7,0,0,0,0,0,0,0,0,0,0,
12,8,0,3,110,117,100,0,12,10,0,6,105,116,115,101,
108,102,0,0,13,9,10,0,26,5,6,4,10,4,2,5,
18,0,255,236,0,0,0,0,12,45,0,7,105,95,116,101,
114,109,115,0,14,45,44,0,12,47,0,7,105,95,116,101,
114,109,115,0,13,46,47,0,12,47,0,1,41,0,0,0,
12,48,0,1,125,0,0,0,12,49,0,1,93,0,0,0,
12,50,0,1,59,0,0,0,12,51,0,1,58,0,0,0,
12,52,0,2,110,108,0,0,12,53,0,4,101,108,105,102,
0,0,0,0,12,54,0,4,101,108,115,101,0,0,0,0,
12,55,0,4,84,114,117,101,0,0,0,0,12,56,0,5,
70,97,108,115,101,0,0,0,12,57,0,4,78,111,110,101,
0,0,0,0,12,58,0,4,110,97,109,101,0,0,0,0,
12,59,0,6,115,116,114,105,110,103,0,0,12,60,0,6,
110,117,109,98,101,114,0,0,12,61,0,6,105,110,100,101,
110,116,0,0,12,62,0,6,100,101,100,101,110,116,0,0,
12,63,0,6,101,120,99,101,112,116,0,0,31,45,47,17,
19,45,46,45,12,46,0,9,98,97,115,101,95,100,109,97,
112,0,0,0,13,45,46,0,12,46,0,2,110,108,0,0,
9,45,45,46,12,46,0,2,110,108,0,0,12,47,0,3,
118,97,108,0,10,45,47,46,16,45,0,38,44,7,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
12,4,0,4,100,109,97,112,0,0,0,0,13,3,4,0,
36,3,3,2,11,4,0,0,0,0,0,0,0,0,0,0,
23,3,3,4,21,3,0,0,18,0,0,15,12,5,0,5,
101,114,114,111,114,0,0,0,13,4,5,0,12,5,0,12,
117,110,107,110,111,119,110,32,34,37,115,34,0,0,0,0,
39,5,5,2,15,6,1,0,31,3,5,2,19,3,4,3,
18,0,0,1,12,4,0,4,100,109,97,112,0,0,0,0,
13,3,4,0,9,3,3,2,20,3,0,0,0,0,0,0,
12,46,0,4,103,109,97,112,0,0,0,0,14,46,45,0,
16,46,0,49,44,8,0,0,28,2,0,0,9,1,0,2,
12,3,0,4,116,121,112,101,0,0,0,0,9,2,1,3,
12,3,0,6,115,121,109,98,111,108,0,0,23,2,2,3,
21,2,0,0,18,0,0,13,12,5,0,4,103,109,97,112,
0,0,0,0,13,4,5,0,15,5,1,0,12,7,0,3,
118,97,108,0,9,6,1,7,31,3,5,2,19,3,4,3,
15,2,3,0,18,0,0,14,12,5,0,4,103,109,97,112,
0,0,0,0,13,4,5,0,15,5,1,0,12,7,0,4,
116,121,112,101,0,0,0,0,9,6,1,7,31,3,5,2,
19,3,4,3,15,2,3,0,18,0,0,1,12,5,0,5,
109,101,114,103,101,0,0,0,13,4,5,0,15,5,1,0,
15,6,2,0,31,3,5,2,19,3,4,3,20,1,0,0,
0,0,0,0,12,47,0,2,100,111,0,0,14,47,46,0,
16,47,0,83,44,10,0,0,12,3,0,1,80,0,0,0,
13,2,3,0,12,3,0,5,116,111,107,101,110,0,0,0,
9,2,2,3,15,1,2,0,27,3,0,0,15,2,3,0,
11,3,0,0,0,0,0,0,0,0,0,0,12,6,0,5,
99,104,101,99,107,0,0,0,13,5,6,0,12,8,0,1,
80,0,0,0,13,6,8,0,12,8,0,5,116,111,107,101,
110,0,0,0,9,6,6,8,12,7,0,3,101,111,102,0,
31,4,6,2,19,4,5,4,23,3,3,4,21,3,0,0,
18,0,0,14,12,5,0,6,97,112,112,101,110,100,0,0,
9,4,2,5,12,7,0,5,98,108,111,99,107,0,0,0,
13,6,7,0,31,5,0,0,19,5,6,5,31,3,5,1,
19,3,4,3,18,0,255,223,11,3,0,0,0,0,0,0,
0,0,240,63,12,6,0,3,108,101,110,0,13,5,6,0,
15,6,2,0,31,4,6,1,19,4,5,4,25,3,3,4,
21,3,0,0,18,0,0,19,12,5,0,5,84,111,107,101,
110,0,0,0,13,4,5,0,12,9,0,3,112,111,115,0,
9,5,1,9,12,6,0,10,115,116,97,116,101,109,101,110,
116,115,0,0,12,7,0,1,59,0,0,0,15,8,2,0,
31,3,5,4,19,3,4,3,20,3,0,0,18,0,0,1,
12,5,0,3,112,111,112,0,9,4,2,5,31,3,0,0,
19,3,4,3,20,3,0,0,0,0,0,0,12,48,0,9,
100,111,95,109,111,100,117,108,101,0,0,0,14,48,47,0,
16,48,0,58,44,9,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,11,3,0,0,0,0,0,0,
0,0,0,0,28,4,0,0,32,3,0,4,12,6,0,8,
116,111,107,101,110,105,122,101,0,0,0,0,13,5,6,0,
12,6,0,5,99,108,101,97,110,0,0,0,9,5,5,6,
15,6,1,0,31,4,6,1,19,4,5,4,15,1,4,0,
12,4,0,1,80,0,0,0,12,7,0,5,80,68,97,116,
97,0,0,0,13,6,7,0,15,7,1,0,15,8,2,0,
31,5,7,2,19,5,6,5,14,4,5,0,12,6,0,1,
80,0,0,0,13,5,6,0,12,6,0,4,105,110,105,116,
0,0,0,0,9,5,5,6,31,4,0,0,19,4,5,4,
12,7,0,9,100,111,95,109,111,100,117,108,101,0,0,0,
13,6,7,0,31,5,0,0,19,5,6,5,15,4,5,0,
12,5,0,1,80,0,0,0,28,6,0,0,14,5,6,0,
20,4,0,0,0,0,0,0,12,49,0,5,112,97,114,115,
101,0,0,0,14,49,48,0,0,0,0,0,
};
unsigned char tp_encode[] = {
44,104,0,0,12,2,0,6,105,109,112,111,114,116,0,0,
13,1,2,0,12,2,0,8,116,111,107,101,110,105,122,101,
0,0,0,0,31,0,2,1,19,0,1,0,12,1,0,8,
116,111,107,101,110,105,122,101,0,0,0,0,14,1,0,0,
12,2,0,6,105,109,112,111,114,116,0,0,13,1,2,0,
12,2,0,8,116,111,107,101,110,105,122,101,0,0,0,0,
31,0,2,1,19,0,1,0,12,2,0,8,95,95,100,105,
99,116,95,95,0,0,0,0,13,1,2,0,12,3,0,5,
84,111,107,101,110,0,0,0,9,2,0,3,12,0,0,5,
84,111,107,101,110,0,0,0,10,1,0,2,12,2,0,3,
115,116,114,0,13,1,2,0,11,2,0,0,0,0,0,0,
0,0,240,63,31,0,2,1,19,0,1,0,12,1,0,1,
46,0,0,0,36,0,0,1,21,0,0,0,18,0,0,23,
12,2,0,6,105,109,112,111,114,116,0,0,13,1,2,0,
12,2,0,4,98,111,111,116,0,0,0,0,31,0,2,1,
19,0,1,0,12,3,0,5,109,101,114,103,101,0,0,0,
13,2,3,0,12,5,0,8,95,95,100,105,99,116,95,95,
0,0,0,0,13,3,5,0,15,4,0,0,31,1,3,2,
19,1,2,1,18,0,0,1,11,1,0,0,0,0,0,0,
0,0,0,0,15,0,1,0,11,2,0,0,0,0,0,0,
0,0,240,63,15,1,2,0,11,3,0,0,0,0,0,0,
0,0,0,64,15,2,3,0,11,4,0,0,0,0,0,0,
0,0,8,64,15,3,4,0,11,5,0,0,0,0,0,0,
0,0,16,64,15,4,5,0,11,6,0,0,0,0,0,0,
0,0,20,64,15,5,6,0,11,7,0,0,0,0,0,0,
0,0,24,64,15,6,7,0,11,8,0,0,0,0,0,0,
0,0,28,64,15,7,8,0,11,9,0,0,0,0,0,0,
0,0,32,64,15,8,9,0,11,10,0,0,0,0,0,0,
0,0,34,64,15,9,10,0,11,11,0,0,0,0,0,0,
0,0,36,64,15,10,11,0,11,12,0,0,0,0,0,0,
0,0,38,64,15,11,12,0,11,13,0,0,0,0,0,0,
0,0,40,64,15,12,13,0,11,14,0,0,0,0,0,0,
0,0,42,64,15,13,14,0,11,15,0,0,0,0,0,0,
0,0,44,64,15,14,15,0,11,16,0,0,0,0,0,0,
0,0,46,64,15,15,16,0,11,17,0,0,0,0,0,0,
0,0,48,64,15,16,17,0,11,18,0,0,0,0,0,0,
0,0,49,64,15,17,18,0,11,19,0,0,0,0,0,0,
0,0,50,64,15,18,19,0,11,20,0,0,0,0,0,0,
0,0,51,64,15,19,20,0,11,21,0,0,0,0,0,0,
0,0,52,64,15,20,21,0,11,22,0,0,0,0,0,0,
0,0,53,64,15,21,22,0,11,23,0,0,0,0,0,0,
0,0,54,64,15,22,23,0,11,24,0,0,0,0,0,0,
0,0,55,64,15,23,24,0,11,25,0,0,0,0,0,0,
0,0,56,64,15,24,25,0,11,26,0,0,0,0,0,0,
0,0,57,64,15,25,26,0,11,27,0,0,0,0,0,0,
0,0,58,64,15,26,27,0,11,28,0,0,0,0,0,0,
0,0,59,64,15,27,28,0,11,29,0,0,0,0,0,0,
0,0,60,64,15,28,29,0,11,30,0,0,0,0,0,0,
0,0,61,64,15,29,30,0,11,31,0,0,0,0,0,0,
0,0,62,64,15,30,31,0,11,32,0,0,0,0,0,0,
0,0,63,64,15,31,32,0,11,33,0,0,0,0,0,0,
0,0,64,64,15,32,33,0,11,34,0,0,0,0,0,0,
0,128,64,64,15,33,34,0,11,35,0,0,0,0,0,0,
0,0,65,64,15,34,35,0,11,36,0,0,0,0,0,0,
0,128,65,64,15,35,36,0,11,37,0,0,0,0,0,0,
0,0,66,64,15,36,37,0,11,38,0,0,0,0,0,0,
0,128,66,64,15,37,38,0,11,39,0,0,0,0,0,0,
0,0,67,64,15,38,39,0,11,40,0,0,0,0,0,0,
0,128,67,64,15,39,40,0,11,41,0,0,0,0,0,0,
0,0,68,64,15,40,41,0,11,42,0,0,0,0,0,0,
0,128,68,64,15,41,42,0,11,43,0,0,0,0,0,0,
0,0,69,64,15,42,43,0,11,44,0,0,0,0,0,0,
0,128,69,64,15,43,44,0,11,45,0,0,0,0,0,0,
0,0,70,64,15,44,45,0,12,45,0,3,69,79,70,0,
14,45,0,0,12,0,0,3,65,68,68,0,14,0,1,0,
12,0,0,3,83,85,66,0,14,0,2,0,12,0,0,3,
77,85,76,0,14,0,3,0,12,0,0,3,68,73,86,0,
14,0,4,0,12,0,0,3,80,79,87,0,14,0,5,0,
12,0,0,3,65,78,68,0,14,0,6,0,12,0,0,2,
79,82,0,0,14,0,7,0,12,0,0,3,67,77,80,0,
14,0,8,0,12,0,0,3,71,69,84,0,14,0,9,0,
12,0,0,3,83,69,84,0,14,0,10,0,12,0,0,6,
78,85,77,66,69,82,0,0,14,0,11,0,12,0,0,6,
83,84,82,73,78,71,0,0,14,0,12,0,12,0,0,4,
71,71,69,84,0,0,0,0,14,0,13,0,12,0,0,4,
71,83,69,84,0,0,0,0,14,0,14,0,12,0,0,4,
77,79,86,69,0,0,0,0,14,0,15,0,12,0,0,3,
68,69,70,0,14,0,16,0,12,0,0,4,80,65,83,83,
0,0,0,0,14,0,17,0,12,0,0,4,74,85,77,80,
0,0,0,0,14,0,18,0,12,0,0,4,67,65,76,76,
0,0,0,0,14,0,19,0,12,0,0,6,82,69,84,85,
82,78,0,0,14,0,20,0,12,0,0,2,73,70,0,0,
14,0,21,0,12,0,0,5,68,69,66,85,71,0,0,0,
14,0,22,0,12,0,0,2,69,81,0,0,14,0,23,0,
12,0,0,2,76,69,0,0,14,0,24,0,12,0,0,2,
76,84,0,0,14,0,25,0,12,0,0,4,68,73,67,84,
0,0,0,0,14,0,26,0,12,0,0,4,76,73,83,84,
0,0,0,0,14,0,27,0,12,0,0,4,78,79,78,69,
0,0,0,0,14,0,28,0,12,0,0,3,76,69,78,0,
14,0,29,0,12,0,0,3,80,79,83,0,14,0,30,0,
12,0,0,6,80,65,82,65,77,83,0,0,14,0,31,0,
12,0,0,4,73,71,69,84,0,0,0,0,14,0,32,0,
12,0,0,4,70,73,76,69,0,0,0,0,14,0,33,0,
12,0,0,4,78,65,77,69,0,0,0,0,14,0,34,0,
12,0,0,2,78,69,0,0,14,0,35,0,12,0,0,3,
72,65,83,0,14,0,36,0,12,0,0,5,82,65,73,83,
69,0,0,0,14,0,37,0,12,0,0,6,83,69,84,74,
77,80,0,0,14,0,38,0,12,0,0,3,77,79,68,0,
14,0,39,0,12,0,0,3,76,83,72,0,14,0,40,0,
12,0,0,3,82,83,72,0,14,0,41,0,12,0,0,4,
73,84,69,82,0,0,0,0,14,0,42,0,12,0,0,3,
68,69,76,0,14,0,43,0,12,0,0,4,82,69,71,83,
0,0,0,0,14,0,44,0,26,0,0,0,12,1,0,6,
68,83,116,97,116,101,0,0,14,1,0,0,16,1,0,86,
44,11,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,28,4,0,0,9,3,0,4,15,4,2,0,
15,5,3,0,12,6,0,4,99,111,100,101,0,0,0,0,
10,1,6,4,12,4,0,5,102,110,97,109,101,0,0,0,
10,1,4,5,12,6,0,4,99,111,100,101,0,0,0,0,
9,5,1,6,12,6,0,5,115,112,108,105,116,0,0,0,
9,5,5,6,12,6,0,1,10,0,0,0,31,4,6,1,
19,4,5,4,12,5,0,5,108,105,110,101,115,0,0,0,
10,1,5,4,27,5,0,0,15,4,5,0,12,8,0,3,
116,97,103,0,12,9,0,3,69,79,70,0,27,7,8,2,
27,6,7,1,15,5,6,0,11,7,0,0,0,0,0,0,
0,0,0,0,15,6,7,0,27,8,0,0,15,7,8,0,
11,9,0,0,0,0,0,0,0,0,0,0,15,8,9,0,
26,10,0,0,15,9,10,0,12,10,0,5,115,116,97,99,
107,0,0,0,10,1,10,4,12,4,0,3,111,117,116,0,
10,1,4,5,12,4,0,7,95,115,99,111,112,101,105,0,
10,1,4,6,12,4,0,6,116,115,116,97,99,107,0,0,
10,1,4,7,12,4,0,5,95,116,97,103,105,0,0,0,
10,1,4,8,12,4,0,4,100,97,116,97,0,0,0,0,
10,1,4,9,11,4,0,0,0,0,0,0,0,0,0,0,
12,5,0,5,101,114,114,111,114,0,0,0,10,1,5,4,
0,0,0,0,12,2,0,8,95,95,105,110,105,116,95,95,
0,0,0,0,10,0,2,1,16,2,0,193,44,18,0,0,
28,2,0,0,9,1,0,2,11,2,0,0,0,0,0,0,
0,0,0,0,28,3,0,0,32,2,0,3,12,5,0,3,
108,101,110,0,13,4,5,0,12,6,0,5,115,116,97,99,
107,0,0,0,9,5,1,6,31,3,5,1,19,3,4,3,
21,3,0,0,18,0,0,56,12,5,0,5,115,116,97,99,
107,0,0,0,9,4,1,5,12,5,0,6,97,112,112,101,
110,100,0,0,9,4,4,5,12,17,0,4,118,97,114,115,
0,0,0,0,9,6,1,17,12,17,0,3,114,50,110,0,
9,7,1,17,12,17,0,3,110,50,114,0,9,8,1,17,
12,17,0,5,95,116,109,112,105,0,0,0,9,9,1,17,
12,17,0,4,109,114,101,103,0,0,0,0,9,10,1,17,
12,17,0,4,115,110,117,109,0,0,0,0,9,11,1,17,
12,17,0,8,95,103,108,111,98,97,108,115,0,0,0,0,
9,12,1,17,12,17,0,6,108,105,110,101,110,111,0,0,
9,13,1,17,12,17,0,7,103,108,111,98,97,108,115,0,
9,14,1,17,12,17,0,5,99,114,101,103,115,0,0,0,
9,15,1,17,12,17,0,4,116,109,112,99,0,0,0,0,
9,16,1,17,27,5,6,11,31,3,5,1,19,3,4,3,
18,0,0,13,12,5,0,5,115,116,97,99,107,0,0,0,
9,4,1,5,12,5,0,6,97,112,112,101,110,100,0,0,
9,4,4,5,28,5,0,0,31,3,5,1,19,3,4,3,
18,0,0,1,27,4,0,0,15,3,4,0,26,5,0,0,
15,4,5,0,26,6,0,0,15,5,6,0,11,7,0,0,
0,0,0,0,0,0,0,0,15,6,7,0,11,8,0,0,
0,0,0,0,0,0,0,0,15,7,8,0,12,11,0,3,
115,116,114,0,13,10,11,0,12,12,0,7,95,115,99,111,
112,101,105,0,9,11,1,12,31,9,11,1,19,9,10,9,
15,8,9,0,15,9,2,0,11,11,0,0,0,0,0,0,
0,0,240,191,15,10,11,0,27,12,0,0,15,11,12,0,
12,14,0,4,114,101,103,115,0,0,0,0,27,13,14,1,
15,12,13,0,11,14,0,0,0,0,0,0,0,0,0,0,
15,13,14,0,12,14,0,4,118,97,114,115,0,0,0,0,
10,1,14,3,12,3,0,3,114,50,110,0,10,1,3,4,
12,3,0,3,110,50,114,0,10,1,3,5,12,3,0,5,
95,116,109,112,105,0,0,0,10,1,3,6,12,3,0,4,
109,114,101,103,0,0,0,0,10,1,3,7,12,3,0,4,
115,110,117,109,0,0,0,0,10,1,3,8,12,3,0,8,
95,103,108,111,98,97,108,115,0,0,0,0,10,1,3,9,
12,3,0,6,108,105,110,101,110,111,0,0,10,1,3,10,
12,3,0,7,103,108,111,98,97,108,115,0,10,1,3,11,
12,3,0,5,99,114,101,103,115,0,0,0,10,1,3,12,
12,3,0,4,116,109,112,99,0,0,0,0,10,1,3,13,
12,4,0,7,95,115,99,111,112,101,105,0,9,3,1,4,
11,4,0,0,0,0,0,0,0,0,240,63,1,3,3,4,
12,4,0,7,95,115,99,111,112,101,105,0,10,1,4,3,
12,5,0,6,105,110,115,101,114,116,0,0,13,4,5,0,
12,6,0,5,99,114,101,103,115,0,0,0,9,5,1,6,
31,3,5,1,19,3,4,3,0,0,0,0,12,3,0,5,
98,101,103,105,110,0,0,0,10,0,3,2,16,3,0,164,
44,7,0,0,28,2,0,0,9,1,0,2,12,4,0,5,
99,114,101,103,115,0,0,0,9,3,1,4,12,4,0,6,
97,112,112,101,110,100,0,0,9,3,3,4,12,5,0,4,
109,114,101,103,0,0,0,0,9,4,1,5,31,2,4,1,
19,2,3,2,12,4,0,4,99,111,100,101,0,0,0,0,
13,3,4,0,12,5,0,3,69,79,70,0,13,4,5,0,
31,2,4,1,19,2,3,2,12,4,0,6,97,115,115,101,
114,116,0,0,13,3,4,0,12,5,0,4,116,109,112,99,
0,0,0,0,9,4,1,5,11,5,0,0,0,0,0,0,
0,0,0,0,23,4,4,5,31,2,4,1,19,2,3,2,
11,2,0,0,0,0,0,0,0,0,240,63,12,5,0,3,
108,101,110,0,13,4,5,0,12,6,0,5,115,116,97,99,
107,0,0,0,9,5,1,6,31,3,5,1,19,3,4,3,
25,2,2,3,21,2,0,0,18,0,0,98,12,4,0,5,
115,116,97,99,107,0,0,0,9,3,1,4,12,4,0,3,
112,111,112,0,9,3,3,4,31,2,0,0,19,2,3,2,
11,4,0,0,0,0,0,0,0,0,0,0,9,3,2,4,
12,4,0,4,118,97,114,115,0,0,0,0,10,1,4,3,
11,4,0,0,0,0,0,0,0,0,240,63,9,3,2,4,
12,4,0,3,114,50,110,0,10,1,4,3,11,4,0,0,
0,0,0,0,0,0,0,64,9,3,2,4,12,4,0,3,
110,50,114,0,10,1,4,3,11,4,0,0,0,0,0,0,
0,0,8,64,9,3,2,4,12,4,0,5,95,116,109,112,
105,0,0,0,10,1,4,3,11,4,0,0,0,0,0,0,
0,0,16,64,9,3,2,4,12,4,0,4,109,114,101,103,
0,0,0,0,10,1,4,3,11,4,0,0,0,0,0,0,
0,0,20,64,9,3,2,4,12,4,0,4,115,110,117,109,
0,0,0,0,10,1,4,3,11,4,0,0,0,0,0,0,
0,0,24,64,9,3,2,4,12,4,0,8,95,103,108,111,
98,97,108,115,0,0,0,0,10,1,4,3,11,4,0,0,
0,0,0,0,0,0,28,64,9,3,2,4,12,4,0,6,
108,105,110,101,110,111,0,0,10,1,4,3,11,4,0,0,
0,0,0,0,0,0,32,64,9,3,2,4,12,4,0,7,
103,108,111,98,97,108,115,0,10,1,4,3,11,4,0,0,
0,0,0,0,0,0,34,64,9,3,2,4,12,4,0,5,
99,114,101,103,115,0,0,0,10,1,4,3,11,4,0,0,
0,0,0,0,0,0,36,64,9,3,2,4,12,4,0,4,
116,109,112,99,0,0,0,0,10,1,4,3,18,0,0,11,
12,4,0,5,115,116,97,99,107,0,0,0,9,3,1,4,
12,4,0,3,112,111,112,0,9,3,3,4,31,2,0,0,
19,2,3,2,18,0,0,1,0,0,0,0,12,4,0,3,
101,110,100,0,10,0,4,3,16,4,0,62,44,7,0,0,
28,2,0,0,9,1,0,2,12,4,0,4,98,105,110,100,
0,0,0,0,13,3,4,0,12,6,0,6,68,83,116,97,
116,101,0,0,13,4,6,0,12,6,0,8,95,95,105,110,
105,116,95,95,0,0,0,0,9,4,4,6,15,5,1,0,
31,2,4,2,19,2,3,2,12,3,0,8,95,95,105,110,
105,116,95,95,0,0,0,0,10,1,3,2,12,4,0,4,
98,105,110,100,0,0,0,0,13,3,4,0,12,6,0,6,
68,83,116,97,116,101,0,0,13,4,6,0,12,6,0,5,
98,101,103,105,110,0,0,0,9,4,4,6,15,5,1,0,
31,2,4,2,19,2,3,2,12,3,0,5,98,101,103,105,
110,0,0,0,10,1,3,2,12,4,0,4,98,105,110,100,
0,0,0,0,13,3,4,0,12,6,0,6,68,83,116,97,
116,101,0,0,13,4,6,0,12,6,0,3,101,110,100,0,
9,4,4,6,15,5,1,0,31,2,4,2,19,2,3,2,
12,3,0,3,101,110,100,0,10,1,3,2,0,0,0,0,
12,5,0,7,95,95,110,101,119,95,95,0,10,0,5,4,
16,5,0,22,44,5,0,0,26,1,0,0,12,4,0,6,
68,83,116,97,116,101,0,0,13,3,4,0,12,4,0,7,
95,95,110,101,119,95,95,0,9,3,3,4,15,4,1,0,
31,2,4,1,19,2,3,2,12,3,0,8,95,95,105,110,
105,116,95,95,0,0,0,0,9,2,1,3,19,4,2,0,
20,1,0,0,0,0,0,0,12,6,0,8,95,95,99,97,
108,108,95,95,0,0,0,0,10,0,6,5,16,0,0,18,
44,5,0,0,28,2,0,0,9,1,0,2,12,4,0,1,
68,0,0,0,13,3,4,0,12,4,0,3,111,117,116,0,
9,3,3,4,12,4,0,6,97,112,112,101,110,100,0,0,
9,3,3,4,15,4,1,0,31,2,4,1,19,2,3,2,
0,0,0,0,12,6,0,6,105,110,115,101,114,116,0,0,
14,6,0,0,16,6,0,68,44,14,0,0,28,2,0,0,
9,1,0,2,12,4,0,6,105,115,116,121,112,101,0,0,
13,3,4,0,15,4,1,0,12,5,0,4,108,105,115,116,
0,0,0,0,31,2,4,2,19,2,3,2,21,2,0,0,
18,0,0,11,12,4,0,6,105,110,115,101,114,116,0,0,
13,3,4,0,15,4,1,0,31,2,4,1,19,2,3,2,
28,2,0,0,20,2,0,0,18,0,0,1,12,5,0,5,
114,97,110,103,101,0,0,0,13,4,5,0,11,5,0,0,
0,0,0,0,0,0,0,0,12,9,0,3,108,101,110,0,
13,8,9,0,15,9,1,0,31,6,9,1,19,6,8,6,
11,7,0,0,0,0,0,0,0,0,16,64,31,3,5,3,
19,3,4,3,11,4,0,0,0,0,0,0,0,0,0,0,
42,2,3,4,18,0,0,19,12,7,0,6,105,110,115,101,
114,116,0,0,13,6,7,0,12,8,0,4,100,97,116,97,
0,0,0,0,15,11,2,0,11,13,0,0,0,0,0,0,
0,0,16,64,1,12,2,13,27,10,11,2,9,9,1,10,
27,7,8,2,31,5,7,1,19,5,6,5,18,0,255,237,
0,0,0,0,12,7,0,5,119,114,105,116,101,0,0,0,
14,7,6,0,16,7,0,107,44,12,0,0,28,2,0,0,
9,1,0,2,12,3,0,4,65,82,71,86,0,0,0,0,
13,2,3,0,12,3,0,6,45,110,111,112,111,115,0,0,
36,2,2,3,21,2,0,0,18,0,0,4,28,2,0,0,
20,2,0,0,18,0,0,1,11,4,0,0,0,0,0,0,
0,0,0,0,9,3,1,4,15,2,3,0,11,5,0,0,
0,0,0,0,0,0,240,63,9,4,1,5,15,3,4,0,
12,5,0,1,68,0,0,0,13,4,5,0,12,5,0,6,
108,105,110,101,110,111,0,0,9,4,4,5,23,1,2,4,
21,1,0,0,18,0,0,4,28,1,0,0,20,1,0,0,
18,0,0,1,12,5,0,1,68,0,0,0,13,4,5,0,
12,5,0,5,108,105,110,101,115,0,0,0,9,4,4,5,
11,6,0,0,0,0,0,0,0,0,240,63,2,5,2,6,
9,4,4,5,15,1,4,0,12,5,0,1,68,0,0,0,
13,4,5,0,12,5,0,6,108,105,110,101,110,111,0,0,
10,4,5,2,12,6,0,1,0,0,0,0,11,7,0,0,
0,0,0,0,0,0,16,64,12,10,0,3,108,101,110,0,
13,9,10,0,15,10,1,0,31,8,10,1,19,8,9,8,
11,9,0,0,0,0,0,0,0,0,16,64,39,8,8,9,
2,7,7,8,3,6,6,7,1,5,1,6,15,4,5,0,
12,7,0,7,99,111,100,101,95,49,54,0,13,6,7,0,
12,10,0,3,80,79,83,0,13,7,10,0,12,11,0,3,
108,101,110,0,13,10,11,0,15,11,4,0,31,8,11,1,
19,8,10,8,11,10,0,0,0,0,0,0,0,0,16,64,
4,8,8,10,15,9,2,0,31,5,7,3,19,5,6,5,
12,7,0,5,119,114,105,116,101,0,0,0,13,6,7,0,
15,7,4,0,31,5,7,1,19,5,6,5,0,0,0,0,
12,8,0,6,115,101,116,112,111,115,0,0,14,8,7,0,
16,8,0,110,44,13,0,0,28,2,0,0,9,1,0,2,
11,2,0,0,0,0,0,0,0,0,0,0,28,3,0,0,
32,2,0,3,11,3,0,0,0,0,0,0,0,0,0,0,
28,4,0,0,32,3,0,4,11,4,0,0,0,0,0,0,
0,0,0,0,28,5,0,0,32,4,0,5,11,5,0,0,
0,0,0,0,0,0,0,0,12,8,0,6,105,115,116,121,
112,101,0,0,13,7,8,0,15,8,1,0,12,9,0,6,
110,117,109,98,101,114,0,0,31,6,8,2,19,6,7,6,
23,5,5,6,21,5,0,0,18,0,0,4,28,5,0,0,
37,5,0,0,18,0,0,1,11,5,0,0,0,0,0,0,
0,0,0,0,12,8,0,6,105,115,116,121,112,101,0,0,
13,7,8,0,15,8,2,0,12,9,0,6,110,117,109,98,
101,114,0,0,31,6,8,2,19,6,7,6,23,5,5,6,
21,5,0,0,18,0,0,4,28,5,0,0,37,5,0,0,
18,0,0,1,11,5,0,0,0,0,0,0,0,0,0,0,
12,8,0,6,105,115,116,121,112,101,0,0,13,7,8,0,
15,8,3,0,12,9,0,6,110,117,109,98,101,114,0,0,
31,6,8,2,19,6,7,6,23,5,5,6,21,5,0,0,
18,0,0,4,28,5,0,0,37,5,0,0,18,0,0,1,
11,5,0,0,0,0,0,0,0,0,0,0,12,8,0,6,
105,115,116,121,112,101,0,0,13,7,8,0,15,8,4,0,
12,9,0,6,110,117,109,98,101,114,0,0,31,6,8,2,
19,6,7,6,23,5,5,6,21,5,0,0,18,0,0,4,
28,5,0,0,37,5,0,0,18,0,0,1,12,7,0,5,
119,114,105,116,101,0,0,0,13,6,7,0,12,8,0,4,
99,111,100,101,0,0,0,0,15,9,1,0,15,10,2,0,
15,11,3,0,15,12,4,0,27,7,8,5,31,5,7,1,
19,5,6,5,0,0,0,0,12,9,0,4,99,111,100,101,
0,0,0,0,14,9,8,0,16,9,0,45,44,11,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
28,4,0,0,9,3,0,4,11,5,0,0,0,0,0,0,
0,0,0,0,25,4,3,5,21,4,0,0,18,0,0,7,
11,5,0,0,0,0,0,0,0,0,224,64,1,4,3,5,
15,3,4,0,18,0,0,1,12,6,0,4,99,111,100,101,
0,0,0,0,13,5,6,0,15,6,1,0,15,7,2,0,
11,10,0,0,0,0,0,0,0,224,239,64,6,8,3,10,
11,10,0,0,0,0,0,0,0,0,32,64,41,8,8,10,
11,10,0,0,0,0,0,0,0,224,111,64,6,9,3,10,
11,10,0,0,0,0,0,0,0,0,0,0,41,9,9,10,
31,4,6,4,19,4,5,4,0,0,0,0,12,10,0,7,
99,111,100,101,95,49,54,0,14,10,9,0,16,10,0,32,
44,11,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,28,4,0,0,9,3,0,4,12,5,0,4,
99,111,100,101,0,0,0,0,15,6,1,0,15,7,2,0,
11,10,0,0,0,0,0,0,0,224,239,64,6,8,3,10,
11,10,0,0,0,0,0,0,0,0,32,64,41,8,8,10,
11,10,0,0,0,0,0,0,0,224,111,64,6,9,3,10,
11,10,0,0,0,0,0,0,0,0,0,0,41,9,9,10,
27,4,5,5,20,4,0,0,0,0,0,0,12,11,0,10,
103,101,116,95,99,111,100,101,49,54,0,0,14,11,10,0,
16,11,0,60,44,11,0,0,28,2,0,0,9,1,0,2,
28,2,0,0,28,3,0,0,32,2,0,3,12,5,0,7,
103,101,116,95,116,109,112,0,13,4,5,0,15,5,2,0,
31,3,5,1,19,3,4,3,15,2,3,0,12,5,0,1,
0,0,0,0,11,6,0,0,0,0,0,0,0,0,16,64,
12,9,0,3,108,101,110,0,13,8,9,0,15,9,1,0,
31,7,9,1,19,7,8,7,11,8,0,0,0,0,0,0,
0,0,16,64,39,7,7,8,2,6,6,7,3,5,5,6,
1,4,1,5,15,3,4,0,12,6,0,7,99,111,100,101,
95,49,54,0,13,5,6,0,12,9,0,6,83,84,82,73,
78,71,0,0,13,6,9,0,15,7,2,0,12,10,0,3,
108,101,110,0,13,9,10,0,15,10,1,0,31,8,10,1,
19,8,9,8,31,4,6,3,19,4,5,4,12,6,0,5,
119,114,105,116,101,0,0,0,13,5,6,0,15,6,3,0,
31,4,6,1,19,4,5,4,20,2,0,0,0,0,0,0,
12,12,0,10,95,100,111,95,115,116,114,105,110,103,0,0,
14,12,11,0,16,12,0,20,44,8,0,0,28,2,0,0,
9,1,0,2,28,2,0,0,28,3,0,0,32,2,0,3,
12,5,0,10,95,100,111,95,115,116,114,105,110,103,0,0,
13,4,5,0,12,7,0,3,118,97,108,0,9,5,1,7,
15,6,2,0,31,3,5,2,19,3,4,3,20,3,0,0,
0,0,0,0,12,13,0,9,100,111,95,115,116,114,105,110,
103,0,0,0,14,13,12,0,16,13,0,53,44,10,0,0,
28,2,0,0,9,1,0,2,28,2,0,0,28,3,0,0,
32,2,0,3,12,5,0,7,103,101,116,95,116,109,112,0,
13,4,5,0,15,5,2,0,31,3,5,1,19,3,4,3,
15,2,3,0,12,5,0,4,99,111,100,101,0,0,0,0,
13,4,5,0,12,9,0,6,78,85,77,66,69,82,0,0,
13,5,9,0,15,6,2,0,11,7,0,0,0,0,0,0,
0,0,0,0,11,8,0,0,0,0,0,0,0,0,0,0,
31,3,5,4,19,3,4,3,12,5,0,5,119,114,105,116,
101,0,0,0,13,4,5,0,12,7,0,5,102,112,97,99,
107,0,0,0,13,6,7,0,12,9,0,6,110,117,109,98,
101,114,0,0,13,8,9,0,15,9,1,0,31,7,9,1,
19,7,8,7,31,5,7,1,19,5,6,5,31,3,5,1,
19,3,4,3,20,2,0,0,0,0,0,0,12,14,0,10,
95,100,111,95,110,117,109,98,101,114,0,0,14,14,13,0,
16,14,0,20,44,8,0,0,28,2,0,0,9,1,0,2,
28,2,0,0,28,3,0,0,32,2,0,3,12,5,0,10,
95,100,111,95,110,117,109,98,101,114,0,0,13,4,5,0,
12,7,0,3,118,97,108,0,9,5,1,7,15,6,2,0,
31,3,5,2,19,3,4,3,20,3,0,0,0,0,0,0,
12,15,0,9,100,111,95,110,117,109,98,101,114,0,0,0,
14,15,14,0,16,15,0,35,44,6,0,0,12,4,0,3,
115,116,114,0,13,3,4,0,12,5,0,1,68,0,0,0,
13,4,5,0,12,5,0,5,95,116,97,103,105,0,0,0,
9,4,4,5,31,2,4,1,19,2,3,2,15,1,2,0,
12,3,0,1,68,0,0,0,13,2,3,0,12,4,0,1,
68,0,0,0,13,3,4,0,12,4,0,5,95,116,97,103,
105,0,0,0,9,3,3,4,11,4,0,0,0,0,0,0,
0,0,240,63,1,3,3,4,12,4,0,5,95,116,97,103,
105,0,0,0,10,2,4,3,20,1,0,0,0,0,0,0,
12,16,0,7,103,101,116,95,116,97,103,0,14,16,15,0,
16,16,0,25,44,5,0,0,12,4,0,7,103,101,116,95,
116,97,103,0,13,3,4,0,31,2,0,0,19,2,3,2,
15,1,2,0,12,4,0,1,68,0,0,0,13,3,4,0,
12,4,0,6,116,115,116,97,99,107,0,0,9,3,3,4,
12,4,0,6,97,112,112,101,110,100,0,0,9,3,3,4,
15,4,1,0,31,2,4,1,19,2,3,2,20,1,0,0,
0,0,0,0,12,17,0,9,115,116,97,99,107,95,116,97,
103,0,0,0,14,17,16,0,16,17,0,15,44,4,0,0,
12,3,0,1,68,0,0,0,13,2,3,0,12,3,0,6,
116,115,116,97,99,107,0,0,9,2,2,3,12,3,0,3,
112,111,112,0,9,2,2,3,31,1,0,0,19,1,2,1,
0,0,0,0,12,18,0,7,112,111,112,95,116,97,103,0,
14,18,17,0,16,18,0,52,44,12,0,0,12,2,0,1,
42,0,0,0,9,1,0,2,12,3,0,1,68,0,0,0,
13,2,3,0,12,3,0,4,115,110,117,109,0,0,0,0,
9,2,2,3,12,3,0,1,58,0,0,0,1,2,2,3,
12,4,0,1,58,0,0,0,12,5,0,4,106,111,105,110,
0,0,0,0,9,4,4,5,27,6,0,0,11,8,0,0,
0,0,0,0,0,0,0,0,42,7,1,8,18,0,0,10,
12,11,0,3,115,116,114,0,13,10,11,0,15,11,7,0,
31,9,11,1,19,9,10,9,28,10,0,0,10,6,10,9,
18,0,255,246,15,5,6,0,31,3,5,1,19,3,4,3,
1,2,2,3,15,1,2,0,12,4,0,6,105,110,115,101,
114,116,0,0,13,3,4,0,12,8,0,3,116,97,103,0,
15,9,1,0,27,4,8,2,31,2,4,1,19,2,3,2,
0,0,0,0,12,19,0,3,116,97,103,0,14,19,18,0,
16,19,0,53,44,12,0,0,12,2,0,1,42,0,0,0,
9,1,0,2,12,3,0,1,68,0,0,0,13,2,3,0,
12,3,0,4,115,110,117,109,0,0,0,0,9,2,2,3,
12,3,0,1,58,0,0,0,1,2,2,3,12,4,0,1,
58,0,0,0,12,5,0,4,106,111,105,110,0,0,0,0,
9,4,4,5,27,6,0,0,11,8,0,0,0,0,0,0,
0,0,0,0,42,7,1,8,18,0,0,10,12,11,0,3,
115,116,114,0,13,10,11,0,15,11,7,0,31,9,11,1,
19,9,10,9,28,10,0,0,10,6,10,9,18,0,255,246,
15,5,6,0,31,3,5,1,19,3,4,3,1,2,2,3,
15,1,2,0,12,4,0,6,105,110,115,101,114,116,0,0,
13,3,4,0,12,8,0,4,106,117,109,112,0,0,0,0,
15,9,1,0,27,4,8,2,31,2,4,1,19,2,3,2,
0,0,0,0,12,20,0,4,106,117,109,112,0,0,0,0,
14,20,19,0,16,20,0,53,44,12,0,0,12,2,0,1,
42,0,0,0,9,1,0,2,12,3,0,1,68,0,0,0,
13,2,3,0,12,3,0,4,115,110,117,109,0,0,0,0,
9,2,2,3,12,3,0,1,58,0,0,0,1,2,2,3,
12,4,0,1,58,0,0,0,12,5,0,4,106,111,105,110,
0,0,0,0,9,4,4,5,27,6,0,0,11,8,0,0,
0,0,0,0,0,0,0,0,42,7,1,8,18,0,0,10,
12,11,0,3,115,116,114,0,13,10,11,0,15,11,7,0,
31,9,11,1,19,9,10,9,28,10,0,0,10,6,10,9,
18,0,255,246,15,5,6,0,31,3,5,1,19,3,4,3,
1,2,2,3,15,1,2,0,12,4,0,6,105,110,115,101,
114,116,0,0,13,3,4,0,12,8,0,6,115,101,116,106,
109,112,0,0,15,9,1,0,27,4,8,2,31,2,4,1,
19,2,3,2,0,0,0,0,12,21,0,6,115,101,116,106,
109,112,0,0,14,21,20,0,16,21,0,62,44,12,0,0,
12,2,0,1,42,0,0,0,9,1,0,2,12,3,0,1,
68,0,0,0,13,2,3,0,12,3,0,4,115,110,117,109,
0,0,0,0,9,2,2,3,12,3,0,1,58,0,0,0,
1,2,2,3,12,4,0,1,58,0,0,0,12,5,0,4,
106,111,105,110,0,0,0,0,9,4,4,5,27,6,0,0,
11,8,0,0,0,0,0,0,0,0,0,0,42,7,1,8,
18,0,0,10,12,11,0,3,115,116,114,0,13,10,11,0,
15,11,7,0,31,9,11,1,19,9,10,9,28,10,0,0,
10,6,10,9,18,0,255,246,15,5,6,0,31,3,5,1,
19,3,4,3,1,2,2,3,15,1,2,0,12,5,0,7,
103,101,116,95,114,101,103,0,13,4,5,0,15,5,1,0,
31,3,5,1,19,3,4,3,15,2,3,0,12,5,0,6,
105,110,115,101,114,116,0,0,13,4,5,0,12,8,0,3,
102,110,99,0,15,9,2,0,15,10,1,0,27,5,8,3,
31,3,5,1,19,3,4,3,20,2,0,0,0,0,0,0,
12,22,0,3,102,110,99,0,14,22,21,0,16,22,1,119,
44,17,0,0,26,2,0,0,15,1,2,0,27,3,0,0,
15,2,3,0,11,4,0,0,0,0,0,0,0,0,0,0,
15,3,4,0,12,6,0,1,68,0,0,0,13,5,6,0,
12,6,0,3,111,117,116,0,9,5,5,6,11,6,0,0,
0,0,0,0,0,0,0,0,42,4,5,6,18,0,0,71,
11,8,0,0,0,0,0,0,0,0,0,0,9,7,4,8,
12,8,0,3,116,97,103,0,23,7,7,8,21,7,0,0,
18,0,0,8,11,8,0,0,0,0,0,0,0,0,240,63,
9,7,4,8,10,1,7,3,18,0,255,240,18,0,0,1,
11,8,0,0,0,0,0,0,0,0,0,0,9,7,4,8,
12,8,0,4,114,101,103,115,0,0,0,0,23,7,7,8,
21,7,0,0,18,0,0,32,12,9,0,6,97,112,112,101,
110,100,0,0,9,8,2,9,12,11,0,10,103,101,116,95,
99,111,100,101,49,54,0,0,13,10,11,0,12,14,0,4,
82,69,71,83,0,0,0,0,13,11,14,0,11,14,0,0,
0,0,0,0,0,0,240,63,9,12,4,14,11,13,0,0,
0,0,0,0,0,0,0,0,31,9,11,3,19,9,10,9,
31,7,9,1,19,7,8,7,11,8,0,0,0,0,0,0,
0,0,240,63,1,7,3,8,15,3,7,0,18,0,255,199,
18,0,0,1,12,9,0,6,97,112,112,101,110,100,0,0,
9,8,2,9,15,9,4,0,31,7,9,1,19,7,8,7,
11,8,0,0,0,0,0,0,0,0,240,63,1,7,3,8,
15,3,7,0,18,0,255,185,12,7,0,5,114,97,110,103,
101,0,0,0,13,6,7,0,11,7,0,0,0,0,0,0,
0,0,0,0,12,10,0,3,108,101,110,0,13,9,10,0,
15,10,2,0,31,8,10,1,19,8,9,8,31,5,7,2,
19,5,6,5,11,6,0,0,0,0,0,0,0,0,0,0,
42,3,5,6,18,0,0,99,9,7,2,3,15,4,7,0,
11,8,0,0,0,0,0,0,0,0,0,0,9,7,4,8,
12,8,0,4,106,117,109,112,0,0,0,0,23,7,7,8,
21,7,0,0,18,0,0,23,12,9,0,10,103,101,116,95,
99,111,100,101,49,54,0,0,13,8,9,0,12,12,0,4,
74,85,77,80,0,0,0,0,13,9,12,0,11,10,0,0,
0,0,0,0,0,0,0,0,11,13,0,0,0,0,0,0,
0,0,240,63,9,12,4,13,9,11,1,12,2,11,11,3,
31,7,9,3,19,7,8,7,10,2,3,7,18,0,0,64,
11,8,0,0,0,0,0,0,0,0,0,0,9,7,4,8,
12,8,0,6,115,101,116,106,109,112,0,0,23,7,7,8,
21,7,0,0,18,0,0,23,12,9,0,10,103,101,116,95,
99,111,100,101,49,54,0,0,13,8,9,0,12,12,0,6,
83,69,84,74,77,80,0,0,13,9,12,0,11,10,0,0,
0,0,0,0,0,0,0,0,11,13,0,0,0,0,0,0,
0,0,240,63,9,12,4,13,9,11,1,12,2,11,11,3,
31,7,9,3,19,7,8,7,10,2,3,7,18,0,0,32,
11,8,0,0,0,0,0,0,0,0,0,0,9,7,4,8,
12,8,0,3,102,110,99,0,23,7,7,8,21,7,0,0,
18,0,0,23,12,9,0,10,103,101,116,95,99,111,100,101,
49,54,0,0,13,8,9,0,12,12,0,3,68,69,70,0,
13,9,12,0,11,12,0,0,0,0,0,0,0,0,240,63,
9,10,4,12,11,13,0,0,0,0,0,0,0,0,0,64,
9,12,4,13,9,11,1,12,2,11,11,3,31,7,9,3,
19,7,8,7,10,2,3,7,18,0,0,1,18,0,255,157,
12,7,0,5,114,97,110,103,101,0,0,0,13,6,7,0,
11,7,0,0,0,0,0,0,0,0,0,0,12,10,0,3,
108,101,110,0,13,9,10,0,15,10,2,0,31,8,10,1,
19,8,9,8,31,5,7,2,19,5,6,5,11,6,0,0,
0,0,0,0,0,0,0,0,42,3,5,6,18,0,0,140,
9,7,2,3,15,4,7,0,11,8,0,0,0,0,0,0,
0,0,0,0,9,7,4,8,12,8,0,4,100,97,116,97,
0,0,0,0,23,7,7,8,21,7,0,0,18,0,0,7,
11,8,0,0,0,0,0,0,0,0,240,63,9,7,4,8,
10,2,3,7,18,0,0,78,11,8,0,0,0,0,0,0,
0,0,0,0,9,7,4,8,12,8,0,4,99,111,100,101,
0,0,0,0,23,7,7,8,21,7,0,0,18,0,0,56,
11,9,0,0,0,0,0,0,0,0,240,63,28,10,0,0,
27,8,9,2,9,7,4,8,11,10,0,0,0,0,0,0,
0,0,0,0,9,9,7,10,15,8,9,0,11,11,0,0,
0,0,0,0,0,0,240,63,9,10,7,11,15,9,10,0,
11,12,0,0,0,0,0,0,0,0,0,64,9,11,7,12,
15,10,11,0,11,13,0,0,0,0,0,0,0,0,8,64,
9,12,7,13,15,11,12,0,12,13,0,3,99,104,114,0,
13,12,13,0,15,13,8,0,31,7,13,1,19,7,12,7,
12,14,0,3,99,104,114,0,13,13,14,0,15,14,9,0,
31,12,14,1,19,12,13,12,1,7,7,12,12,14,0,3,
99,104,114,0,13,13,14,0,15,14,10,0,31,12,14,1,
19,12,13,12,1,7,7,12,12,14,0,3,99,104,114,0,
13,13,14,0,15,14,11,0,31,12,14,1,19,12,13,12,
1,7,7,12,10,2,3,7,18,0,0,13,12,13,0,3,
115,116,114,0,13,12,13,0,12,14,0,4,104,117,104,63,
0,0,0,0,15,15,4,0,27,13,14,2,31,7,13,1,
19,7,12,7,37,7,0,0,18,0,0,1,12,13,0,3,
108,101,110,0,13,12,13,0,9,13,2,3,31,7,13,1,
19,7,12,7,11,12,0,0,0,0,0,0,0,0,16,64,
35,7,7,12,21,7,0,0,18,0,0,32,12,7,0,5,
99,111,100,101,32,0,0,0,12,14,0,3,115,116,114,0,
13,13,14,0,15,14,3,0,31,12,14,1,19,12,13,12,
1,7,7,12,12,12,0,17,32,105,115,32,119,114,111,110,
103,32,108,101,110,103,116,104,32,0,0,0,1,7,7,12,
12,14,0,3,115,116,114,0,13,13,14,0,12,16,0,3,
108,101,110,0,13,15,16,0,9,16,2,3,31,14,16,1,
19,14,15,14,31,12,14,1,19,12,13,12,1,7,7,12,
37,7,0,0,18,0,0,1,18,0,255,116,12,6,0,1,
68,0,0,0,13,5,6,0,12,6,0,3,111,117,116,0,
10,5,6,2,0,0,0,0,12,23,0,8,109,97,112,95,
116,97,103,115,0,0,0,0,14,23,22,0,16,23,0,27,
44,5,0,0,28,1,0,0,28,2,0,0,32,1,0,2,
28,3,0,0,35,2,1,3,21,2,0,0,18,0,0,3,
20,1,0,0,18,0,0,1,12,4,0,8,103,101,116,95,
116,109,112,115,0,0,0,0,13,3,4,0,11,4,0,0,
0,0,0,0,0,0,240,63,31,2,4,1,19,2,3,2,
11,3,0,0,0,0,0,0,0,0,0,0,9,2,2,3,
20,2,0,0,0,0,0,0,12,24,0,7,103,101,116,95,
116,109,112,0,14,24,23,0,16,24,0,84,44,14,0,0,
28,2,0,0,9,1,0,2,12,5,0,5,97,108,108,111,
99,0,0,0,13,4,5,0,15,5,1,0,31,3,5,1,
19,3,4,3,15,2,3,0,12,6,0,5,114,97,110,103,
101,0,0,0,13,5,6,0,15,6,2,0,1,7,2,1,
31,4,6,2,19,4,5,4,15,3,4,0,11,5,0,0,
0,0,0,0,0,0,0,0,42,4,3,5,18,0,0,42,
12,8,0,7,115,101,116,95,114,101,103,0,13,7,8,0,
15,8,4,0,12,9,0,1,36,0,0,0,12,12,0,3,
115,116,114,0,13,11,12,0,12,13,0,1,68,0,0,0,
13,12,13,0,12,13,0,5,95,116,109,112,105,0,0,0,
9,12,12,13,31,10,12,1,19,10,11,10,1,9,9,10,
31,6,8,2,19,6,7,6,12,7,0,1,68,0,0,0,
13,6,7,0,12,8,0,1,68,0,0,0,13,7,8,0,
12,8,0,5,95,116,109,112,105,0,0,0,9,7,7,8,
11,8,0,0,0,0,0,0,0,0,240,63,1,7,7,8,
12,8,0,5,95,116,109,112,105,0,0,0,10,6,8,7,
18,0,255,214,12,6,0,1,68,0,0,0,13,5,6,0,
12,7,0,1,68,0,0,0,13,6,7,0,12,7,0,4,
116,109,112,99,0,0,0,0,9,6,6,7,1,6,6,1,
12,7,0,4,116,109,112,99,0,0,0,0,10,5,7,6,
20,3,0,0,0,0,0,0,12,25,0,8,103,101,116,95,
116,109,112,115,0,0,0,0,14,25,24,0,16,25,0,69,
44,16,0,0,28,2,0,0,9,1,0,2,12,4,0,0,
0,0,0,0,12,5,0,4,106,111,105,110,0,0,0,0,
9,4,4,5,27,6,0,0,12,10,0,5,114,97,110,103,
101,0,0,0,13,9,10,0,11,10,0,0,0,0,0,0,
0,0,0,0,12,13,0,3,109,105,110,0,13,12,13,0,
11,13,0,0,0,0,0,0,0,0,112,64,12,15,0,1,
68,0,0,0,13,14,15,0,12,15,0,4,109,114,101,103,
0,0,0,0,9,14,14,15,1,14,14,1,31,11,13,2,
19,11,12,11,31,8,10,2,19,8,9,8,11,9,0,0,
0,0,0,0,0,0,0,0,42,7,8,9,18,0,0,14,
12,10,0,2,48,49,0,0,12,12,0,1,68,0,0,0,
13,11,12,0,12,12,0,3,114,50,110,0,9,11,11,12,
36,11,11,7,9,10,10,11,28,11,0,0,10,6,11,10,
18,0,255,242,15,5,6,0,31,3,5,1,19,3,4,3,
15,2,3,0,12,5,0,5,105,110,100,101,120,0,0,0,
9,4,2,5,12,5,0,1,48,0,0,0,3,5,5,1,
31,3,5,1,19,3,4,3,20,3,0,0,0,0,0,0,
12,26,0,5,97,108,108,111,99,0,0,0,14,26,25,0,
16,26,0,29,44,4,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,23,2,1,3,21,2,0,0,18,0,0,6,
11,2,0,0,0,0,0,0,0,0,0,0,20,2,0,0,
18,0,0,1,12,3,0,1,68,0,0,0,13,2,3,0,
12,3,0,3,114,50,110,0,9,2,2,3,9,2,2,1,
11,3,0,0,0,0,0,0,0,0,0,0,9,2,2,3,
12,3,0,1,36,0,0,0,23,2,2,3,20,2,0,0,
0,0,0,0,12,27,0,6,105,115,95,116,109,112,0,0,
14,27,26,0,16,27,0,31,44,7,0,0,28,2,0,0,
9,1,0,2,12,4,0,1,68,0,0,0,13,3,4,0,
12,4,0,3,114,50,110,0,9,3,3,4,9,3,3,1,
15,2,3,0,12,5,0,8,102,114,101,101,95,114,101,103,
0,0,0,0,13,4,5,0,15,5,1,0,31,3,5,1,
19,3,4,3,12,5,0,7,115,101,116,95,114,101,103,0,
13,4,5,0,15,5,1,0,12,6,0,1,42,0,0,0,
1,6,6,2,31,3,5,2,19,3,4,3,0,0,0,0,
12,28,0,6,117,110,95,116,109,112,0,0,14,28,27,0,
16,28,0,24,44,5,0,0,28,2,0,0,9,1,0,2,
12,4,0,6,105,115,95,116,109,112,0,0,13,3,4,0,
15,4,1,0,31,2,4,1,19,2,3,2,21,2,0,0,
18,0,0,10,12,4,0,8,102,114,101,101,95,114,101,103,
0,0,0,0,13,3,4,0,15,4,1,0,31,2,4,1,
19,2,3,2,18,0,0,1,20,1,0,0,0,0,0,0,
12,29,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
14,29,28,0,16,29,0,19,44,7,0,0,28,2,0,0,
9,1,0,2,11,3,0,0,0,0,0,0,0,0,0,0,
42,2,1,3,18,0,0,10,12,6,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,5,6,0,15,6,2,0,
31,4,6,1,19,4,5,4,18,0,255,246,0,0,0,0,
12,30,0,9,102,114,101,101,95,116,109,112,115,0,0,0,
14,30,29,0,16,30,0,43,44,8,0,0,28,2,0,0,
9,1,0,2,12,3,0,1,68,0,0,0,13,2,3,0,
12,3,0,3,110,50,114,0,9,2,2,3,36,2,2,1,
11,3,0,0,0,0,0,0,0,0,0,0,23,2,2,3,
21,2,0,0,18,0,0,18,12,4,0,7,115,101,116,95,
114,101,103,0,13,3,4,0,12,7,0,5,97,108,108,111,
99,0,0,0,13,6,7,0,11,7,0,0,0,0,0,0,
0,0,240,63,31,4,7,1,19,4,6,4,15,5,1,0,
31,2,4,2,19,2,3,2,18,0,0,1,12,3,0,1,
68,0,0,0,13,2,3,0,12,3,0,3,110,50,114,0,
9,2,2,3,9,2,2,1,20,2,0,0,0,0,0,0,
12,31,0,7,103,101,116,95,114,101,103,0,14,31,30,0,
16,31,0,44,44,9,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,12,4,0,1,68,0,0,0,
13,3,4,0,12,4,0,3,110,50,114,0,9,3,3,4,
10,3,2,1,12,4,0,1,68,0,0,0,13,3,4,0,
12,4,0,3,114,50,110,0,9,3,3,4,10,3,1,2,
12,4,0,1,68,0,0,0,13,3,4,0,12,6,0,3,
109,97,120,0,13,5,6,0,12,8,0,1,68,0,0,0,
13,6,8,0,12,8,0,4,109,114,101,103,0,0,0,0,
9,6,6,8,11,8,0,0,0,0,0,0,0,0,240,63,
1,7,1,8,31,4,6,2,19,4,5,4,12,5,0,4,
109,114,101,103,0,0,0,0,10,3,5,4,0,0,0,0,
12,32,0,7,115,101,116,95,114,101,103,0,14,32,31,0,
16,32,0,55,44,5,0,0,28,2,0,0,9,1,0,2,
12,4,0,6,105,115,95,116,109,112,0,0,13,3,4,0,
15,4,1,0,31,2,4,1,19,2,3,2,21,2,0,0,
18,0,0,20,12,3,0,1,68,0,0,0,13,2,3,0,
12,4,0,1,68,0,0,0,13,3,4,0,12,4,0,4,
116,109,112,99,0,0,0,0,9,3,3,4,11,4,0,0,
0,0,0,0,0,0,240,63,2,3,3,4,12,4,0,4,
116,109,112,99,0,0,0,0,10,2,4,3,18,0,0,1,
12,4,0,1,68,0,0,0,13,3,4,0,12,4,0,3,
114,50,110,0,9,3,3,4,9,3,3,1,15,2,3,0,
12,4,0,1,68,0,0,0,13,3,4,0,12,4,0,3,
114,50,110,0,9,3,3,4,43,3,1,0,12,4,0,1,
68,0,0,0,13,3,4,0,12,4,0,3,110,50,114,0,
9,3,3,4,43,3,2,0,0,0,0,0,12,33,0,8,
102,114,101,101,95,114,101,103,0,0,0,0,14,33,32,0,
16,33,0,49,44,14,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,12,5,0,5,105,116,101,109,
115,0,0,0,9,4,1,5,15,3,4,0,12,5,0,3,
118,97,108,0,9,4,1,5,28,6,0,0,11,7,0,0,
0,0,0,0,0,0,240,191,27,5,6,2,9,4,4,5,
12,5,0,3,118,97,108,0,10,1,5,4,12,7,0,5,
84,111,107,101,110,0,0,0,13,6,7,0,12,11,0,3,
112,111,115,0,9,7,1,11,12,8,0,6,115,121,109,98,
111,108,0,0,12,9,0,1,61,0,0,0,11,13,0,0,
0,0,0,0,0,0,0,0,9,11,3,13,15,12,1,0,
27,10,11,2,31,5,7,4,19,5,6,5,15,4,5,0,
15,6,4,0,31,5,6,1,19,5,2,5,20,5,0,0,
0,0,0,0,12,34,0,7,105,109,97,110,97,103,101,0,
14,34,33,0,16,34,0,68,44,13,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,28,4,0,0,
9,3,0,4,28,4,0,0,28,5,0,0,32,4,0,5,
12,7,0,7,103,101,116,95,116,109,112,0,13,6,7,0,
15,7,4,0,31,5,7,1,19,5,6,5,15,4,5,0,
12,8,0,2,100,111,0,0,13,7,8,0,15,8,2,0,
15,9,4,0,31,6,8,2,19,6,7,6,15,5,6,0,
12,9,0,2,100,111,0,0,13,8,9,0,15,9,3,0,
31,7,9,1,19,7,8,7,15,6,7,0,15,7,5,0,
15,5,6,0,12,9,0,4,99,111,100,101,0,0,0,0,
13,8,9,0,15,9,1,0,15,10,4,0,15,11,7,0,
15,12,5,0,31,6,9,4,19,6,8,6,35,6,4,7,
21,6,0,0,18,0,0,10,12,9,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,8,9,0,15,9,7,0,
31,6,9,1,19,6,8,6,18,0,0,1,12,9,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,8,9,0,
15,9,5,0,31,6,9,1,19,6,8,6,20,4,0,0,
0,0,0,0,12,35,0,5,105,110,102,105,120,0,0,0,
14,35,34,0,16,35,0,136,44,15,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,28,4,0,0,
9,3,0,4,28,5,0,0,9,4,0,5,28,5,0,0,
28,6,0,0,32,5,0,6,12,8,0,7,103,101,116,95,
116,109,112,0,13,7,8,0,15,8,5,0,31,6,8,1,
19,6,7,6,15,5,6,0,12,9,0,7,103,101,116,95,
116,109,112,0,13,8,9,0,31,7,0,0,19,7,8,7,
15,6,7,0,12,9,0,10,95,100,111,95,110,117,109,98,
101,114,0,0,13,8,9,0,15,9,1,0,31,7,9,1,
19,7,8,7,15,1,7,0,12,10,0,7,103,101,116,95,
116,97,103,0,13,9,10,0,31,8,0,0,19,8,9,8,
15,7,8,0,12,10,0,2,100,111,0,0,13,9,10,0,
15,10,3,0,15,11,5,0,31,8,10,2,19,8,9,8,
15,5,8,0,12,10,0,4,99,111,100,101,0,0,0,0,
13,9,10,0,12,14,0,2,69,81,0,0,13,10,14,0,
15,11,6,0,15,12,5,0,15,13,1,0,31,8,10,4,
19,8,9,8,12,10,0,4,99,111,100,101,0,0,0,0,
13,9,10,0,12,12,0,2,73,70,0,0,13,10,12,0,
15,11,6,0,31,8,10,2,19,8,9,8,12,10,0,4,
106,117,109,112,0,0,0,0,13,9,10,0,15,10,7,0,
12,11,0,4,101,108,115,101,0,0,0,0,31,8,10,2,
19,8,9,8,12,10,0,4,106,117,109,112,0,0,0,0,
13,9,10,0,15,10,7,0,12,11,0,3,101,110,100,0,
31,8,10,2,19,8,9,8,12,10,0,3,116,97,103,0,
13,9,10,0,15,10,7,0,12,11,0,4,101,108,115,101,
0,0,0,0,31,8,10,2,19,8,9,8,12,10,0,2,
100,111,0,0,13,9,10,0,15,10,4,0,15,11,5,0,
31,8,10,2,19,8,9,8,15,5,8,0,12,10,0,3,
116,97,103,0,13,9,10,0,15,10,7,0,12,11,0,3,
101,110,100,0,31,8,10,2,19,8,9,8,12,10,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,9,10,0,
15,10,6,0,31,8,10,1,19,8,9,8,12,10,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,9,10,0,
15,10,1,0,31,8,10,1,19,8,9,8,20,5,0,0,
0,0,0,0,12,36,0,8,115,115,95,105,110,102,105,120,
0,0,0,0,14,36,35,0,16,36,0,26,44,7,0,0,
28,1,0,0,28,2,0,0,32,1,0,2,12,4,0,7,
103,101,116,95,116,109,112,0,13,3,4,0,15,4,1,0,
31,2,4,1,19,2,3,2,15,1,2,0,12,4,0,4,
99,111,100,101,0,0,0,0,13,3,4,0,12,6,0,4,
78,79,78,69,0,0,0,0,13,4,6,0,15,5,1,0,
31,2,4,2,19,2,3,2,20,1,0,0,0,0,0,0,
12,37,0,8,95,100,111,95,110,111,110,101,0,0,0,0,
14,37,36,0,16,37,2,41,44,33,0,0,28,2,0,0,
9,1,0,2,28,2,0,0,28,3,0,0,32,2,0,3,
12,5,0,1,61,0,0,0,27,4,5,1,15,3,4,0,
12,6,0,2,43,61,0,0,12,7,0,2,45,61,0,0,
12,8,0,2,42,61,0,0,12,9,0,2,47,61,0,0,
27,5,6,4,15,4,5,0,12,7,0,1,60,0,0,0,
12,8,0,1,62,0,0,0,12,9,0,2,60,61,0,0,
12,10,0,2,62,61,0,0,12,11,0,2,61,61,0,0,
12,12,0,2,33,61,0,0,27,6,7,6,15,5,6,0,
12,8,0,1,43,0,0,0,12,32,0,3,65,68,68,0,
13,9,32,0,12,10,0,1,42,0,0,0,12,32,0,3,
77,85,76,0,13,11,32,0,12,12,0,1,47,0,0,0,
12,32,0,3,68,73,86,0,13,13,32,0,12,14,0,2,
42,42,0,0,12,32,0,3,80,79,87,0,13,15,32,0,
12,16,0,1,45,0,0,0,12,32,0,3,83,85,66,0,
13,17,32,0,12,18,0,3,97,110,100,0,12,32,0,3,
65,78,68,0,13,19,32,0,12,20,0,2,111,114,0,0,
12,32,0,2,79,82,0,0,13,21,32,0,12,22,0,1,
37,0,0,0,12,32,0,3,77,79,68,0,13,23,32,0,
12,24,0,2,62,62,0,0,12,32,0,3,82,83,72,0,
13,25,32,0,12,26,0,2,60,60,0,0,12,32,0,3,
76,83,72,0,13,27,32,0,12,28,0,1,38,0,0,0,
12,32,0,3,65,78,68,0,13,29,32,0,12,30,0,1,
124,0,0,0,12,32,0,2,79,82,0,0,13,31,32,0,
26,7,8,24,15,6,7,0,12,8,0,3,118,97,108,0,
9,7,1,8,12,8,0,4,78,111,110,101,0,0,0,0,
23,7,7,8,21,7,0,0,18,0,0,11,12,9,0,8,
95,100,111,95,110,111,110,101,0,0,0,0,13,8,9,0,
15,9,2,0,31,7,9,1,19,7,8,7,20,7,0,0,
18,0,0,1,12,8,0,3,118,97,108,0,9,7,1,8,
12,8,0,4,84,114,117,101,0,0,0,0,23,7,7,8,
21,7,0,0,18,0,0,13,12,9,0,10,95,100,111,95,
110,117,109,98,101,114,0,0,13,8,9,0,12,9,0,1,
49,0,0,0,15,10,2,0,31,7,9,2,19,7,8,7,
20,7,0,0,18,0,0,1,12,8,0,3,118,97,108,0,
9,7,1,8,12,8,0,5,70,97,108,115,101,0,0,0,
23,7,7,8,21,7,0,0,18,0,0,13,12,9,0,10,
95,100,111,95,110,117,109,98,101,114,0,0,13,8,9,0,
12,9,0,1,48,0,0,0,15,10,2,0,31,7,9,2,
19,7,8,7,20,7,0,0,18,0,0,1,12,9,0,5,
105,116,101,109,115,0,0,0,9,8,1,9,15,7,8,0,
12,9,0,3,97,110,100,0,12,10,0,2,111,114,0,0,
27,8,9,2,12,10,0,3,118,97,108,0,9,9,1,10,
36,8,8,9,21,8,0,0,18,0,0,36,12,11,0,3,
105,110,116,0,13,10,11,0,12,12,0,3,118,97,108,0,
9,11,1,12,12,12,0,2,111,114,0,0,23,11,11,12,
31,9,11,1,19,9,10,9,15,8,9,0,12,11,0,8,
115,115,95,105,110,102,105,120,0,0,0,0,13,10,11,0,
15,11,8,0,12,17,0,3,118,97,108,0,9,16,1,17,
9,12,6,16,11,16,0,0,0,0,0,0,0,0,0,0,
9,13,7,16,11,16,0,0,0,0,0,0,0,0,240,63,
9,14,7,16,15,15,2,0,31,9,11,5,19,9,10,9,
20,9,0,0,18,0,0,1,12,11,0,3,118,97,108,0,
9,10,1,11,36,9,4,10,21,9,0,0,18,0,0,15,
12,11,0,7,105,109,97,110,97,103,101,0,13,10,11,0,
15,11,1,0,12,13,0,9,100,111,95,115,121,109,98,111,
108,0,0,0,13,12,13,0,31,9,11,2,19,9,10,9,
20,9,0,0,18,0,0,1,12,10,0,3,118,97,108,0,
9,9,1,10,12,10,0,2,105,115,0,0,23,9,9,10,
21,9,0,0,18,0,0,21,12,11,0,5,105,110,102,105,
120,0,0,0,13,10,11,0,12,15,0,2,69,81,0,0,
13,11,15,0,11,15,0,0,0,0,0,0,0,0,0,0,
9,12,7,15,11,15,0,0,0,0,0,0,0,0,240,63,
9,13,7,15,15,14,2,0,31,9,11,4,19,9,10,9,
20,9,0,0,18,0,0,1,12,10,0,3,118,97,108,0,
9,9,1,10,12,10,0,5,105,115,110,111,116,0,0,0,
23,9,9,10,21,9,0,0,18,0,0,21,12,11,0,5,
105,110,102,105,120,0,0,0,13,10,11,0,12,15,0,3,
67,77,80,0,13,11,15,0,11,15,0,0,0,0,0,0,
0,0,0,0,9,12,7,15,11,15,0,0,0,0,0,0,
0,0,240,63,9,13,7,15,15,14,2,0,31,9,11,4,
19,9,10,9,20,9,0,0,18,0,0,1,12,10,0,3,
118,97,108,0,9,9,1,10,12,10,0,3,110,111,116,0,
23,9,9,10,21,9,0,0,18,0,0,32,12,11,0,5,
105,110,102,105,120,0,0,0,13,10,11,0,12,15,0,2,
69,81,0,0,13,11,15,0,12,16,0,5,84,111,107,101,
110,0,0,0,13,15,16,0,12,19,0,3,112,111,115,0,
9,16,1,19,12,17,0,6,110,117,109,98,101,114,0,0,
11,18,0,0,0,0,0,0,0,0,0,0,31,12,16,3,
19,12,15,12,11,15,0,0,0,0,0,0,0,0,0,0,
9,13,7,15,15,14,2,0,31,9,11,4,19,9,10,9,
20,9,0,0,18,0,0,1,12,10,0,3,118,97,108,0,
9,9,1,10,12,10,0,2,105,110,0,0,23,9,9,10,
21,9,0,0,18,0,0,21,12,11,0,5,105,110,102,105,
120,0,0,0,13,10,11,0,12,15,0,3,72,65,83,0,
13,11,15,0,11,15,0,0,0,0,0,0,0,0,240,63,
9,12,7,15,11,15,0,0,0,0,0,0,0,0,0,0,
9,13,7,15,15,14,2,0,31,9,11,4,19,9,10,9,
20,9,0,0,18,0,0,1,12,10,0,3,118,97,108,0,
9,9,1,10,12,10,0,5,110,111,116,105,110,0,0,0,
23,9,9,10,21,9,0,0,18,0,0,51,12,11,0,5,
105,110,102,105,120,0,0,0,13,10,11,0,12,15,0,3,
72,65,83,0,13,11,15,0,11,15,0,0,0,0,0,0,
0,0,240,63,9,12,7,15,11,15,0,0,0,0,0,0,
0,0,0,0,9,13,7,15,15,14,2,0,31,9,11,4,
19,9,10,9,15,2,9,0,12,12,0,10,95,100,111,95,
110,117,109,98,101,114,0,0,13,11,12,0,12,12,0,1,
48,0,0,0,31,10,12,1,19,10,11,10,15,9,10,0,
12,12,0,4,99,111,100,101,0,0,0,0,13,11,12,0,
12,16,0,2,69,81,0,0,13,12,16,0,15,13,2,0,
15,14,2,0,12,17,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,16,17,0,15,17,9,0,31,15,17,1,
19,15,16,15,31,10,12,4,19,10,11,10,20,2,0,0,
18,0,0,1,12,12,0,3,118,97,108,0,9,11,1,12,
36,10,3,11,21,10,0,0,18,0,0,18,12,12,0,10,
100,111,95,115,101,116,95,99,116,120,0,0,13,11,12,0,
11,14,0,0,0,0,0,0,0,0,0,0,9,12,7,14,
11,14,0,0,0,0,0,0,0,0,240,63,9,13,7,14,
31,10,12,2,19,10,11,10,20,10,0,0,18,0,0,118,
12,12,0,3,118,97,108,0,9,11,1,12,36,10,5,11,
21,10,0,0,18,0,0,91,11,12,0,0,0,0,0,0,
0,0,0,0,9,11,7,12,15,10,11,0,11,13,0,0,
0,0,0,0,0,0,240,63,9,12,7,13,15,11,12,0,
15,12,10,0,15,10,11,0,12,14,0,3,118,97,108,0,
9,13,1,14,15,11,13,0,12,14,0,1,62,0,0,0,
12,15,0,2,62,61,0,0,27,13,14,2,11,15,0,0,
0,0,0,0,0,0,0,0,9,14,11,15,36,13,13,14,
21,13,0,0,18,0,0,17,15,13,10,0,15,14,12,0,
12,16,0,1,60,0,0,0,11,19,0,0,0,0,0,0,
0,0,240,63,28,20,0,0,27,18,19,2,9,17,11,18,
1,16,16,17,15,15,16,0,15,12,13,0,15,10,14,0,
15,11,15,0,18,0,0,1,12,15,0,2,69,81,0,0,
13,14,15,0,15,13,14,0,12,15,0,1,60,0,0,0,
23,14,11,15,21,14,0,0,18,0,0,6,12,15,0,2,
76,84,0,0,13,14,15,0,15,13,14,0,18,0,0,1,
12,15,0,2,60,61,0,0,23,14,11,15,21,14,0,0,
18,0,0,6,12,15,0,2,76,69,0,0,13,14,15,0,
15,13,14,0,18,0,0,1,12,15,0,2,33,61,0,0,
23,14,11,15,21,14,0,0,18,0,0,6,12,15,0,2,
78,69,0,0,13,14,15,0,15,13,14,0,18,0,0,1,
12,16,0,5,105,110,102,105,120,0,0,0,13,15,16,0,
15,16,13,0,15,17,12,0,15,18,10,0,15,19,2,0,
31,14,16,4,19,14,15,14,20,14,0,0,18,0,0,22,
12,16,0,5,105,110,102,105,120,0,0,0,13,15,16,0,
12,21,0,3,118,97,108,0,9,20,1,21,9,16,6,20,
11,20,0,0,0,0,0,0,0,0,0,0,9,17,7,20,
11,20,0,0,0,0,0,0,0,0,240,63,9,18,7,20,
15,19,2,0,31,14,16,4,19,14,15,14,20,14,0,0,
18,0,0,1,0,0,0,0,12,38,0,9,100,111,95,115,
121,109,98,111,108,0,0,0,14,38,37,0,16,38,1,221,
44,34,0,0,28,2,0,0,9,1,0,2,28,3,0,0,
9,2,0,3,12,4,0,4,116,121,112,101,0,0,0,0,
9,3,1,4,12,4,0,4,110,97,109,101,0,0,0,0,
23,3,3,4,21,3,0,0,18,0,0,136,11,5,0,0,
0,0,0,0,0,0,240,63,11,7,0,0,0,0,0,0,
0,0,0,0,12,8,0,1,68,0,0,0,13,3,8,0,
12,8,0,8,95,103,108,111,98,97,108,115,0,0,0,0,
9,3,3,8,23,6,3,7,21,6,0,0,18,0,0,2,
18,0,0,16,12,8,0,1,68,0,0,0,13,3,8,0,
12,8,0,4,118,97,114,115,0,0,0,0,9,3,3,8,
12,9,0,3,118,97,108,0,9,8,1,9,36,3,3,8,
11,8,0,0,0,0,0,0,0,0,0,0,23,3,3,8,
23,4,3,5,21,4,0,0,18,0,0,2,18,0,0,12,
12,6,0,1,68,0,0,0,13,3,6,0,12,6,0,7,
103,108,111,98,97,108,115,0,9,3,3,6,12,7,0,3,
118,97,108,0,9,6,1,7,36,3,3,6,21,3,0,0,
18,0,0,48,12,6,0,9,100,111,95,115,116,114,105,110,
103,0,0,0,13,5,6,0,15,6,1,0,31,4,6,1,
19,4,5,4,15,3,4,0,12,7,0,2,100,111,0,0,
13,6,7,0,15,7,2,0,31,5,7,1,19,5,6,5,
15,4,5,0,12,7,0,4,99,111,100,101,0,0,0,0,
13,6,7,0,12,10,0,4,71,83,69,84,0,0,0,0,
13,7,10,0,15,8,3,0,15,9,4,0,31,5,7,3,
19,5,6,5,12,7,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,6,7,0,15,7,3,0,31,5,7,1,
19,5,6,5,12,7,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,6,7,0,15,7,4,0,31,5,7,1,
19,5,6,5,28,5,0,0,20,5,0,0,18,0,0,1,
12,8,0,8,100,111,95,108,111,99,97,108,0,0,0,0,
13,7,8,0,15,8,1,0,31,6,8,1,19,6,7,6,
15,5,6,0,12,8,0,2,100,111,0,0,13,7,8,0,
15,8,2,0,31,6,8,1,19,6,7,6,15,4,6,0,
12,8,0,4,99,111,100,101,0,0,0,0,13,7,8,0,
12,11,0,4,77,79,86,69,0,0,0,0,13,8,11,0,
15,9,5,0,15,10,4,0,31,6,8,3,19,6,7,6,
12,8,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,7,8,0,15,8,4,0,31,6,8,1,19,6,7,6,
20,5,0,0,18,0,1,6,12,7,0,5,116,117,112,108,
101,0,0,0,12,8,0,4,108,105,115,116,0,0,0,0,
27,6,7,2,12,8,0,4,116,121,112,101,0,0,0,0,
9,7,1,8,36,6,6,7,21,6,0,0,18,0,0,248,
12,7,0,5,116,117,112,108,101,0,0,0,12,8,0,4,
108,105,115,116,0,0,0,0,27,6,7,2,12,8,0,4,
116,121,112,101,0,0,0,0,9,7,2,8,36,6,6,7,
21,6,0,0,18,0,0,128,11,7,0,0,0,0,0,0,
0,0,0,0,15,6,7,0,27,8,0,0,15,7,8,0,
15,8,6,0,15,6,7,0,12,10,0,5,105,116,101,109,
115,0,0,0,9,9,1,10,11,10,0,0,0,0,0,0,
0,0,0,0,42,7,9,10,18,0,0,54,12,13,0,5,
105,116,101,109,115,0,0,0,9,12,2,13,9,12,12,8,
15,11,12,0,12,15,0,7,103,101,116,95,116,109,112,0,
13,14,15,0,31,13,0,0,19,13,14,13,15,12,13,0,
12,15,0,6,97,112,112,101,110,100,0,0,9,14,6,15,
15,15,12,0,31,13,15,1,19,13,14,13,12,16,0,2,
100,111,0,0,13,15,16,0,15,16,11,0,31,14,16,1,
19,14,15,14,15,13,14,0,12,16,0,4,99,111,100,101,
0,0,0,0,13,15,16,0,12,19,0,4,77,79,86,69,
0,0,0,0,13,16,19,0,15,17,12,0,15,18,13,0,
31,14,16,3,19,14,15,14,12,16,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,15,16,0,15,16,13,0,
31,14,16,1,19,14,15,14,11,15,0,0,0,0,0,0,
0,0,240,63,1,14,8,15,15,8,14,0,18,0,255,202,
11,9,0,0,0,0,0,0,0,0,0,0,15,8,9,0,
12,10,0,5,105,116,101,109,115,0,0,0,9,9,1,10,
11,10,0,0,0,0,0,0,0,0,0,0,42,7,9,10,
18,0,0,42,12,15,0,5,105,116,101,109,115,0,0,0,
9,14,2,15,9,14,14,8,15,11,14,0,9,14,6,8,
15,12,14,0,12,16,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,15,16,0,12,18,0,10,100,111,95,115,
101,116,95,99,116,120,0,0,13,17,18,0,15,18,7,0,
12,21,0,5,84,111,107,101,110,0,0,0,13,20,21,0,
12,24,0,3,112,111,115,0,9,21,11,24,12,22,0,3,
114,101,103,0,15,23,12,0,31,19,21,3,19,19,20,19,
31,16,18,2,19,16,17,16,31,14,16,1,19,14,15,14,
11,15,0,0,0,0,0,0,0,0,240,63,1,14,8,15,
15,8,14,0,18,0,255,214,28,9,0,0,20,9,0,0,
18,0,0,1,12,14,0,2,100,111,0,0,13,10,14,0,
15,14,2,0,31,9,14,1,19,9,10,9,15,13,9,0,
12,14,0,6,117,110,95,116,109,112,0,0,13,10,14,0,
15,14,13,0,31,9,14,1,19,9,10,9,11,10,0,0,
0,0,0,0,0,0,0,0,15,9,10,0,12,16,0,5,
84,111,107,101,110,0,0,0,13,15,16,0,12,19,0,3,
112,111,115,0,9,16,2,19,12,17,0,3,114,101,103,0,
15,18,13,0,31,14,16,3,19,14,15,14,15,10,14,0,
15,8,9,0,15,12,10,0,12,14,0,5,105,116,101,109,
115,0,0,0,9,10,1,14,11,14,0,0,0,0,0,0,
0,0,0,0,42,9,10,14,18,0,0,54,12,17,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,16,17,0,
12,19,0,10,100,111,95,115,101,116,95,99,116,120,0,0,
13,18,19,0,15,19,9,0,12,22,0,5,84,111,107,101,
110,0,0,0,13,21,22,0,12,26,0,3,112,111,115,0,
9,22,12,26,12,23,0,3,103,101,116,0,28,24,0,0,
15,26,12,0,12,29,0,5,84,111,107,101,110,0,0,0,
13,28,29,0,12,32,0,3,112,111,115,0,9,29,12,32,
12,30,0,6,110,117,109,98,101,114,0,0,12,33,0,3,
115,116,114,0,13,32,33,0,15,33,8,0,31,31,33,1,
19,31,32,31,31,27,29,3,19,27,28,27,27,25,26,2,
31,20,22,4,19,20,21,20,31,17,19,2,19,17,18,17,
31,15,17,1,19,15,16,15,11,16,0,0,0,0,0,0,
0,0,240,63,1,15,8,16,15,8,15,0,18,0,255,202,
12,15,0,8,102,114,101,101,95,114,101,103,0,0,0,0,
13,14,15,0,15,15,13,0,31,10,15,1,19,10,14,10,
28,10,0,0,20,10,0,0,18,0,0,1,12,15,0,2,
100,111,0,0,13,14,15,0,12,16,0,5,105,116,101,109,
115,0,0,0,9,15,1,16,11,16,0,0,0,0,0,0,
0,0,0,0,9,15,15,16,31,10,15,1,19,10,14,10,
15,13,10,0,12,16,0,2,100,111,0,0,13,15,16,0,
15,16,2,0,31,14,16,1,19,14,15,14,15,10,14,0,
12,16,0,2,100,111,0,0,13,15,16,0,12,17,0,5,
105,116,101,109,115,0,0,0,9,16,1,17,11,17,0,0,
0,0,0,0,0,0,240,63,9,16,16,17,31,14,16,1,
19,14,15,14,15,12,14,0,12,16,0,4,99,111,100,101,
0,0,0,0,13,15,16,0,12,20,0,3,83,69,84,0,
13,16,20,0,15,17,13,0,15,18,12,0,15,19,10,0,
31,14,16,4,19,14,15,14,12,16,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,15,16,0,15,16,13,0,
31,14,16,1,19,14,15,14,12,16,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,15,16,0,15,16,12,0,
31,14,16,1,19,14,15,14,20,10,0,0,0,0,0,0,
12,39,0,10,100,111,95,115,101,116,95,99,116,120,0,0,
14,39,38,0,16,39,0,152,44,19,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,28,4,0,0,
9,3,0,4,11,4,0,0,0,0,0,0,0,0,0,0,
28,5,0,0,32,4,0,5,12,8,0,3,109,97,120,0,
13,7,8,0,15,8,4,0,12,11,0,3,108,101,110,0,
13,10,11,0,15,11,3,0,31,9,11,1,19,9,10,9,
31,6,8,2,19,6,7,6,15,5,6,0,11,7,0,0,
0,0,0,0,0,0,0,0,15,6,7,0,12,10,0,8,
103,101,116,95,116,109,112,115,0,0,0,0,13,9,10,0,
15,10,5,0,31,8,10,1,19,8,9,8,15,7,8,0,
15,8,6,0,15,6,7,0,11,9,0,0,0,0,0,0,
0,0,0,0,42,7,3,9,18,0,0,41,9,11,6,8,
15,10,11,0,12,14,0,2,100,111,0,0,13,13,14,0,
15,14,7,0,15,15,10,0,31,12,14,2,19,12,13,12,
15,11,12,0,35,12,10,11,21,12,0,0,18,0,0,22,
12,14,0,4,99,111,100,101,0,0,0,0,13,13,14,0,
12,17,0,4,77,79,86,69,0,0,0,0,13,14,17,0,
15,15,10,0,15,16,11,0,31,12,14,3,19,12,13,12,
12,14,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,13,14,0,15,14,11,0,31,12,14,1,19,12,13,12,
18,0,0,1,11,13,0,0,0,0,0,0,0,0,240,63,
1,12,8,13,15,8,12,0,18,0,255,215,11,9,0,0,
0,0,0,0,0,0,0,0,12,14,0,3,108,101,110,0,
13,13,14,0,15,14,6,0,31,12,14,1,19,12,13,12,
23,9,9,12,21,9,0,0,18,0,0,20,12,13,0,4,
99,111,100,101,0,0,0,0,13,12,13,0,15,13,1,0,
15,14,2,0,11,15,0,0,0,0,0,0,0,0,0,0,
11,16,0,0,0,0,0,0,0,0,0,0,31,9,13,4,
19,9,12,9,11,9,0,0,0,0,0,0,0,0,0,0,
20,9,0,0,18,0,0,1,12,13,0,4,99,111,100,101,
0,0,0,0,13,12,13,0,15,13,1,0,15,14,2,0,
11,17,0,0,0,0,0,0,0,0,0,0,9,15,6,17,
12,18,0,3,108,101,110,0,13,17,18,0,15,18,3,0,
31,16,18,1,19,16,17,16,31,9,13,4,19,9,12,9,
12,13,0,9,102,114,101,101,95,116,109,112,115,0,0,0,
13,12,13,0,15,15,4,0,28,16,0,0,27,14,15,2,
9,13,6,14,31,9,13,1,19,9,12,9,11,12,0,0,
0,0,0,0,0,0,0,0,9,9,6,12,20,9,0,0,
0,0,0,0,12,40,0,10,109,97,110,97,103,101,95,115,
101,113,0,0,14,40,39,0,16,40,0,92,44,12,0,0,
28,2,0,0,9,1,0,2,27,3,0,0,15,2,3,0,
27,4,0,0,15,3,4,0,28,5,0,0,15,4,5,0,
28,6,0,0,15,5,6,0,15,6,2,0,15,2,3,0,
15,3,4,0,15,4,5,0,11,7,0,0,0,0,0,0,
0,0,0,0,42,5,1,7,18,0,0,65,11,10,0,0,
0,0,0,0,0,0,0,0,12,11,0,4,116,121,112,101,
0,0,0,0,9,8,5,11,12,11,0,6,115,121,109,98,
111,108,0,0,23,8,8,11,23,9,8,10,21,9,0,0,
18,0,0,2,18,0,0,7,12,11,0,3,118,97,108,0,
9,8,5,11,12,11,0,1,61,0,0,0,23,8,8,11,
21,8,0,0,18,0,0,9,12,10,0,6,97,112,112,101,
110,100,0,0,9,9,2,10,15,10,5,0,31,8,10,1,
19,8,9,8,18,0,0,33,12,9,0,4,116,121,112,101,
0,0,0,0,9,8,5,9,12,9,0,4,97,114,103,115,
0,0,0,0,23,8,8,9,21,8,0,0,18,0,0,3,
15,3,5,0,18,0,0,21,12,9,0,4,116,121,112,101,
0,0,0,0,9,8,5,9,12,9,0,5,110,97,114,103,
115,0,0,0,23,8,8,9,21,8,0,0,18,0,0,3,
15,4,5,0,18,0,0,9,12,10,0,6,97,112,112,101,
110,100,0,0,9,9,6,10,15,10,5,0,31,8,10,1,
19,8,9,8,18,0,0,1,18,0,255,191,15,8,6,0,
15,9,2,0,15,10,3,0,15,11,4,0,27,7,8,4,
20,7,0,0,0,0,0,0,12,41,0,8,112,95,102,105,
108,116,101,114,0,0,0,0,14,41,40,0,16,41,0,86,
44,21,0,0,28,2,0,0,9,1,0,2,12,4,0,5,
105,116,101,109,115,0,0,0,9,3,1,4,11,4,0,0,
0,0,0,0,0,0,0,0,42,2,3,4,18,0,0,73,
12,5,0,6,115,116,114,105,110,103,0,0,12,6,0,4,
116,121,112,101,0,0,0,0,10,2,6,5,12,8,0,7,
100,111,95,99,97,108,108,0,13,7,8,0,12,10,0,5,
84,111,107,101,110,0,0,0,13,9,10,0,12,14,0,3,
112,111,115,0,9,10,1,14,12,11,0,4,99,97,108,108,
0,0,0,0,28,12,0,0,12,17,0,5,84,111,107,101,
110,0,0,0,13,16,17,0,12,20,0,3,112,111,115,0,
9,17,1,20,12,18,0,4,110,97,109,101,0,0,0,0,
12,19,0,6,105,109,112,111,114,116,0,0,31,14,17,3,
19,14,16,14,15,15,2,0,27,13,14,2,31,8,10,4,
19,8,9,8,31,6,8,1,19,6,7,6,15,5,6,0,
12,6,0,4,110,97,109,101,0,0,0,0,12,7,0,4,
116,121,112,101,0,0,0,0,10,2,7,6,12,8,0,10,
100,111,95,115,101,116,95,99,116,120,0,0,13,7,8,0,
15,8,2,0,12,11,0,5,84,111,107,101,110,0,0,0,
13,10,11,0,12,14,0,3,112,111,115,0,9,11,1,14,
12,12,0,3,114,101,103,0,15,13,5,0,31,9,11,3,
19,9,10,9,31,6,8,2,19,6,7,6,18,0,255,183,
0,0,0,0,12,42,0,9,100,111,95,105,109,112,111,114,
116,0,0,0,14,42,41,0,16,42,0,222,44,23,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,105,116,101,109,
115,0,0,0,9,3,1,4,11,4,0,0,0,0,0,0,
0,0,0,0,9,3,3,4,15,2,3,0,12,3,0,6,
115,116,114,105,110,103,0,0,12,4,0,4,116,121,112,101,
0,0,0,0,10,2,4,3,12,6,0,2,100,111,0,0,
13,5,6,0,12,8,0,5,84,111,107,101,110,0,0,0,
13,7,8,0,12,12,0,3,112,111,115,0,9,8,1,12,
12,9,0,4,99,97,108,108,0,0,0,0,28,10,0,0,
12,15,0,5,84,111,107,101,110,0,0,0,13,14,15,0,
12,18,0,3,112,111,115,0,9,15,1,18,12,16,0,4,
110,97,109,101,0,0,0,0,12,17,0,6,105,109,112,111,
114,116,0,0,31,12,15,3,19,12,14,12,15,13,2,0,
27,11,12,2,31,6,8,4,19,6,7,6,31,4,6,1,
19,4,5,4,15,3,4,0,12,6,0,5,105,116,101,109,
115,0,0,0,9,5,1,6,11,6,0,0,0,0,0,0,
0,0,240,63,9,5,5,6,15,4,5,0,12,6,0,3,
118,97,108,0,9,5,4,6,12,6,0,1,42,0,0,0,
23,5,5,6,21,5,0,0,18,0,0,71,12,7,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,6,7,0,
12,9,0,2,100,111,0,0,13,8,9,0,12,11,0,5,
84,111,107,101,110,0,0,0,13,10,11,0,12,15,0,3,
112,111,115,0,9,11,1,15,12,12,0,4,99,97,108,108,
0,0,0,0,28,13,0,0,12,19,0,5,84,111,107,101,
110,0,0,0,13,18,19,0,12,22,0,3,112,111,115,0,
9,19,1,22,12,20,0,4,110,97,109,101,0,0,0,0,
12,21,0,5,109,101,114,103,101,0,0,0,31,15,19,3,
19,15,18,15,12,19,0,5,84,111,107,101,110,0,0,0,
13,18,19,0,12,22,0,3,112,111,115,0,9,19,1,22,
12,20,0,4,110,97,109,101,0,0,0,0,12,21,0,8,
95,95,100,105,99,116,95,95,0,0,0,0,31,16,19,3,
19,16,18,16,12,19,0,5,84,111,107,101,110,0,0,0,
13,18,19,0,12,22,0,3,112,111,115,0,9,19,1,22,
12,20,0,3,114,101,103,0,15,21,3,0,31,17,19,3,
19,17,18,17,27,14,15,3,31,9,11,4,19,9,10,9,
31,7,9,1,19,7,8,7,31,5,7,1,19,5,6,5,
18,0,0,79,12,5,0,6,115,116,114,105,110,103,0,0,
12,6,0,4,116,121,112,101,0,0,0,0,10,4,6,5,
12,7,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,6,7,0,12,9,0,10,100,111,95,115,101,116,95,99,
116,120,0,0,13,8,9,0,12,12,0,5,84,111,107,101,
110,0,0,0,13,11,12,0,12,16,0,3,112,111,115,0,
9,12,1,16,12,13,0,3,103,101,116,0,28,14,0,0,
12,19,0,5,84,111,107,101,110,0,0,0,13,18,19,0,
12,22,0,3,112,111,115,0,9,19,1,22,12,20,0,4,
110,97,109,101,0,0,0,0,12,21,0,8,95,95,100,105,
99,116,95,95,0,0,0,0,31,16,19,3,19,16,18,16,
15,17,4,0,27,15,16,2,31,9,12,4,19,9,11,9,
12,12,0,5,84,111,107,101,110,0,0,0,13,11,12,0,
12,16,0,3,112,111,115,0,9,12,1,16,12,13,0,3,
103,101,116,0,28,14,0,0,12,19,0,5,84,111,107,101,
110,0,0,0,13,18,19,0,12,22,0,3,112,111,115,0,
9,19,1,22,12,20,0,3,114,101,103,0,15,21,3,0,
31,16,19,3,19,16,18,16,15,17,4,0,27,15,16,2,
31,10,12,4,19,10,11,10,31,7,9,2,19,7,8,7,
31,5,7,1,19,5,6,5,18,0,0,1,0,0,0,0,
12,43,0,7,100,111,95,102,114,111,109,0,14,43,42,0,
16,43,0,49,44,8,0,0,28,2,0,0,9,1,0,2,
12,3,0,5,105,116,101,109,115,0,0,0,9,2,1,3,
11,3,0,0,0,0,0,0,0,0,0,0,42,1,2,3,
18,0,0,36,12,5,0,1,68,0,0,0,13,4,5,0,
12,5,0,7,103,108,111,98,97,108,115,0,9,4,4,5,
12,6,0,3,118,97,108,0,9,5,1,6,36,4,4,5,
11,5,0,0,0,0,0,0,0,0,0,0,23,4,4,5,
21,4,0,0,18,0,0,18,12,6,0,1,68,0,0,0,
13,5,6,0,12,6,0,7,103,108,111,98,97,108,115,0,
9,5,5,6,12,6,0,6,97,112,112,101,110,100,0,0,
9,5,5,6,12,7,0,3,118,97,108,0,9,6,1,7,
31,4,6,1,19,4,5,4,18,0,0,1,18,0,255,220,
0,0,0,0,12,44,0,10,100,111,95,103,108,111,98,97,
108,115,0,0,14,44,43,0,16,44,0,70,44,13,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,105,116,101,109,
115,0,0,0,9,3,1,4,11,4,0,0,0,0,0,0,
0,0,0,0,42,2,3,4,18,0,0,57,12,8,0,2,
100,111,0,0,13,7,8,0,12,9,0,5,105,116,101,109,
115,0,0,0,9,8,2,9,11,9,0,0,0,0,0,0,
0,0,0,0,9,8,8,9,31,6,8,1,19,6,7,6,
15,5,6,0,12,9,0,2,100,111,0,0,13,8,9,0,
12,10,0,5,105,116,101,109,115,0,0,0,9,9,2,10,
11,10,0,0,0,0,0,0,0,0,240,63,9,9,9,10,
31,7,9,1,19,7,8,7,15,6,7,0,12,9,0,4,
99,111,100,101,0,0,0,0,13,8,9,0,12,12,0,3,
68,69,76,0,13,9,12,0,15,10,5,0,15,11,6,0,
31,7,9,3,19,7,8,7,12,9,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,8,9,0,15,9,5,0,
31,7,9,1,19,7,8,7,12,9,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,8,9,0,15,9,6,0,
31,7,9,1,19,7,8,7,18,0,255,199,0,0,0,0,
12,45,0,6,100,111,95,100,101,108,0,0,14,45,44,0,
16,45,1,133,44,31,0,0,28,2,0,0,9,1,0,2,
28,2,0,0,28,3,0,0,32,2,0,3,12,5,0,7,
103,101,116,95,116,109,112,0,13,4,5,0,15,5,2,0,
31,3,5,1,19,3,4,3,15,2,3,0,12,5,0,5,
105,116,101,109,115,0,0,0,9,4,1,5,15,3,4,0,
12,7,0,2,100,111,0,0,13,6,7,0,11,8,0,0,
0,0,0,0,0,0,0,0,9,7,3,8,31,5,7,1,
19,5,6,5,15,4,5,0,12,7,0,8,112,95,102,105,
108,116,101,114,0,0,0,0,13,6,7,0,12,8,0,5,
105,116,101,109,115,0,0,0,9,7,1,8,11,9,0,0,
0,0,0,0,0,0,240,63,28,10,0,0,27,8,9,2,
9,7,7,8,31,5,7,1,19,5,6,5,11,8,0,0,
0,0,0,0,0,0,0,0,9,7,5,8,15,6,7,0,
11,9,0,0,0,0,0,0,0,0,240,63,9,8,5,9,
15,7,8,0,11,10,0,0,0,0,0,0,0,0,0,64,
9,9,5,10,15,8,9,0,11,11,0,0,0,0,0,0,
0,0,8,64,9,10,5,11,15,9,10,0,28,10,0,0,
15,5,10,0,11,12,0,0,0,0,0,0,0,0,240,63,
12,14,0,3,108,101,110,0,13,13,14,0,15,14,7,0,
31,10,14,1,19,10,13,10,11,13,0,0,0,0,0,0,
0,0,0,0,35,10,10,13,23,11,10,12,21,11,0,0,
18,0,0,2,18,0,0,3,28,13,0,0,35,10,9,13,
21,10,0,0,18,0,0,172,12,12,0,2,100,111,0,0,
13,11,12,0,12,14,0,5,84,111,107,101,110,0,0,0,
13,13,14,0,12,18,0,3,112,111,115,0,9,14,1,18,
12,15,0,4,100,105,99,116,0,0,0,0,28,16,0,0,
27,17,0,0,31,12,14,4,19,12,13,12,31,10,12,1,
19,10,11,10,15,5,10,0,12,12,0,6,117,110,95,116,
109,112,0,0,13,11,12,0,15,12,5,0,31,10,12,1,
19,10,11,10,11,11,0,0,0,0,0,0,0,0,0,0,
42,10,7,11,18,0,0,75,12,13,0,5,105,116,101,109,
115,0,0,0,9,12,10,13,11,13,0,0,0,0,0,0,
0,0,0,0,9,12,12,13,12,13,0,6,115,116,114,105,
110,103,0,0,12,14,0,4,116,121,112,101,0,0,0,0,
10,12,14,13,12,15,0,2,100,111,0,0,13,14,15,0,
12,16,0,5,105,116,101,109,115,0,0,0,9,15,10,16,
11,16,0,0,0,0,0,0,0,0,0,0,9,15,15,16,
31,13,15,1,19,13,14,13,15,12,13,0,12,16,0,2,
100,111,0,0,13,15,16,0,12,17,0,5,105,116,101,109,
115,0,0,0,9,16,10,17,11,17,0,0,0,0,0,0,
0,0,240,63,9,16,16,17,31,14,16,1,19,14,15,14,
15,13,14,0,15,14,12,0,15,12,13,0,12,16,0,4,
99,111,100,101,0,0,0,0,13,15,16,0,12,20,0,3,
83,69,84,0,13,16,20,0,15,17,5,0,15,18,14,0,
15,19,12,0,31,13,16,4,19,13,15,13,12,16,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,15,16,0,
15,16,14,0,31,13,16,1,19,13,15,13,12,16,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,15,16,0,
15,16,12,0,31,13,16,1,19,13,15,13,18,0,255,181,
21,9,0,0,18,0,0,63,12,15,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,13,15,0,12,17,0,2,
100,111,0,0,13,16,17,0,12,19,0,5,84,111,107,101,
110,0,0,0,13,18,19,0,12,23,0,3,112,111,115,0,
9,19,1,23,12,20,0,4,99,97,108,108,0,0,0,0,
28,21,0,0,12,27,0,5,84,111,107,101,110,0,0,0,
13,26,27,0,12,30,0,3,112,111,115,0,9,27,1,30,
12,28,0,4,110,97,109,101,0,0,0,0,12,29,0,5,
109,101,114,103,101,0,0,0,31,23,27,3,19,23,26,23,
12,27,0,5,84,111,107,101,110,0,0,0,13,26,27,0,
12,30,0,3,112,111,115,0,9,27,1,30,12,28,0,3,
114,101,103,0,15,29,5,0,31,24,27,3,19,24,26,24,
12,26,0,5,105,116,101,109,115,0,0,0,9,25,9,26,
11,26,0,0,0,0,0,0,0,0,0,0,9,25,25,26,
27,22,23,3,31,17,19,4,19,17,18,17,31,15,17,1,
19,15,16,15,31,11,15,1,19,11,13,11,18,0,0,1,
18,0,0,1,12,15,0,10,109,97,110,97,103,101,95,115,
101,113,0,0,13,13,15,0,12,18,0,6,80,65,82,65,
77,83,0,0,13,15,18,0,15,16,2,0,15,17,6,0,
31,11,15,3,19,11,13,11,28,13,0,0,35,11,8,13,
21,11,0,0,18,0,0,56,12,16,0,10,95,100,111,95,
115,116,114,105,110,103,0,0,13,15,16,0,12,16,0,1,
42,0,0,0,31,13,16,1,19,13,15,13,15,11,13,0,
12,17,0,2,100,111,0,0,13,16,17,0,12,18,0,5,
105,116,101,109,115,0,0,0,9,17,8,18,11,18,0,0,
0,0,0,0,0,0,0,0,9,17,17,18,31,15,17,1,
19,15,16,15,15,13,15,0,15,14,11,0,15,12,13,0,
12,15,0,4,99,111,100,101,0,0,0,0,13,13,15,0,
12,19,0,3,83,69,84,0,13,15,19,0,15,16,2,0,
15,17,14,0,15,18,12,0,31,11,15,4,19,11,13,11,
12,15,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,13,15,0,15,15,14,0,31,11,15,1,19,11,13,11,
12,15,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,13,15,0,15,15,12,0,31,11,15,1,19,11,13,11,
18,0,0,1,28,13,0,0,35,11,5,13,21,11,0,0,
18,0,0,30,12,15,0,8,95,100,111,95,110,111,110,101,
0,0,0,0,13,13,15,0,31,11,0,0,19,11,13,11,
15,14,11,0,12,15,0,4,99,111,100,101,0,0,0,0,
13,13,15,0,12,19,0,3,83,69,84,0,13,15,19,0,
15,16,2,0,15,17,14,0,15,18,5,0,31,11,15,4,
19,11,13,11,12,15,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,13,15,0,15,15,14,0,31,11,15,1,
19,11,13,11,18,0,0,1,12,15,0,4,99,111,100,101,
0,0,0,0,13,13,15,0,12,19,0,4,67,65,76,76,
0,0,0,0,13,15,19,0,15,16,2,0,15,17,4,0,
15,18,2,0,31,11,15,4,19,11,13,11,12,15,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,13,15,0,
15,15,4,0,31,11,15,1,19,11,13,11,20,2,0,0,
0,0,0,0,12,46,0,7,100,111,95,99,97,108,108,0,
14,46,45,0,16,46,0,70,44,10,0,0,28,2,0,0,
9,1,0,2,28,2,0,0,28,3,0,0,32,2,0,3,
12,4,0,1,68,0,0,0,13,3,4,0,12,4,0,4,
118,97,114,115,0,0,0,0,9,3,3,4,12,5,0,3,
118,97,108,0,9,4,1,5,36,3,3,4,21,3,0,0,
18,0,0,12,12,5,0,8,100,111,95,108,111,99,97,108,
0,0,0,0,13,4,5,0,15,5,1,0,15,6,2,0,
31,3,5,2,19,3,4,3,20,3,0,0,18,0,0,1,
12,5,0,7,103,101,116,95,116,109,112,0,13,4,5,0,
15,5,2,0,31,3,5,1,19,3,4,3,15,2,3,0,
12,6,0,9,100,111,95,115,116,114,105,110,103,0,0,0,
13,5,6,0,15,6,1,0,31,4,6,1,19,4,5,4,
15,3,4,0,12,6,0,4,99,111,100,101,0,0,0,0,
13,5,6,0,12,9,0,4,71,71,69,84,0,0,0,0,
13,6,9,0,15,7,2,0,15,8,3,0,31,4,6,3,
19,4,5,4,12,6,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,5,6,0,15,6,3,0,31,4,6,1,
19,4,5,4,20,2,0,0,0,0,0,0,12,47,0,7,
100,111,95,110,97,109,101,0,14,47,46,0,16,47,0,52,
44,7,0,0,28,2,0,0,9,1,0,2,28,2,0,0,
28,3,0,0,32,2,0,3,12,4,0,1,68,0,0,0,
13,3,4,0,12,4,0,4,118,97,114,115,0,0,0,0,
9,3,3,4,12,5,0,3,118,97,108,0,9,4,1,5,
36,3,3,4,11,4,0,0,0,0,0,0,0,0,0,0,
23,3,3,4,21,3,0,0,18,0,0,18,12,5,0,1,
68,0,0,0,13,4,5,0,12,5,0,4,118,97,114,115,
0,0,0,0,9,4,4,5,12,5,0,6,97,112,112,101,
110,100,0,0,9,4,4,5,12,6,0,3,118,97,108,0,
9,5,1,6,31,3,5,1,19,3,4,3,18,0,0,1,
12,5,0,7,103,101,116,95,114,101,103,0,13,4,5,0,
12,6,0,3,118,97,108,0,9,5,1,6,31,3,5,1,
19,3,4,3,20,3,0,0,0,0,0,0,12,48,0,8,
100,111,95,108,111,99,97,108,0,0,0,0,14,48,47,0,
16,48,1,229,44,25,0,0,28,2,0,0,9,1,0,2,
28,2,0,0,28,3,0,0,32,2,0,3,12,5,0,5,
105,116,101,109,115,0,0,0,9,4,1,5,15,3,4,0,
12,7,0,7,103,101,116,95,116,97,103,0,13,6,7,0,
31,5,0,0,19,5,6,5,15,4,5,0,12,8,0,3,
102,110,99,0,13,7,8,0,15,8,4,0,12,9,0,3,
101,110,100,0,31,6,8,2,19,6,7,6,15,5,6,0,
12,8,0,1,68,0,0,0,13,7,8,0,12,8,0,5,
98,101,103,105,110,0,0,0,9,7,7,8,31,6,0,0,
19,6,7,6,12,8,0,6,115,101,116,112,111,115,0,0,
13,7,8,0,12,9,0,3,112,111,115,0,9,8,1,9,
31,6,8,1,19,6,7,6,12,9,0,8,100,111,95,108,
111,99,97,108,0,0,0,0,13,8,9,0,12,11,0,5,
84,111,107,101,110,0,0,0,13,10,11,0,12,14,0,3,
112,111,115,0,9,11,1,14,12,12,0,4,110,97,109,101,
0,0,0,0,12,13,0,8,95,95,112,97,114,97,109,115,
0,0,0,0,31,9,11,3,19,9,10,9,31,7,9,1,
19,7,8,7,15,6,7,0,12,9,0,7,100,111,95,105,
110,102,111,0,13,8,9,0,11,10,0,0,0,0,0,0,
0,0,0,0,9,9,3,10,12,10,0,3,118,97,108,0,
9,9,9,10,31,7,9,1,19,7,8,7,12,9,0,8,
112,95,102,105,108,116,101,114,0,0,0,0,13,8,9,0,
11,10,0,0,0,0,0,0,0,0,240,63,9,9,3,10,
12,10,0,5,105,116,101,109,115,0,0,0,9,9,9,10,
31,7,9,1,19,7,8,7,11,10,0,0,0,0,0,0,
0,0,0,0,9,9,7,10,15,8,9,0,11,11,0,0,
0,0,0,0,0,0,240,63,9,10,7,11,15,9,10,0,
11,12,0,0,0,0,0,0,0,0,0,64,9,11,7,12,
15,10,11,0,11,13,0,0,0,0,0,0,0,0,8,64,
9,12,7,13,15,11,12,0,11,12,0,0,0,0,0,0,
0,0,0,0,42,7,8,12,18,0,0,39,12,16,0,8,
100,111,95,108,111,99,97,108,0,0,0,0,13,15,16,0,
15,16,7,0,31,14,16,1,19,14,15,14,15,13,14,0,
12,17,0,8,95,100,111,95,110,111,110,101,0,0,0,0,
13,16,17,0,31,15,0,0,19,15,16,15,15,14,15,0,
12,17,0,4,99,111,100,101,0,0,0,0,13,16,17,0,
12,21,0,3,71,69,84,0,13,17,21,0,15,18,13,0,
15,19,6,0,15,20,14,0,31,15,17,4,19,15,16,15,
12,17,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,16,17,0,15,17,14,0,31,15,17,1,19,15,16,15,
18,0,255,217,11,12,0,0,0,0,0,0,0,0,0,0,
42,7,9,12,18,0,0,61,12,17,0,8,100,111,95,108,
111,99,97,108,0,0,0,0,13,16,17,0,12,18,0,5,
105,116,101,109,115,0,0,0,9,17,7,18,11,18,0,0,
0,0,0,0,0,0,0,0,9,17,17,18,31,15,17,1,
19,15,16,15,15,13,15,0,12,17,0,2,100,111,0,0,
13,16,17,0,12,19,0,5,105,116,101,109,115,0,0,0,
9,17,7,19,11,19,0,0,0,0,0,0,0,0,240,63,
9,17,17,19,15,18,13,0,31,15,17,2,19,15,16,15,
12,17,0,8,95,100,111,95,110,111,110,101,0,0,0,0,
13,16,17,0,31,15,0,0,19,15,16,15,15,14,15,0,
12,17,0,4,99,111,100,101,0,0,0,0,13,16,17,0,
12,21,0,4,73,71,69,84,0,0,0,0,13,17,21,0,
15,18,13,0,15,19,6,0,15,20,14,0,31,15,17,4,
19,15,16,15,12,17,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,16,17,0,15,17,14,0,31,15,17,1,
19,15,16,15,18,0,255,195,28,15,0,0,35,12,10,15,
21,12,0,0,18,0,0,48,12,16,0,8,100,111,95,108,
111,99,97,108,0,0,0,0,13,15,16,0,12,17,0,5,
105,116,101,109,115,0,0,0,9,16,10,17,11,17,0,0,
0,0,0,0,0,0,0,0,9,16,16,17,31,12,16,1,
19,12,15,12,15,13,12,0,12,16,0,10,95,100,111,95,
115,116,114,105,110,103,0,0,13,15,16,0,12,16,0,1,
42,0,0,0,31,12,16,1,19,12,15,12,15,14,12,0,
12,16,0,4,99,111,100,101,0,0,0,0,13,15,16,0,
12,20,0,3,71,69,84,0,13,16,20,0,15,17,13,0,
15,18,6,0,15,19,14,0,31,12,16,4,19,12,15,12,
12,16,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,15,16,0,15,16,14,0,31,12,16,1,19,12,15,12,
18,0,0,1,28,15,0,0,35,12,11,15,21,12,0,0,
18,0,0,64,12,17,0,8,100,111,95,108,111,99,97,108,
0,0,0,0,13,16,17,0,12,18,0,5,105,116,101,109,
115,0,0,0,9,17,11,18,11,18,0,0,0,0,0,0,
0,0,0,0,9,17,17,18,31,15,17,1,19,15,16,15,
15,12,15,0,12,17,0,4,99,111,100,101,0,0,0,0,
13,16,17,0,12,21,0,4,68,73,67,84,0,0,0,0,
13,17,21,0,15,18,12,0,11,19,0,0,0,0,0,0,
0,0,0,0,11,20,0,0,0,0,0,0,0,0,0,0,
31,15,17,4,19,15,16,15,12,17,0,8,95,100,111,95,
110,111,110,101,0,0,0,0,13,16,17,0,31,15,0,0,
19,15,16,15,15,14,15,0,12,17,0,4,99,111,100,101,
0,0,0,0,13,16,17,0,12,21,0,4,73,71,69,84,
0,0,0,0,13,17,21,0,15,18,12,0,15,19,6,0,
15,20,14,0,31,15,17,4,19,15,16,15,12,17,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,16,17,0,
15,17,14,0,31,15,17,1,19,15,16,15,18,0,0,1,
12,17,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,16,17,0,12,19,0,2,100,111,0,0,13,18,19,0,
11,20,0,0,0,0,0,0,0,0,0,64,9,19,3,20,
31,17,19,1,19,17,18,17,31,15,17,1,19,15,16,15,
12,17,0,1,68,0,0,0,13,16,17,0,12,17,0,3,
101,110,100,0,9,16,16,17,31,15,0,0,19,15,16,15,
12,17,0,3,116,97,103,0,13,16,17,0,15,17,4,0,
12,18,0,3,101,110,100,0,31,15,17,2,19,15,16,15,
28,16,0,0,23,15,2,16,21,15,0,0,18,0,0,64,
12,16,0,1,68,0,0,0,13,15,16,0,12,16,0,8,
95,103,108,111,98,97,108,115,0,0,0,0,9,15,15,16,
21,15,0,0,18,0,0,29,12,17,0,10,100,111,95,103,
108,111,98,97,108,115,0,0,13,16,17,0,12,19,0,5,
84,111,107,101,110,0,0,0,13,18,19,0,12,23,0,3,
112,111,115,0,9,19,1,23,11,20,0,0,0,0,0,0,
0,0,0,0,11,21,0,0,0,0,0,0,0,0,0,0,
11,24,0,0,0,0,0,0,0,0,0,0,9,23,3,24,
27,22,23,1,31,17,19,4,19,17,18,17,31,15,17,1,
19,15,16,15,18,0,0,1,12,17,0,10,100,111,95,115,
101,116,95,99,116,120,0,0,13,16,17,0,11,19,0,0,
0,0,0,0,0,0,0,0,9,17,3,19,12,20,0,5,
84,111,107,101,110,0,0,0,13,19,20,0,12,23,0,3,
112,111,115,0,9,20,1,23,12,21,0,3,114,101,103,0,
15,22,5,0,31,18,20,3,19,18,19,18,31,15,17,2,
19,15,16,15,15,6,15,0,18,0,0,34,12,18,0,9,
100,111,95,115,116,114,105,110,103,0,0,0,13,17,18,0,
11,19,0,0,0,0,0,0,0,0,0,0,9,18,3,19,
31,16,18,1,19,16,17,16,15,15,16,0,12,18,0,4,
99,111,100,101,0,0,0,0,13,17,18,0,12,22,0,3,
83,69,84,0,13,18,22,0,15,19,2,0,15,20,15,0,
15,21,5,0,31,16,18,4,19,16,17,16,12,18,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,17,18,0,
15,18,15,0,31,16,18,1,19,16,17,16,18,0,0,1,
12,18,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,17,18,0,15,18,5,0,31,16,18,1,19,16,17,16,
0,0,0,0,12,49,0,6,100,111,95,100,101,102,0,0,
14,49,48,0,16,49,3,36,44,44,0,0,28,2,0,0,
9,1,0,2,15,2,1,0,12,5,0,5,105,116,101,109,
115,0,0,0,9,4,1,5,15,3,4,0,28,5,0,0,
15,4,5,0,11,6,0,0,0,0,0,0,0,0,0,0,
9,5,3,6,12,6,0,4,116,121,112,101,0,0,0,0,
9,5,5,6,12,6,0,4,110,97,109,101,0,0,0,0,
23,5,5,6,21,5,0,0,18,0,0,10,11,7,0,0,
0,0,0,0,0,0,0,0,9,6,3,7,12,7,0,3,
118,97,108,0,9,6,6,7,15,5,6,0,18,0,0,34,
11,7,0,0,0,0,0,0,0,0,0,0,9,6,3,7,
12,7,0,5,105,116,101,109,115,0,0,0,9,6,6,7,
11,7,0,0,0,0,0,0,0,0,0,0,9,6,6,7,
12,7,0,3,118,97,108,0,9,6,6,7,15,5,6,0,
11,7,0,0,0,0,0,0,0,0,0,0,9,6,3,7,
12,7,0,5,105,116,101,109,115,0,0,0,9,6,6,7,
11,7,0,0,0,0,0,0,0,0,240,63,9,6,6,7,
12,7,0,3,118,97,108,0,9,6,6,7,15,4,6,0,
18,0,0,1,12,9,0,2,100,111,0,0,13,8,9,0,
12,11,0,5,84,111,107,101,110,0,0,0,13,10,11,0,
12,15,0,3,112,111,115,0,9,11,1,15,12,12,0,4,
100,105,99,116,0,0,0,0,11,13,0,0,0,0,0,0,
0,0,0,0,27,14,0,0,31,9,11,4,19,9,10,9,
31,7,9,1,19,7,8,7,15,6,7,0,12,10,0,10,
95,100,111,95,115,116,114,105,110,103,0,0,13,9,10,0,
15,10,5,0,31,8,10,1,19,8,9,8,15,7,8,0,
12,10,0,4,99,111,100,101,0,0,0,0,13,9,10,0,
12,13,0,4,71,83,69,84,0,0,0,0,13,10,13,0,
15,11,7,0,15,12,6,0,31,8,10,3,19,8,9,8,
12,10,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,9,10,0,15,10,7,0,31,8,10,1,19,8,9,8,
11,9,0,0,0,0,0,0,0,0,0,0,15,8,9,0,
27,10,0,0,15,9,10,0,15,10,8,0,15,8,9,0,
21,4,0,0,18,0,0,78,12,12,0,6,97,112,112,101,
110,100,0,0,9,11,8,12,12,14,0,5,84,111,107,101,
110,0,0,0,13,13,14,0,12,18,0,3,112,111,115,0,
9,14,1,18,12,15,0,4,99,97,108,108,0,0,0,0,
28,16,0,0,12,21,0,5,84,111,107,101,110,0,0,0,
13,20,21,0,12,25,0,3,112,111,115,0,9,21,1,25,
12,22,0,3,103,101,116,0,28,23,0,0,12,28,0,5,
84,111,107,101,110,0,0,0,13,27,28,0,12,31,0,3,
112,111,115,0,9,28,1,31,12,29,0,4,110,97,109,101,
0,0,0,0,15,30,4,0,31,25,28,3,19,25,27,25,
12,28,0,5,84,111,107,101,110,0,0,0,13,27,28,0,
12,31,0,3,112,111,115,0,9,28,1,31,12,29,0,6,
115,116,114,105,110,103,0,0,12,30,0,7,95,95,110,101,
119,95,95,0,31,26,28,3,19,26,27,26,27,24,25,2,
31,18,21,4,19,18,20,18,12,21,0,5,84,111,107,101,
110,0,0,0,13,20,21,0,12,24,0,3,112,111,115,0,
9,21,1,24,12,22,0,4,110,97,109,101,0,0,0,0,
12,23,0,4,115,101,108,102,0,0,0,0,31,19,21,3,
19,19,20,19,27,17,18,2,31,12,14,4,19,12,13,12,
31,9,12,1,19,9,11,9,18,0,0,1,11,12,0,0,
0,0,0,0,0,0,240,63,9,11,3,12,12,12,0,5,
105,116,101,109,115,0,0,0,9,11,11,12,11,12,0,0,
0,0,0,0,0,0,0,0,42,9,11,12,18,0,0,190,
12,14,0,4,116,121,112,101,0,0,0,0,9,13,9,14,
12,14,0,3,100,101,102,0,35,13,13,14,21,13,0,0,
18,0,0,3,18,0,255,245,18,0,0,1,12,15,0,5,
105,116,101,109,115,0,0,0,9,14,9,15,11,15,0,0,
0,0,0,0,0,0,0,0,9,14,14,15,12,15,0,3,
118,97,108,0,9,14,14,15,15,13,14,0,12,15,0,8,
95,95,105,110,105,116,95,95,0,0,0,0,23,14,13,15,
21,14,0,0,18,0,0,6,11,14,0,0,0,0,0,0,
0,0,240,63,15,10,14,0,18,0,0,1,12,16,0,6,
100,111,95,100,101,102,0,0,13,15,16,0,15,16,9,0,
15,17,6,0,31,14,16,2,19,14,15,14,12,16,0,6,
97,112,112,101,110,100,0,0,9,15,8,16,12,18,0,5,
84,111,107,101,110,0,0,0,13,17,18,0,12,22,0,3,
112,111,115,0,9,18,9,22,12,19,0,6,115,121,109,98,
111,108,0,0,12,20,0,1,61,0,0,0,12,25,0,5,
84,111,107,101,110,0,0,0,13,24,25,0,12,29,0,3,
112,111,115,0,9,25,9,29,12,26,0,3,103,101,116,0,
28,27,0,0,12,32,0,5,84,111,107,101,110,0,0,0,
13,31,32,0,12,35,0,3,112,111,115,0,9,32,9,35,
12,33,0,4,110,97,109,101,0,0,0,0,12,34,0,4,
115,101,108,102,0,0,0,0,31,29,32,3,19,29,31,29,
12,32,0,5,84,111,107,101,110,0,0,0,13,31,32,0,
12,35,0,3,112,111,115,0,9,32,9,35,12,33,0,6,
115,116,114,105,110,103,0,0,15,34,13,0,31,30,32,3,
19,30,31,30,27,28,29,2,31,22,25,4,19,22,24,22,
12,25,0,5,84,111,107,101,110,0,0,0,13,24,25,0,
12,29,0,3,112,111,115,0,9,25,9,29,12,26,0,4,
99,97,108,108,0,0,0,0,28,27,0,0,12,33,0,5,
84,111,107,101,110,0,0,0,13,32,33,0,12,36,0,3,
112,111,115,0,9,33,9,36,12,34,0,4,110,97,109,101,
0,0,0,0,12,35,0,4,98,105,110,100,0,0,0,0,
31,29,33,3,19,29,32,29,12,33,0,5,84,111,107,101,
110,0,0,0,13,32,33,0,12,37,0,3,112,111,115,0,
9,33,9,37,12,34,0,3,103,101,116,0,28,35,0,0,
12,40,0,5,84,111,107,101,110,0,0,0,13,39,40,0,
12,43,0,3,112,111,115,0,9,40,9,43,12,41,0,4,
110,97,109,101,0,0,0,0,15,42,5,0,31,37,40,3,
19,37,39,37,12,40,0,5,84,111,107,101,110,0,0,0,
13,39,40,0,12,43,0,3,112,111,115,0,9,40,9,43,
12,41,0,6,115,116,114,105,110,103,0,0,15,42,13,0,
31,38,40,3,19,38,39,38,27,36,37,2,31,30,33,4,
19,30,32,30,12,33,0,5,84,111,107,101,110,0,0,0,
13,32,33,0,12,36,0,3,112,111,115,0,9,33,9,36,
12,34,0,4,110,97,109,101,0,0,0,0,12,35,0,4,
115,101,108,102,0,0,0,0,31,31,33,3,19,31,32,31,
27,28,29,3,31,23,25,4,19,23,24,23,27,21,22,2,
31,16,18,4,19,16,17,16,31,14,16,1,19,14,15,14,
18,0,255,66,12,14,0,6,100,111,95,100,101,102,0,0,
13,12,14,0,12,17,0,5,84,111,107,101,110,0,0,0,
13,16,17,0,12,21,0,3,112,111,115,0,9,17,1,21,
12,18,0,3,100,101,102,0,28,19,0,0,12,25,0,5,
84,111,107,101,110,0,0,0,13,24,25,0,12,28,0,3,
112,111,115,0,9,25,1,28,12,26,0,4,110,97,109,101,
0,0,0,0,12,27,0,7,95,95,110,101,119,95,95,0,
31,21,25,3,19,21,24,21,12,25,0,5,84,111,107,101,
110,0,0,0,13,24,25,0,12,29,0,3,112,111,115,0,
9,25,1,29,12,26,0,4,108,105,115,116,0,0,0,0,
28,27,0,0,12,31,0,5,84,111,107,101,110,0,0,0,
13,30,31,0,12,34,0,3,112,111,115,0,9,31,1,34,
12,32,0,4,110,97,109,101,0,0,0,0,12,33,0,4,
115,101,108,102,0,0,0,0,31,29,31,3,19,29,30,29,
27,28,29,1,31,22,25,4,19,22,24,22,12,25,0,5,
84,111,107,101,110,0,0,0,13,24,25,0,12,29,0,3,
112,111,115,0,9,25,1,29,12,26,0,10,115,116,97,116,
101,109,101,110,116,115,0,0,28,27,0,0,15,28,8,0,
31,23,25,4,19,23,24,23,27,20,21,3,31,14,17,4,
19,14,16,14,15,15,6,0,31,11,14,2,19,11,12,11,
12,14,0,7,103,101,116,95,116,97,103,0,13,12,14,0,
31,11,0,0,19,11,12,11,15,1,11,0,12,15,0,3,
102,110,99,0,13,14,15,0,15,15,1,0,12,16,0,3,
101,110,100,0,31,12,15,2,19,12,14,12,15,11,12,0,
12,15,0,1,68,0,0,0,13,14,15,0,12,15,0,5,
98,101,103,105,110,0,0,0,9,14,14,15,31,12,0,0,
19,12,14,12,12,16,0,8,100,111,95,108,111,99,97,108,
0,0,0,0,13,15,16,0,12,18,0,5,84,111,107,101,
110,0,0,0,13,17,18,0,12,21,0,3,112,111,115,0,
9,18,2,21,12,19,0,4,110,97,109,101,0,0,0,0,
12,20,0,8,95,95,112,97,114,97,109,115,0,0,0,0,
31,16,18,3,19,16,17,16,31,14,16,1,19,14,15,14,
15,12,14,0,12,17,0,8,100,111,95,108,111,99,97,108,
0,0,0,0,13,16,17,0,12,19,0,5,84,111,107,101,
110,0,0,0,13,18,19,0,12,22,0,3,112,111,115,0,
9,19,2,22,12,20,0,4,110,97,109,101,0,0,0,0,
12,21,0,4,115,101,108,102,0,0,0,0,31,17,19,3,
19,17,18,17,31,15,17,1,19,15,16,15,15,14,15,0,
12,17,0,4,99,111,100,101,0,0,0,0,13,16,17,0,
12,21,0,4,68,73,67,84,0,0,0,0,13,17,21,0,
15,18,14,0,11,19,0,0,0,0,0,0,0,0,0,0,
11,20,0,0,0,0,0,0,0,0,0,0,31,15,17,4,
19,15,16,15,12,17,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,16,17,0,12,19,0,2,100,111,0,0,
13,18,19,0,12,21,0,5,84,111,107,101,110,0,0,0,
13,20,21,0,12,25,0,3,112,111,115,0,9,21,2,25,
12,22,0,4,99,97,108,108,0,0,0,0,28,23,0,0,
12,28,0,5,84,111,107,101,110,0,0,0,13,27,28,0,
12,32,0,3,112,111,115,0,9,28,2,32,12,29,0,3,
103,101,116,0,28,30,0,0,12,35,0,5,84,111,107,101,
110,0,0,0,13,34,35,0,12,38,0,3,112,111,115,0,
9,35,2,38,12,36,0,4,110,97,109,101,0,0,0,0,
15,37,5,0,31,32,35,3,19,32,34,32,12,35,0,5,
84,111,107,101,110,0,0,0,13,34,35,0,12,38,0,3,
112,111,115,0,9,35,2,38,12,36,0,6,115,116,114,105,
110,103,0,0,12,37,0,7,95,95,110,101,119,95,95,0,
31,33,35,3,19,33,34,33,27,31,32,2,31,25,28,4,
19,25,27,25,12,28,0,5,84,111,107,101,110,0,0,0,
13,27,28,0,12,31,0,3,112,111,115,0,9,28,2,31,
12,29,0,4,110,97,109,101,0,0,0,0,12,30,0,4,
115,101,108,102,0,0,0,0,31,26,28,3,19,26,27,26,
27,24,25,2,31,19,21,4,19,19,20,19,31,17,19,1,
19,17,18,17,31,15,17,1,19,15,16,15,21,10,0,0,
18,0,0,77,12,18,0,7,103,101,116,95,116,109,112,0,
13,17,18,0,31,16,0,0,19,16,17,16,15,15,16,0,
12,19,0,10,95,100,111,95,115,116,114,105,110,103,0,0,
13,18,19,0,12,19,0,8,95,95,105,110,105,116,95,95,
0,0,0,0,31,17,19,1,19,17,18,17,15,16,17,0,
12,19,0,4,99,111,100,101,0,0,0,0,13,18,19,0,
12,23,0,3,71,69,84,0,13,19,23,0,15,20,15,0,
15,21,14,0,15,22,16,0,31,17,19,4,19,17,18,17,
12,20,0,7,103,101,116,95,116,109,112,0,13,19,20,0,
31,18,0,0,19,18,19,18,15,17,18,0,12,20,0,4,
99,111,100,101,0,0,0,0,13,19,20,0,12,24,0,4,
67,65,76,76,0,0,0,0,13,20,24,0,15,21,17,0,
15,22,15,0,15,23,12,0,31,18,20,4,19,18,19,18,
12,20,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,19,20,0,15,20,15,0,31,18,20,1,19,18,19,18,
12,20,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,19,20,0,15,20,16,0,31,18,20,1,19,18,19,18,
12,20,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,19,20,0,15,20,17,0,31,18,20,1,19,18,19,18,
18,0,0,1,12,20,0,4,99,111,100,101,0,0,0,0,
13,19,20,0,12,22,0,6,82,69,84,85,82,78,0,0,
13,20,22,0,15,21,14,0,31,18,20,2,19,18,19,18,
12,20,0,1,68,0,0,0,13,19,20,0,12,20,0,3,
101,110,100,0,9,19,19,20,31,18,0,0,19,18,19,18,
12,20,0,3,116,97,103,0,13,19,20,0,15,20,1,0,
12,21,0,3,101,110,100,0,31,18,20,2,19,18,19,18,
12,20,0,10,95,100,111,95,115,116,114,105,110,103,0,0,
13,19,20,0,12,20,0,8,95,95,99,97,108,108,95,95,
0,0,0,0,31,18,20,1,19,18,19,18,15,7,18,0,
12,20,0,4,99,111,100,101,0,0,0,0,13,19,20,0,
12,24,0,3,83,69,84,0,13,20,24,0,15,21,6,0,
15,22,7,0,15,23,11,0,31,18,20,4,19,18,19,18,
12,20,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,19,20,0,15,20,6,0,31,18,20,1,19,18,19,18,
12,20,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,19,20,0,15,20,7,0,31,18,20,1,19,18,19,18,
0,0,0,0,12,50,0,8,100,111,95,99,108,97,115,115,
0,0,0,0,14,50,49,0,16,50,0,123,44,10,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,105,116,101,109,
115,0,0,0,9,3,1,4,15,2,3,0,12,5,0,9,
115,116,97,99,107,95,116,97,103,0,0,0,13,4,5,0,
31,3,0,0,19,3,4,3,15,1,3,0,12,5,0,3,
116,97,103,0,13,4,5,0,15,5,1,0,12,6,0,5,
98,101,103,105,110,0,0,0,31,3,5,2,19,3,4,3,
12,5,0,3,116,97,103,0,13,4,5,0,15,5,1,0,
12,6,0,8,99,111,110,116,105,110,117,101,0,0,0,0,
31,3,5,2,19,3,4,3,12,6,0,2,100,111,0,0,
13,5,6,0,11,7,0,0,0,0,0,0,0,0,0,0,
9,6,2,7,31,4,6,1,19,4,5,4,15,3,4,0,
12,6,0,4,99,111,100,101,0,0,0,0,13,5,6,0,
12,8,0,2,73,70,0,0,13,6,8,0,15,7,3,0,
31,4,6,2,19,4,5,4,12,6,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,5,6,0,15,6,3,0,
31,4,6,1,19,4,5,4,12,6,0,4,106,117,109,112,
0,0,0,0,13,5,6,0,15,6,1,0,12,7,0,3,
101,110,100,0,31,4,6,2,19,4,5,4,12,6,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,5,6,0,
12,8,0,2,100,111,0,0,13,7,8,0,11,9,0,0,
0,0,0,0,0,0,240,63,9,8,2,9,31,6,8,1,
19,6,7,6,31,4,6,1,19,4,5,4,12,6,0,4,
106,117,109,112,0,0,0,0,13,5,6,0,15,6,1,0,
12,7,0,5,98,101,103,105,110,0,0,0,31,4,6,2,
19,4,5,4,12,6,0,3,116,97,103,0,13,5,6,0,
15,6,1,0,12,7,0,5,98,114,101,97,107,0,0,0,
31,4,6,2,19,4,5,4,12,6,0,3,116,97,103,0,
13,5,6,0,15,6,1,0,12,7,0,3,101,110,100,0,
31,4,6,2,19,4,5,4,12,6,0,7,112,111,112,95,
116,97,103,0,13,5,6,0,31,4,0,0,19,4,5,4,
0,0,0,0,12,51,0,8,100,111,95,119,104,105,108,101,
0,0,0,0,14,51,50,0,16,51,0,156,44,14,0,0,
28,2,0,0,9,1,0,2,12,4,0,5,105,116,101,109,
115,0,0,0,9,3,1,4,15,2,3,0,12,6,0,8,
100,111,95,108,111,99,97,108,0,0,0,0,13,5,6,0,
11,7,0,0,0,0,0,0,0,0,0,0,9,6,2,7,
31,4,6,1,19,4,5,4,15,3,4,0,12,7,0,2,
100,111,0,0,13,6,7,0,11,8,0,0,0,0,0,0,
0,0,240,63,9,7,2,8,31,5,7,1,19,5,6,5,
15,4,5,0,12,8,0,10,95,100,111,95,110,117,109,98,
101,114,0,0,13,7,8,0,12,8,0,1,48,0,0,0,
31,6,8,1,19,6,7,6,15,5,6,0,12,9,0,9,
115,116,97,99,107,95,116,97,103,0,0,0,13,8,9,0,
31,7,0,0,19,7,8,7,15,6,7,0,12,9,0,3,
116,97,103,0,13,8,9,0,15,9,6,0,12,10,0,4,
108,111,111,112,0,0,0,0,31,7,9,2,19,7,8,7,
12,9,0,3,116,97,103,0,13,8,9,0,15,9,6,0,
12,10,0,8,99,111,110,116,105,110,117,101,0,0,0,0,
31,7,9,2,19,7,8,7,12,9,0,4,99,111,100,101,
0,0,0,0,13,8,9,0,12,13,0,4,73,84,69,82,
0,0,0,0,13,9,13,0,15,10,3,0,15,11,4,0,
15,12,5,0,31,7,9,4,19,7,8,7,12,9,0,4,
106,117,109,112,0,0,0,0,13,8,9,0,15,9,6,0,
12,10,0,3,101,110,100,0,31,7,9,2,19,7,8,7,
12,9,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,8,9,0,12,11,0,2,100,111,0,0,13,10,11,0,
11,12,0,0,0,0,0,0,0,0,0,64,9,11,2,12,
31,9,11,1,19,9,10,9,31,7,9,1,19,7,8,7,
12,9,0,4,106,117,109,112,0,0,0,0,13,8,9,0,
15,9,6,0,12,10,0,4,108,111,111,112,0,0,0,0,
31,7,9,2,19,7,8,7,12,9,0,3,116,97,103,0,
13,8,9,0,15,9,6,0,12,10,0,5,98,114,101,97,
107,0,0,0,31,7,9,2,19,7,8,7,12,9,0,3,
116,97,103,0,13,8,9,0,15,9,6,0,12,10,0,3,
101,110,100,0,31,7,9,2,19,7,8,7,12,9,0,7,
112,111,112,95,116,97,103,0,13,8,9,0,31,7,0,0,
19,7,8,7,12,9,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,8,9,0,15,9,4,0,31,7,9,1,
19,7,8,7,12,9,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,8,9,0,15,9,5,0,31,7,9,1,
19,7,8,7,0,0,0,0,12,52,0,6,100,111,95,102,
111,114,0,0,14,52,51,0,16,52,0,159,44,18,0,0,
28,2,0,0,9,1,0,2,28,2,0,0,28,3,0,0,
32,2,0,3,12,4,0,5,99,111,109,112,58,0,0,0,
12,7,0,7,103,101,116,95,116,97,103,0,13,6,7,0,
31,5,0,0,19,5,6,5,1,4,4,5,15,3,4,0,
12,6,0,8,100,111,95,108,111,99,97,108,0,0,0,0,
13,5,6,0,12,8,0,5,84,111,107,101,110,0,0,0,
13,7,8,0,12,11,0,3,112,111,115,0,9,8,1,11,
12,9,0,4,110,97,109,101,0,0,0,0,15,10,3,0,
31,6,8,3,19,6,7,6,31,4,6,1,19,4,5,4,
15,2,4,0,12,6,0,4,99,111,100,101,0,0,0,0,
13,5,6,0,12,10,0,4,76,73,83,84,0,0,0,0,
13,6,10,0,15,7,2,0,11,8,0,0,0,0,0,0,
0,0,0,0,11,9,0,0,0,0,0,0,0,0,0,0,
31,4,6,4,19,4,5,4,12,7,0,5,84,111,107,101,
110,0,0,0,13,6,7,0,12,11,0,3,112,111,115,0,
9,7,1,11,12,8,0,3,103,101,116,0,28,9,0,0,
12,14,0,5,84,111,107,101,110,0,0,0,13,13,14,0,
12,17,0,3,112,111,115,0,9,14,1,17,12,15,0,3,
114,101,103,0,15,16,2,0,31,11,14,3,19,11,13,11,
12,14,0,5,84,111,107,101,110,0,0,0,13,13,14,0,
12,17,0,3,112,111,115,0,9,14,1,17,12,15,0,6,
115,121,109,98,111,108,0,0,12,16,0,4,78,111,110,101,
0,0,0,0,31,12,14,3,19,12,13,12,27,10,11,2,
31,5,7,4,19,5,6,5,15,4,5,0,12,8,0,5,
84,111,107,101,110,0,0,0,13,7,8,0,12,12,0,3,
112,111,115,0,9,8,1,12,12,9,0,6,115,121,109,98,
111,108,0,0,12,10,0,1,61,0,0,0,15,12,4,0,
12,14,0,5,105,116,101,109,115,0,0,0,9,13,1,14,
11,14,0,0,0,0,0,0,0,0,0,0,9,13,13,14,
27,11,12,2,31,6,8,4,19,6,7,6,15,5,6,0,
12,8,0,2,100,111,0,0,13,7,8,0,12,10,0,5,
84,111,107,101,110,0,0,0,13,9,10,0,12,14,0,3,
112,111,115,0,9,10,1,14,12,11,0,3,102,111,114,0,
28,12,0,0,12,17,0,5,105,116,101,109,115,0,0,0,
9,14,1,17,11,17,0,0,0,0,0,0,0,0,240,63,
9,14,14,17,12,17,0,5,105,116,101,109,115,0,0,0,
9,15,1,17,11,17,0,0,0,0,0,0,0,0,0,64,
9,15,15,17,15,16,5,0,27,13,14,3,31,8,10,4,
19,8,9,8,31,6,8,1,19,6,7,6,20,2,0,0,
0,0,0,0,12,53,0,7,100,111,95,99,111,109,112,0,
14,53,52,0,16,53,0,171,44,13,0,0,28,2,0,0,
9,1,0,2,12,4,0,5,105,116,101,109,115,0,0,0,
9,3,1,4,15,2,3,0,12,5,0,7,103,101,116,95,
116,97,103,0,13,4,5,0,31,3,0,0,19,3,4,3,
15,1,3,0,11,4,0,0,0,0,0,0,0,0,0,0,
15,3,4,0,11,5,0,0,0,0,0,0,0,0,0,0,
42,4,2,5,18,0,0,131,12,8,0,3,116,97,103,0,
13,7,8,0,15,8,1,0,15,9,3,0,31,6,8,2,
19,6,7,6,12,7,0,4,116,121,112,101,0,0,0,0,
9,6,4,7,12,7,0,4,101,108,105,102,0,0,0,0,
23,6,6,7,21,6,0,0,18,0,0,65,12,9,0,2,
100,111,0,0,13,8,9,0,12,10,0,5,105,116,101,109,
115,0,0,0,9,9,4,10,11,10,0,0,0,0,0,0,
0,0,0,0,9,9,9,10,31,7,9,1,19,7,8,7,
15,6,7,0,12,9,0,4,99,111,100,101,0,0,0,0,
13,8,9,0,12,11,0,2,73,70,0,0,13,9,11,0,
15,10,6,0,31,7,9,2,19,7,8,7,12,9,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,8,9,0,
15,9,6,0,31,7,9,1,19,7,8,7,12,9,0,4,
106,117,109,112,0,0,0,0,13,8,9,0,15,9,1,0,
11,11,0,0,0,0,0,0,0,0,240,63,1,10,3,11,
31,7,9,2,19,7,8,7,12,9,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,8,9,0,12,11,0,2,
100,111,0,0,13,10,11,0,12,12,0,5,105,116,101,109,
115,0,0,0,9,11,4,12,11,12,0,0,0,0,0,0,
0,0,240,63,9,11,11,12,31,9,11,1,19,9,10,9,
31,7,9,1,19,7,8,7,18,0,0,35,12,8,0,4,
116,121,112,101,0,0,0,0,9,7,4,8,12,8,0,4,
101,108,115,101,0,0,0,0,23,7,7,8,21,7,0,0,
18,0,0,22,12,9,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,8,9,0,12,11,0,2,100,111,0,0,
13,10,11,0,12,12,0,5,105,116,101,109,115,0,0,0,
9,11,4,12,11,12,0,0,0,0,0,0,0,0,0,0,
9,11,11,12,31,9,11,1,19,9,10,9,31,7,9,1,
19,7,8,7,18,0,0,4,28,7,0,0,37,7,0,0,
18,0,0,1,12,9,0,4,106,117,109,112,0,0,0,0,
13,8,9,0,15,9,1,0,12,10,0,3,101,110,100,0,
31,7,9,2,19,7,8,7,11,8,0,0,0,0,0,0,
0,0,240,63,1,7,3,8,15,3,7,0,18,0,255,125,
12,8,0,3,116,97,103,0,13,7,8,0,15,8,1,0,
15,9,3,0,31,5,8,2,19,5,7,5,12,8,0,3,
116,97,103,0,13,7,8,0,15,8,1,0,12,9,0,3,
101,110,100,0,31,5,8,2,19,5,7,5,0,0,0,0,
12,54,0,5,100,111,95,105,102,0,0,0,14,54,53,0,
16,54,0,93,44,9,0,0,28,2,0,0,9,1,0,2,
12,4,0,5,105,116,101,109,115,0,0,0,9,3,1,4,
15,2,3,0,12,5,0,7,103,101,116,95,116,97,103,0,
13,4,5,0,31,3,0,0,19,3,4,3,15,1,3,0,
12,5,0,6,115,101,116,106,109,112,0,0,13,4,5,0,
15,5,1,0,12,6,0,6,101,120,99,101,112,116,0,0,
31,3,5,2,19,3,4,3,12,5,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,4,5,0,12,7,0,2,
100,111,0,0,13,6,7,0,11,8,0,0,0,0,0,0,
0,0,0,0,9,7,2,8,31,5,7,1,19,5,6,5,
31,3,5,1,19,3,4,3,12,5,0,4,106,117,109,112,
0,0,0,0,13,4,5,0,15,5,1,0,12,6,0,3,
101,110,100,0,31,3,5,2,19,3,4,3,12,5,0,3,
116,97,103,0,13,4,5,0,15,5,1,0,12,6,0,6,
101,120,99,101,112,116,0,0,31,3,5,2,19,3,4,3,
12,5,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,4,5,0,12,7,0,2,100,111,0,0,13,6,7,0,
11,8,0,0,0,0,0,0,0,0,240,63,9,7,2,8,
12,8,0,5,105,116,101,109,115,0,0,0,9,7,7,8,
11,8,0,0,0,0,0,0,0,0,240,63,9,7,7,8,
31,5,7,1,19,5,6,5,31,3,5,1,19,3,4,3,
12,5,0,3,116,97,103,0,13,4,5,0,15,5,1,0,
12,6,0,3,101,110,100,0,31,3,5,2,19,3,4,3,
0,0,0,0,12,55,0,6,100,111,95,116,114,121,0,0,
14,55,54,0,16,55,0,56,44,8,0,0,28,2,0,0,
9,1,0,2,12,3,0,5,105,116,101,109,115,0,0,0,
9,2,1,3,21,2,0,0,18,0,0,16,12,5,0,2,
100,111,0,0,13,4,5,0,12,6,0,5,105,116,101,109,
115,0,0,0,9,5,1,6,11,6,0,0,0,0,0,0,
0,0,0,0,9,5,5,6,31,3,5,1,19,3,4,3,
15,2,3,0,18,0,0,10,12,5,0,8,95,100,111,95,
110,111,110,101,0,0,0,0,13,4,5,0,31,3,0,0,
19,3,4,3,15,2,3,0,18,0,0,1,12,5,0,4,
99,111,100,101,0,0,0,0,13,4,5,0,12,7,0,6,
82,69,84,85,82,78,0,0,13,5,7,0,15,6,2,0,
31,3,5,2,19,3,4,3,12,5,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,4,5,0,15,5,2,0,
31,3,5,1,19,3,4,3,28,3,0,0,20,3,0,0,
0,0,0,0,12,56,0,9,100,111,95,114,101,116,117,114,
110,0,0,0,14,56,55,0,16,56,0,56,44,8,0,0,
28,2,0,0,9,1,0,2,12,3,0,5,105,116,101,109,
115,0,0,0,9,2,1,3,21,2,0,0,18,0,0,16,
12,5,0,2,100,111,0,0,13,4,5,0,12,6,0,5,
105,116,101,109,115,0,0,0,9,5,1,6,11,6,0,0,
0,0,0,0,0,0,0,0,9,5,5,6,31,3,5,1,
19,3,4,3,15,2,3,0,18,0,0,10,12,5,0,8,
95,100,111,95,110,111,110,101,0,0,0,0,13,4,5,0,
31,3,0,0,19,3,4,3,15,2,3,0,18,0,0,1,
12,5,0,4,99,111,100,101,0,0,0,0,13,4,5,0,
12,7,0,5,82,65,73,83,69,0,0,0,13,5,7,0,
15,6,2,0,31,3,5,2,19,3,4,3,12,5,0,8,
102,114,101,101,95,116,109,112,0,0,0,0,13,4,5,0,
15,5,2,0,31,3,5,1,19,3,4,3,28,3,0,0,
20,3,0,0,0,0,0,0,12,57,0,8,100,111,95,114,
97,105,115,101,0,0,0,0,14,57,56,0,16,57,0,28,
44,10,0,0,28,2,0,0,9,1,0,2,12,4,0,5,
105,116,101,109,115,0,0,0,9,3,1,4,11,4,0,0,
0,0,0,0,0,0,0,0,42,2,3,4,18,0,0,15,
12,7,0,8,102,114,101,101,95,116,109,112,0,0,0,0,
13,6,7,0,12,9,0,2,100,111,0,0,13,8,9,0,
15,9,2,0,31,7,9,1,19,7,8,7,31,5,7,1,
19,5,6,5,18,0,255,241,0,0,0,0,12,58,0,13,
100,111,95,115,116,97,116,101,109,101,110,116,115,0,0,0,
14,58,57,0,16,58,0,33,44,9,0,0,28,2,0,0,
9,1,0,2,28,2,0,0,28,3,0,0,32,2,0,3,
12,5,0,7,103,101,116,95,116,109,112,0,13,4,5,0,
15,5,2,0,31,3,5,1,19,3,4,3,15,2,3,0,
12,5,0,10,109,97,110,97,103,101,95,115,101,113,0,0,
13,4,5,0,12,8,0,4,76,73,83,84,0,0,0,0,
13,5,8,0,15,6,2,0,12,8,0,5,105,116,101,109,
115,0,0,0,9,7,1,8,31,3,5,3,19,3,4,3,
20,2,0,0,0,0,0,0,12,59,0,7,100,111,95,108,
105,115,116,0,14,59,58,0,16,59,0,33,44,9,0,0,
28,2,0,0,9,1,0,2,28,2,0,0,28,3,0,0,
32,2,0,3,12,5,0,7,103,101,116,95,116,109,112,0,
13,4,5,0,15,5,2,0,31,3,5,1,19,3,4,3,
15,2,3,0,12,5,0,10,109,97,110,97,103,101,95,115,
101,113,0,0,13,4,5,0,12,8,0,4,68,73,67,84,
0,0,0,0,13,5,8,0,15,6,2,0,12,8,0,5,
105,116,101,109,115,0,0,0,9,7,1,8,31,3,5,3,
19,3,4,3,20,2,0,0,0,0,0,0,12,60,0,7,
100,111,95,100,105,99,116,0,14,60,59,0,16,60,0,32,
44,11,0,0,28,2,0,0,9,1,0,2,28,2,0,0,
28,3,0,0,32,2,0,3,12,5,0,5,105,116,101,109,
115,0,0,0,9,4,1,5,15,3,4,0,12,6,0,5,
105,110,102,105,120,0,0,0,13,5,6,0,12,10,0,3,
71,69,84,0,13,6,10,0,11,10,0,0,0,0,0,0,
0,0,0,0,9,7,3,10,11,10,0,0,0,0,0,0,
0,0,240,63,9,8,3,10,15,9,2,0,31,4,6,4,
19,4,5,4,20,4,0,0,0,0,0,0,12,61,0,6,
100,111,95,103,101,116,0,0,14,61,60,0,16,61,0,25,
44,7,0,0,28,2,0,0,9,1,0,2,12,4,0,4,
106,117,109,112,0,0,0,0,13,3,4,0,12,6,0,1,
68,0,0,0,13,4,6,0,12,6,0,6,116,115,116,97,
99,107,0,0,9,4,4,6,11,6,0,0,0,0,0,0,
0,0,240,191,9,4,4,6,12,5,0,5,98,114,101,97,
107,0,0,0,31,2,4,2,19,2,3,2,0,0,0,0,
12,62,0,8,100,111,95,98,114,101,97,107,0,0,0,0,
14,62,61,0,16,62,0,26,44,7,0,0,28,2,0,0,
9,1,0,2,12,4,0,4,106,117,109,112,0,0,0,0,
13,3,4,0,12,6,0,1,68,0,0,0,13,4,6,0,
12,6,0,6,116,115,116,97,99,107,0,0,9,4,4,6,
11,6,0,0,0,0,0,0,0,0,240,191,9,4,4,6,
12,5,0,8,99,111,110,116,105,110,117,101,0,0,0,0,
31,2,4,2,19,2,3,2,0,0,0,0,12,63,0,11,
100,111,95,99,111,110,116,105,110,117,101,0,14,63,62,0,
16,63,0,15,44,6,0,0,28,2,0,0,9,1,0,2,
12,4,0,4,99,111,100,101,0,0,0,0,13,3,4,0,
12,5,0,4,80,65,83,83,0,0,0,0,13,4,5,0,
31,2,4,1,19,2,3,2,0,0,0,0,12,64,0,7,
100,111,95,112,97,115,115,0,14,64,63,0,16,64,0,76,
44,11,0,0,12,1,0,1,63,0,0,0,28,2,0,0,
32,1,0,2,12,3,0,4,65,82,71,86,0,0,0,0,
13,2,3,0,12,3,0,6,45,110,111,112,111,115,0,0,
36,2,2,3,21,2,0,0,18,0,0,4,28,2,0,0,
20,2,0,0,18,0,0,1,12,4,0,4,99,111,100,101,
0,0,0,0,13,3,4,0,12,6,0,4,70,73,76,69,
0,0,0,0,13,4,6,0,12,7,0,8,102,114,101,101,
95,116,109,112,0,0,0,0,13,6,7,0,12,9,0,10,
95,100,111,95,115,116,114,105,110,103,0,0,13,8,9,0,
12,10,0,1,68,0,0,0,13,9,10,0,12,10,0,5,
102,110,97,109,101,0,0,0,9,9,9,10,31,7,9,1,
19,7,8,7,31,5,7,1,19,5,6,5,31,2,4,2,
19,2,3,2,12,4,0,4,99,111,100,101,0,0,0,0,
13,3,4,0,12,6,0,4,78,65,77,69,0,0,0,0,
13,4,6,0,12,7,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,6,7,0,12,9,0,10,95,100,111,95,
115,116,114,105,110,103,0,0,13,8,9,0,15,9,1,0,
31,7,9,1,19,7,8,7,31,5,7,1,19,5,6,5,
31,2,4,2,19,2,3,2,0,0,0,0,12,65,0,7,
100,111,95,105,110,102,111,0,14,65,64,0,16,65,0,31,
44,8,0,0,28,2,0,0,9,1,0,2,12,4,0,7,
100,111,95,105,110,102,111,0,13,3,4,0,31,2,0,0,
19,2,3,2,12,4,0,8,102,114,101,101,95,116,109,112,
0,0,0,0,13,3,4,0,12,6,0,2,100,111,0,0,
13,5,6,0,12,7,0,5,105,116,101,109,115,0,0,0,
9,6,1,7,11,7,0,0,0,0,0,0,0,0,0,0,
9,6,6,7,31,4,6,1,19,4,5,4,31,2,4,1,
19,2,3,2,0,0,0,0,12,66,0,9,100,111,95,109,
111,100,117,108,101,0,0,0,14,66,65,0,16,66,0,12,
44,5,0,0,28,2,0,0,9,1,0,2,28,2,0,0,
28,3,0,0,32,2,0,3,12,4,0,3,118,97,108,0,
9,3,1,4,20,3,0,0,0,0,0,0,12,67,0,6,
100,111,95,114,101,103,0,0,14,67,66,0,12,67,0,4,
102,109,97,112,0,0,0,0,12,69,0,6,109,111,100,117,
108,101,0,0,12,103,0,9,100,111,95,109,111,100,117,108,
101,0,0,0,13,70,103,0,12,71,0,10,115,116,97,116,
101,109,101,110,116,115,0,0,12,103,0,13,100,111,95,115,
116,97,116,101,109,101,110,116,115,0,0,0,13,72,103,0,
12,73,0,3,100,101,102,0,12,103,0,6,100,111,95,100,
101,102,0,0,13,74,103,0,12,75,0,6,114,101,116,117,
114,110,0,0,12,103,0,9,100,111,95,114,101,116,117,114,
110,0,0,0,13,76,103,0,12,77,0,5,119,104,105,108,
101,0,0,0,12,103,0,8,100,111,95,119,104,105,108,101,
0,0,0,0,13,78,103,0,12,79,0,2,105,102,0,0,
12,103,0,5,100,111,95,105,102,0,0,0,13,80,103,0,
12,81,0,5,98,114,101,97,107,0,0,0,12,103,0,8,
100,111,95,98,114,101,97,107,0,0,0,0,13,82,103,0,
12,83,0,4,112,97,115,115,0,0,0,0,12,103,0,7,
100,111,95,112,97,115,115,0,13,84,103,0,12,85,0,8,
99,111,110,116,105,110,117,101,0,0,0,0,12,103,0,11,
100,111,95,99,111,110,116,105,110,117,101,0,13,86,103,0,
12,87,0,3,102,111,114,0,12,103,0,6,100,111,95,102,
111,114,0,0,13,88,103,0,12,89,0,5,99,108,97,115,
115,0,0,0,12,103,0,8,100,111,95,99,108,97,115,115,
0,0,0,0,13,90,103,0,12,91,0,5,114,97,105,115,
101,0,0,0,12,103,0,8,100,111,95,114,97,105,115,101,
0,0,0,0,13,92,103,0,12,93,0,3,116,114,121,0,
12,103,0,6,100,111,95,116,114,121,0,0,13,94,103,0,
12,95,0,6,105,109,112,111,114,116,0,0,12,103,0,9,
100,111,95,105,109,112,111,114,116,0,0,0,13,96,103,0,
12,97,0,7,103,108,111,98,97,108,115,0,12,103,0,10,
100,111,95,103,108,111,98,97,108,115,0,0,13,98,103,0,
12,99,0,3,100,101,108,0,12,103,0,6,100,111,95,100,
101,108,0,0,13,100,103,0,12,101,0,4,102,114,111,109,
0,0,0,0,12,103,0,7,100,111,95,102,114,111,109,0,
13,102,103,0,26,68,69,34,14,67,68,0,12,67,0,4,
114,109,97,112,0,0,0,0,12,69,0,4,108,105,115,116,
0,0,0,0,12,93,0,7,100,111,95,108,105,115,116,0,
13,70,93,0,12,71,0,5,116,117,112,108,101,0,0,0,
12,93,0,7,100,111,95,108,105,115,116,0,13,72,93,0,
12,73,0,4,100,105,99,116,0,0,0,0,12,93,0,7,
100,111,95,100,105,99,116,0,13,74,93,0,12,75,0,5,
115,108,105,99,101,0,0,0,12,93,0,7,100,111,95,108,
105,115,116,0,13,76,93,0,12,77,0,4,99,111,109,112,
0,0,0,0,12,93,0,7,100,111,95,99,111,109,112,0,
13,78,93,0,12,79,0,4,110,97,109,101,0,0,0,0,
12,93,0,7,100,111,95,110,97,109,101,0,13,80,93,0,
12,81,0,6,115,121,109,98,111,108,0,0,12,93,0,9,
100,111,95,115,121,109,98,111,108,0,0,0,13,82,93,0,
12,83,0,6,110,117,109,98,101,114,0,0,12,93,0,9,
100,111,95,110,117,109,98,101,114,0,0,0,13,84,93,0,
12,85,0,6,115,116,114,105,110,103,0,0,12,93,0,9,
100,111,95,115,116,114,105,110,103,0,0,0,13,86,93,0,
12,87,0,3,103,101,116,0,12,93,0,6,100,111,95,103,
101,116,0,0,13,88,93,0,12,89,0,4,99,97,108,108,
0,0,0,0,12,93,0,7,100,111,95,99,97,108,108,0,
13,90,93,0,12,91,0,3,114,101,103,0,12,93,0,6,
100,111,95,114,101,103,0,0,13,92,93,0,26,68,69,24,
14,67,68,0,16,67,0,110,44,9,0,0,28,2,0,0,
9,1,0,2,28,2,0,0,28,3,0,0,32,2,0,3,
12,4,0,3,112,111,115,0,9,3,1,4,21,3,0,0,
18,0,0,11,12,5,0,6,115,101,116,112,111,115,0,0,
13,4,5,0,12,6,0,3,112,111,115,0,9,5,1,6,
31,3,5,1,19,3,4,3,18,0,0,1,38,0,0,41,
12,4,0,4,114,109,97,112,0,0,0,0,13,3,4,0,
12,5,0,4,116,121,112,101,0,0,0,0,9,4,1,5,
36,3,3,4,21,3,0,0,18,0,0,16,12,5,0,4,
114,109,97,112,0,0,0,0,13,4,5,0,12,6,0,4,
116,121,112,101,0,0,0,0,9,5,1,6,9,4,4,5,
15,5,1,0,15,6,2,0,31,3,5,2,19,3,4,3,
20,3,0,0,18,0,0,1,12,5,0,4,102,109,97,112,
0,0,0,0,13,4,5,0,12,6,0,4,116,121,112,101,
0,0,0,0,9,5,1,6,9,4,4,5,15,5,1,0,
31,3,5,1,19,3,4,3,20,3,0,0,18,0,0,47,
12,4,0,1,68,0,0,0,13,3,4,0,12,4,0,5,
101,114,114,111,114,0,0,0,9,3,3,4,21,3,0,0,
18,0,0,4,28,3,0,0,37,3,0,0,18,0,0,1,
12,4,0,1,68,0,0,0,13,3,4,0,11,4,0,0,
0,0,0,0,0,0,240,63,12,5,0,5,101,114,114,111,
114,0,0,0,10,3,5,4,12,5,0,8,116,111,107,101,
110,105,122,101,0,0,0,0,13,4,5,0,12,5,0,7,
117,95,101,114,114,111,114,0,9,4,4,5,12,5,0,6,
101,110,99,111,100,101,0,0,12,8,0,1,68,0,0,0,
13,6,8,0,12,8,0,4,99,111,100,101,0,0,0,0,
9,6,6,8,12,8,0,3,112,111,115,0,9,7,1,8,
31,3,5,3,19,3,4,3,0,0,0,0,12,68,0,2,
100,111,0,0,14,68,67,0,16,68,0,109,44,12,0,0,
28,2,0,0,9,1,0,2,28,3,0,0,9,2,0,3,
28,4,0,0,9,3,0,4,12,6,0,5,84,111,107,101,
110,0,0,0,13,5,6,0,11,10,0,0,0,0,0,0,
0,0,240,63,11,11,0,0,0,0,0,0,0,0,240,63,
27,6,10,2,12,7,0,6,109,111,100,117,108,101,0,0,
12,8,0,6,109,111,100,117,108,101,0,0,15,10,3,0,
27,9,10,1,31,4,6,4,19,4,5,4,15,3,4,0,
12,6,0,8,116,111,107,101,110,105,122,101,0,0,0,0,
13,5,6,0,12,6,0,5,99,108,101,97,110,0,0,0,
9,5,5,6,15,6,2,0,31,4,6,1,19,4,5,4,
15,2,4,0,12,4,0,1,68,0,0,0,12,7,0,6,
68,83,116,97,116,101,0,0,13,6,7,0,15,7,2,0,
15,8,1,0,31,5,7,2,19,5,6,5,14,4,5,0,
12,6,0,1,68,0,0,0,13,5,6,0,12,6,0,5,
98,101,103,105,110,0,0,0,9,5,5,6,11,6,0,0,
0,0,0,0,0,0,240,63,31,4,6,1,19,4,5,4,
12,6,0,2,100,111,0,0,13,5,6,0,15,6,3,0,
31,4,6,1,19,4,5,4,12,6,0,1,68,0,0,0,
13,5,6,0,12,6,0,3,101,110,100,0,9,5,5,6,
31,4,0,0,19,4,5,4,12,6,0,8,109,97,112,95,
116,97,103,115,0,0,0,0,13,5,6,0,31,4,0,0,
19,4,5,4,12,6,0,1,68,0,0,0,13,5,6,0,
12,6,0,3,111,117,116,0,9,5,5,6,15,4,5,0,
12,5,0,1,68,0,0,0,28,6,0,0,14,5,6,0,
12,6,0,0,0,0,0,0,12,7,0,4,106,111,105,110,
0,0,0,0,9,6,6,7,15,7,4,0,31,5,7,1,
19,5,6,5,20,5,0,0,0,0,0,0,12,69,0,6,
101,110,99,111,100,101,0,0,14,69,68,0,0,0,0,0,
};
unsigned char tp_py2bc[] = {
44,11,0,0,11,0,0,0,0,0,0,0,0,0,0,0,
12,3,0,3,115,116,114,0,13,2,3,0,11,3,0,0,
0,0,0,0,0,0,240,63,31,1,3,1,19,1,2,1,
12,2,0,1,49,0,0,0,23,1,1,2,23,0,0,1,
21,0,0,0,18,0,0,23,12,2,0,6,105,109,112,111,
114,116,0,0,13,1,2,0,12,2,0,4,98,111,111,116,
0,0,0,0,31,0,2,1,19,0,1,0,12,3,0,5,
109,101,114,103,101,0,0,0,13,2,3,0,12,5,0,8,
95,95,100,105,99,116,95,95,0,0,0,0,13,3,5,0,
15,4,0,0,31,1,3,2,19,1,2,1,18,0,0,1,
12,2,0,6,105,109,112,111,114,116,0,0,13,1,2,0,
12,2,0,8,116,111,107,101,110,105,122,101,0,0,0,0,
31,0,2,1,19,0,1,0,12,1,0,8,116,111,107,101,
110,105,122,101,0,0,0,0,14,1,0,0,12,2,0,6,
105,109,112,111,114,116,0,0,13,1,2,0,12,2,0,5,
112,97,114,115,101,0,0,0,31,0,2,1,19,0,1,0,
12,1,0,5,112,97,114,115,101,0,0,0,14,1,0,0,
12,2,0,6,105,109,112,111,114,116,0,0,13,1,2,0,
12,2,0,6,101,110,99,111,100,101,0,0,31,0,2,1,
19,0,1,0,12,1,0,6,101,110,99,111,100,101,0,0,
14,1,0,0,16,0,0,49,44,11,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,12,6,0,8,
116,111,107,101,110,105,122,101,0,0,0,0,13,5,6,0,
12,6,0,8,116,111,107,101,110,105,122,101,0,0,0,0,
9,5,5,6,15,6,1,0,31,4,6,1,19,4,5,4,
15,3,4,0,12,7,0,5,112,97,114,115,101,0,0,0,
13,6,7,0,12,7,0,5,112,97,114,115,101,0,0,0,
9,6,6,7,15,7,1,0,15,8,3,0,31,5,7,2,
19,5,6,5,15,4,5,0,12,8,0,6,101,110,99,111,
100,101,0,0,13,7,8,0,12,8,0,6,101,110,99,111,
100,101,0,0,9,7,7,8,15,8,2,0,15,9,1,0,
15,10,4,0,31,6,8,3,19,6,7,6,15,5,6,0,
20,5,0,0,0,0,0,0,12,1,0,8,95,99,111,109,
112,105,108,101,0,0,0,0,14,1,0,0,16,1,0,155,
44,12,0,0,28,2,0,0,9,1,0,2,12,3,0,7,
77,79,68,85,76,69,83,0,13,2,3,0,36,2,2,1,
21,2,0,0,18,0,0,8,12,3,0,7,77,79,68,85,
76,69,83,0,13,2,3,0,9,2,2,1,20,2,0,0,
18,0,0,1,12,4,0,3,46,112,121,0,1,3,1,4,
15,2,3,0,12,5,0,4,46,116,112,99,0,0,0,0,
1,4,1,5,15,3,4,0,12,6,0,6,101,120,105,115,
116,115,0,0,13,5,6,0,15,6,2,0,31,4,6,1,
19,4,5,4,21,4,0,0,18,0,0,64,11,6,0,0,
0,0,0,0,0,0,240,63,11,4,0,0,0,0,0,0,
0,0,0,0,12,9,0,6,101,120,105,115,116,115,0,0,
13,8,9,0,15,9,3,0,31,7,9,1,19,7,8,7,
23,4,4,7,23,5,4,6,21,5,0,0,18,0,0,2,
18,0,0,16,12,8,0,5,109,116,105,109,101,0,0,0,
13,7,8,0,15,8,3,0,31,4,8,1,19,4,7,4,
12,9,0,5,109,116,105,109,101,0,0,0,13,8,9,0,
15,9,2,0,31,7,9,1,19,7,8,7,25,4,4,7,
21,4,0,0,18,0,0,28,12,7,0,4,108,111,97,100,
0,0,0,0,13,6,7,0,15,7,2,0,31,5,7,1,
19,5,6,5,15,4,5,0,12,8,0,8,95,99,111,109,
112,105,108,101,0,0,0,0,13,7,8,0,15,8,4,0,
15,9,2,0,31,6,8,2,19,6,7,6,15,5,6,0,
12,8,0,4,115,97,118,101,0,0,0,0,13,7,8,0,
15,8,3,0,15,9,5,0,31,6,8,2,19,6,7,6,
18,0,0,1,18,0,0,1,11,6,0,0,0,0,0,0,
0,0,0,0,12,9,0,6,101,120,105,115,116,115,0,0,
13,8,9,0,15,9,3,0,31,7,9,1,19,7,8,7,
23,6,6,7,21,6,0,0,18,0,0,4,28,6,0,0,
37,6,0,0,18,0,0,1,12,8,0,4,108,111,97,100,
0,0,0,0,13,7,8,0,15,8,3,0,31,6,8,1,
19,6,7,6,15,5,6,0,12,8,0,8,95,95,110,97,
109,101,95,95,0,0,0,0,15,9,1,0,12,10,0,8,
95,95,99,111,100,101,95,95,0,0,0,0,15,11,5,0,
26,7,8,4,15,6,7,0,12,7,0,8,95,95,100,105,
99,116,95,95,0,0,0,0,10,6,7,6,12,8,0,7,
77,79,68,85,76,69,83,0,13,7,8,0,10,7,1,6,
12,9,0,4,101,120,101,99,0,0,0,0,13,8,9,0,
15,9,5,0,15,10,6,0,31,7,9,2,19,7,8,7,
20,6,0,0,0,0,0,0,12,2,0,7,95,105,109,112,
111,114,116,0,14,2,1,0,16,2,0,30,44,4,0,0,
12,2,0,8,66,85,73,76,84,73,78,83,0,0,0,0,
13,1,2,0,12,3,0,8,95,99,111,109,112,105,108,101,
0,0,0,0,13,2,3,0,12,3,0,7,99,111,109,112,
105,108,101,0,10,1,3,2,12,2,0,8,66,85,73,76,
84,73,78,83,0,0,0,0,13,1,2,0,12,3,0,7,
95,105,109,112,111,114,116,0,13,2,3,0,12,3,0,6,
105,109,112,111,114,116,0,0,10,1,3,2,0,0,0,0,
12,3,0,5,95,105,110,105,116,0,0,0,14,3,2,0,
16,3,0,51,44,10,0,0,28,2,0,0,9,1,0,2,
28,3,0,0,9,2,0,3,26,4,0,0,15,3,4,0,
12,4,0,8,95,95,110,97,109,101,95,95,0,0,0,0,
10,3,4,2,12,5,0,7,77,79,68,85,76,69,83,0,
13,4,5,0,10,4,2,3,12,7,0,4,108,111,97,100,
0,0,0,0,13,6,7,0,15,7,1,0,31,5,7,1,
19,5,6,5,15,4,5,0,12,8,0,8,95,99,111,109,
112,105,108,101,0,0,0,0,13,7,8,0,15,8,4,0,
15,9,1,0,31,6,8,2,19,6,7,6,15,5,6,0,
12,6,0,8,95,95,99,111,100,101,95,95,0,0,0,0,
10,3,6,5,12,8,0,4,101,120,101,99,0,0,0,0,
13,7,8,0,15,8,5,0,15,9,3,0,31,6,8,2,
19,6,7,6,20,3,0,0,0,0,0,0,12,4,0,12,
105,109,112,111,114,116,95,102,110,97,109,101,0,0,0,0,
14,4,3,0,16,4,0,24,44,6,0,0,12,3,0,12,
105,109,112,111,114,116,95,102,110,97,109,101,0,0,0,0,
13,2,3,0,12,5,0,4,65,82,71,86,0,0,0,0,
13,3,5,0,11,5,0,0,0,0,0,0,0,0,0,0,
9,3,3,5,12,4,0,8,95,95,109,97,105,110,95,95,
0,0,0,0,31,1,3,2,19,1,2,1,20,1,0,0,
0,0,0,0,12,5,0,6,116,105,110,121,112,121,0,0,
14,5,4,0,16,5,0,33,44,9,0,0,28,2,0,0,
9,1,0,2,28,3,0,0,9,2,0,3,12,6,0,4,
108,111,97,100,0,0,0,0,13,5,6,0,15,6,1,0,
31,4,6,1,19,4,5,4,15,3,4,0,12,7,0,8,
95,99,111,109,112,105,108,101,0,0,0,0,13,6,7,0,
15,7,3,0,15,8,1,0,31,5,7,2,19,5,6,5,
15,4,5,0,12,7,0,4,115,97,118,101,0,0,0,0,
13,6,7,0,15,7,2,0,15,8,4,0,31,5,7,2,
19,5,6,5,0,0,0,0,12,6,0,4,109,97,105,110,
0,0,0,0,14,6,5,0,12,7,0,8,95,95,110,97,
109,101,95,95,0,0,0,0,13,6,7,0,12,7,0,8,
95,95,109,97,105,110,95,95,0,0,0,0,23,6,6,7,
21,6,0,0,18,0,0,24,12,8,0,4,109,97,105,110,
0,0,0,0,13,7,8,0,12,10,0,4,65,82,71,86,
0,0,0,0,13,8,10,0,11,10,0,0,0,0,0,0,
0,0,240,63,9,8,8,10,12,10,0,4,65,82,71,86,
0,0,0,0,13,9,10,0,11,10,0,0,0,0,0,0,
0,0,0,64,9,9,9,10,31,6,8,2,19,6,7,6,
18,0,0,1,0,0,0,0,
};
