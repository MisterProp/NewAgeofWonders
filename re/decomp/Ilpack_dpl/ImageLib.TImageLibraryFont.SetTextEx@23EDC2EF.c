// /Ilpack.dpl : ImageLib.TImageLibraryFont.SetTextEx@23EDC2EF

void ImageLib_TImageLibraryFont_SetTextEx_23EDC2EF
               (int param_1,int param_2,undefined4 param_3,undefined4 param_4,byte param_5,
               undefined4 param_6,undefined4 param_7)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  int local_18;
  int local_14;
  int local_10;
  undefined4 local_c;
  int local_8;
  
                    /* 0x14698  499  ImageLib.TImageLibraryFont.SetTextEx@23EDC2EF */
  puStack_28 = &stack0xfffffffc;
  local_14 = 0;
  puStack_2c = &LAB_5521478e;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  local_c = param_3;
  local_8 = param_2;
  if (*(char *)(param_1 + 0x58) == '\x01') {
    SysUtils_UpperCase_435799AA(param_7,&local_14);
  }
  else if (*(char *)(param_1 + 0x58) == '\x02') {
    puStack_28 = &stack0xfffffffc;
    SysUtils_LowerCase_435799AA(param_7,&local_14);
  }
  else {
    puStack_28 = &stack0xfffffffc;
    System__LStrLAsg_51F89FF7(&local_14,param_7);
  }
  if ((param_5 & 1) != 0) {
    uVar2 = FUN_552142a0(param_1,local_14);
    local_8 = local_8 - (uVar2 >> 1);
  }
  local_18 = System__LStrLen_C43AB84F(local_14);
  if (0 < local_18) {
    local_10 = 1;
    do {
      uVar2 = (uint)*(byte *)(local_14 + -1 + local_10);
      if ((int)uVar2 < *(int *)(param_1 + 0x44)) {
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,uVar2);
        if (iVar3 == 0) goto LAB_55214767;
        uVar5 = param_4;
        uVar6 = param_6;
        piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,uVar2);
        (**(code **)(*piVar4 + 0xa0))(piVar4,local_8,local_c,uVar5,uVar6);
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,uVar2);
        local_8 = local_8 + *(int *)(iVar3 + 0x20) + *(int *)(param_1 + 0x50);
      }
      else {
LAB_55214767:
        local_8 = local_8 + *(int *)(param_1 + 0x50) + 5;
      }
      local_10 = local_10 + 1;
      local_18 = local_18 + -1;
    } while (local_18 != 0);
  }
  puVar1 = puStack_28;
  *in_FS_OFFSET = uStack_30;
  puStack_28 = &LAB_55214795;
  puStack_2c = (undefined1 *)0x5521478d;
  System__LStrClr_40929B27(&local_14,uStack_30,puVar1);
  return;
}

