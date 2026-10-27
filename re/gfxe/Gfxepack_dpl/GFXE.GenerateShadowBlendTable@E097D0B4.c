// /Gfxepack.dpl : GFXE.GenerateShadowBlendTable@E097D0B4

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void GFXE_GenerateShadowBlendTable_E097D0B4(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
                    /* 0xa79c  98  GFXE.GenerateShadowBlendTable@E097D0B4 */
  cVar1 = System__IsClass_51F89FF7(param_1,&GFXE__TBlendTable5Bit_7FE8F72A);
  if (cVar1 != '\0') {
    iVar4 = 0;
    do {
      iVar3 = 0;
      do {
        iVar2 = System__ROUND_51F89FF7();
        if (iVar2 < 0) {
          iVar2 = 0;
        }
        if (iVar3 < iVar2) {
          iVar2 = iVar3;
        }
        *(char *)(param_1 + iVar4 * 0x20 + 0xc + iVar3) = (char)iVar2;
        iVar3 = iVar3 + 1;
      } while (iVar3 != 0x20);
      iVar4 = iVar4 + 1;
    } while (iVar4 != 0x20);
  }
  return;
}

