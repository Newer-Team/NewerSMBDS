nsub_0217A020_ov_52:
	BEQ		0x0217A03C		@If Mario is not in shell
	LDR		R0, [R8, #0x4]	@Load Snowball ptr from Collider
	LDRB	R0, [R0, #0x8]	@Load last sprite data byte
	AND 	R0, R0, #0x1
	CMP		R0, #0x1
	BEQ		0x0217A09C		@Ignore Collision
	B		0x0217A024		@Destroy Snowball