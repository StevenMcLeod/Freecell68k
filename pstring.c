/* Parameter List and Prototypes disabled so that stack frame not created */

#pragma options(!require_protos)
StringPtr strcat_p(/*StringPtr s1, const StringPtr s2*/) {
	asm {
		MOVEA.L	4(sp), a0		;  A0 = s1
		MOVEA.L	8(sp), a1		;  A1 = s2
		CLR.L	d0
		CLR.L	d1
		MOVE.B	(a0), d0		;  D0 = n(s1)
		MOVE.B	(a1)+, d1		;  D1 = n(s2)
		ADD.B	d1, (a0)+		;  Update n(s1)
		ADDA.L	d0, a0			;  Offset s1
		TST.B	d1
		BRA.S	@2
@1		MOVE.B	(a1)+, (a0)+
		SUBQ.B	#1, d1
@2		BNE.S	@1

		MOVE.L	4(sp), d0
	}
}