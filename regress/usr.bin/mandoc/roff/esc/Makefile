# $OpenBSD: Makefile,v 1.10 2015/01/21 20:20:49 schwarze Exp $

REGRESS_TARGETS = one two multi B c c_man e f h o w z ignore
LINT_TARGETS	= B h w ignore

# Postprocessing to remove "character backspace" sequences
# unless they are followed by the same character again.
# This removes underlining as well, so we mustn't use it.
# Cannot use /g because matches progress backwards.

z.out_ascii: z.in
	${NROFF} ${NOPTS} -Tascii ${.ALLSRC} | \
		perl -pe 'while (s/(.)\010(?!\1)//) {}' \
		> ${.TARGET}

.include <bsd.regress.mk>
