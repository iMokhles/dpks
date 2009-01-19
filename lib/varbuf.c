/*
 * libdpkg - Debian packaging suite library routines
 * varbuf.c - variable length expandable buffer handling
 *
 * Copyright © 1994,1995 Ian Jackson <ian@chiark.greenend.org.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <config.h>
#include <compat.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <dpkg.h>
#include <dpkg-db.h>

void
varbufaddc(struct varbuf *v, int c)
{
  if (v->used >= v->size) varbufextend(v);
  v->buf[v->used++]= c;
}

void
varbufdupc(struct varbuf *v, int c, size_t n)
{
  size_t old_used = v->used;

  v->used += n;
  if (v->used >= v->size) varbufextend(v);

  memset(v->buf + old_used, c, n);
}

int varbufprintf(struct varbuf *v, const char *fmt, ...) {
  int r;
  va_list al;

  va_start(al, fmt);
  r = varbufvprintf(v, fmt, al);
  va_end(al);

  return r;
}

int varbufvprintf(struct varbuf *v, const char *fmt, va_list va) {
  size_t ou;
  int r;
  va_list al;

  ou= v->used;
  v->used+= strlen(fmt);

  do {
    varbufextend(v);
    va_copy(al, va);
    r= vsnprintf(v->buf+ou,v->size-ou,fmt,al);
    va_end(al);
    if (r < 0) r= (v->size-ou+1) * 2;
    v->used= ou+r;
  } while (r >= (int)(v->size - ou - 1));
  return r;
}

void
varbufaddbuf(struct varbuf *v, const void *s, size_t l)
{
  int ou;
  ou= v->used;
  v->used += l;
  if (v->used >= v->size) varbufextend(v);
  memcpy(v->buf + ou, s, l);
}

void
varbufinit(struct varbuf *v, size_t size)
{
  v->used = 0;
  v->size = size;
  if (size)
    v->buf = m_malloc(size);
  else
    v->buf = NULL;
}

void varbufreset(struct varbuf *v) {
  v->used= 0;
}

void varbufextend(struct varbuf *v) {
  int newsize;
  char *newbuf;

  newsize= v->size + 80 + v->used;
  newbuf = m_realloc(v->buf, newsize);
  v->size= newsize;
  v->buf= newbuf;
}

void varbuffree(struct varbuf *v) {
  free(v->buf); v->buf=NULL; v->size=0; v->used=0;
}
