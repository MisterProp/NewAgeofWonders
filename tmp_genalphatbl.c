
void GFXE_GenerateAlphaBlendTable_E097D0B4(int param_1)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  
                    /* 0xa578  100  GFXE.GenerateAlphaBlendTable@E097D0B4 */
  cVar1 = System__IsClass_51F89FF7(param_1,&GFXE__TBlendTable5Bit_7FE8F72A);
  if (cVar1 == '\0') {
    SysUtils_Exception_Create_23EDC2EF
              (SysUtils__Exception_9880D0BF_exref,1,"BlendTable format not supported");
    System__RaiseExcept_51F89FF7();
  }
  else {
    iVar4 = 0;
    do {
      iVar3 = 0;
      do {
        uVar2 = System__ROUND_51F89FF7();
        *(undefined1 *)(param_1 + iVar4 * 0x20 + 0xc + iVar3) = uVar2;
        iVar3 = iVar3 + 1;
      } while (iVar3 != 0x20);
      iVar4 = iVar4 + 1;
    } while (iVar4 != 0x20);
  }
  return;
}

