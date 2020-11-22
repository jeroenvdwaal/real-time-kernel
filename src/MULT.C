void UMult32Bit(UINT32 *pu32ResHi, UINT32 *pu32ResLo,
		   UINT32 u32Fac1, UINT32 u32Fac2)
{
  /* u32Fac1 is multiplied by u32Fac2, the highest 32 bits of the
     result are stored in u32ResHi, the lowest 32 bits are stored in
     u32ResLo.

     Now follows a short description of how de 32-bit multiply is
     implemented:

			A|B 		A = High(u32Fac1), B = Low(u32Fac1)
			C|D    		C = High(u32Fac1), D = Low(u32Fac1)
     ----------------------- x
		 BD(H)|BD(L)            BD(H) = High(B*D), BD(L) = Low(B*D)
	   AD(H)|AD(L)                  AD(H) = High(A*D), AD(L) = Low(A*D)
	   BC(H)|BC(L)                  BC(H) = High(B*C), BC(L) = Low(B*C)
     AC(H)|AC(L)                        AC(H) = High(A*C), AC(L) = Low(A*C)
     ----------------------- +
     R3(L)|R2(L)|R1(L)|R0(L)		R0(L) = Low(R0) etc.

     R0 = BD(L)
     R1 = BD(H) + AD(L) + BC(L)
     R2 = R1(H) + AD(H) + BC(H) + AC(L)
     R3 = R2(H) + AC(H)                 Note: all variables used, are 16 bit

     Eventually this must be implemented in assembly.
  */
  UINT16 A, B, C, D;
  UINT32 R0, R1, R2, R3;
  UINT32 BD, AD, BC, AC;

  A  = u32Fac1 >> 16;
  B  = u32Fac1 & 0xffff;
  C  = u32Fac2 >> 16;
  D  = u32Fac2 & 0xffff;

  BD = (UINT32)B*D;
  AD = (UINT32)A*D;
  BC = (UINT32)B*C;
  AC = (UINT32)A*C;

  R0 = (BD & 0xffff);
  R1 = (BD >> 16   ) + (AD & 0xffff) + (BC & 0xffff);
  R2 = (R1 >> 16   ) + (AD >> 16   ) + (BC >> 16   ) + (AC & 0xffff);
  R3 = (R2 >> 16   ) + (AC >> 16   );

  *pu32ResHi = (R3 << 16) + (R2 & 0xffff);
  *pu32ResLo = (R1 << 16) + (R0 & 0xffff);
}
