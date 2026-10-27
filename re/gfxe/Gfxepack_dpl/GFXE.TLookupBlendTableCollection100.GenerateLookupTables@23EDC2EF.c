// /Gfxepack.dpl : GFXE.TLookupBlendTableCollection100.GenerateLookupTables@23EDC2EF

/* WARNING: Removing unreachable block (ram,0x5500aafb) */

undefined4
GFXE_TLookupBlendTableCollection100_GenerateLookupTables_23EDC2EF
          (int param_1,int param_2,code *param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  int local_1c;
  int local_18;
  
                    /* 0xaaa8  81  GFXE.TLookupBlendTableCollection100.GenerateLookupTables@23EDC2EF
                        */
  GFXE_TLookupBlendTableCollection100_Clear_23EDC2EF(param_1);
  if ((param_2 == 5) && (0 < param_4)) {
    if (-1 < param_4 + -1) {
      local_1c = param_4;
      do {
        uVar1 = GFXE_TBlendTable5Bit_Create_23EDC2EF(&GFXE__TBlendTable5Bit_7FE8F72A,1);
        (*param_3)(uVar1);
        GFXE_TBlendTableList_Add_23EDC2EF(*(undefined4 *)(param_1 + 4),uVar1);
        local_1c = local_1c + -1;
      } while (local_1c != 0);
    }
    iVar2 = 0;
    do {
      local_18 = (iVar2 * param_4) / 100;
      if (param_4 == local_18) {
        local_18 = param_4 + -1;
      }
      uVar1 = GFXE_TBlendTableList_GetTables_23EDC2EF(*(undefined4 *)(param_1 + 4),local_18);
      *(undefined4 *)(param_1 + 8 + iVar2 * 4) = uVar1;
      iVar2 = iVar2 + 1;
    } while (iVar2 != 0x65);
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

