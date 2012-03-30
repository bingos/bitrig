#	$OpenBSD: bsd.dep.mk,v 1.9 2010/03/27 03:47:59 oga Exp $
#	$NetBSD: bsd.dep.mk,v 1.12 1995/09/27 01:15:09 christos Exp $

DEPFILES=${SRCS
# some of the rules involve .h sources, so remove them from mkdep line
.if !target(depend)
depend: beforedepend subdirdepend afterdepend
.ORDER: beforedepend subdirdepend afterdepend
subdirdepend: _SUBDIRUSE

.  if defined(OBJS) && !empty(OBJS)
CFLAGS+= -MD -MP
.   ifnmake clean
.    for o in ${OBJS}
.     if exists(${o:R}.d)
.      include "${o:R}.d"
.     elif exists($o)
       .PHONY: $o
.      endif
.    endfor
.   endif
.  else
.  endif
.  if !target(beforedepend)
beforedepend:
.  endif
.  if !target(afterdepend)
afterdepend:
.  endif
.endif

.if !target(tags)
.  if defined(SRCS)
tags: ${SRCS} _SUBDIRUSE
	-cd ${.CURDIR}; ${CTAGS} -f /dev/stdout -d -t ${.ALLSRC:N*.h} | \
	    sed "s;\${.CURDIR}/;;" > tags
.  else
tags:
.  endif
.endif

.if defined(SRCS)
cleandir: cleandepend
cleandepend:
	rm -f ${OBJS:R:S/$/.d/} ${.CURDIR}/tags
.endif

.PHONY: beforedepend depend afterdepend cleandepend subdirdepend
