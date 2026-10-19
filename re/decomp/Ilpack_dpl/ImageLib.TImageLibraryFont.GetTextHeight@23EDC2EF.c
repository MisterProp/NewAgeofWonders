// /Ilpack.dpl : ImageLib.TImageLibraryFont.GetTextHeight@23EDC2EF

void ImageLib_TImageLibraryFont_GetTextHeight_23EDC2EF(int param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x14448  502  ImageLib.TImageLibraryFont.GetTextHeight@23EDC2EF */
  puStack_20 = &stack0xfffffffc;
  local_10 = 0;
  puStack_24 = &LAB_5521450c;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  if (*(char *)(param_1 + 0x58) == '\x01') {
    SysUtils_UpperCase_435799AA(param_2,&local_10);
  }
  else if (*(char *)(param_1 + 0x58) == '\x02') {
    puStack_20 = &stack0xfffffffc;
    SysUtils_LowerCase_435799AA(param_2,&local_10);
  }
  else {
    puStack_20 = &stack0xfffffffc;
    System__LStrLAsg_51F89FF7(&local_10,param_2);
  }
  local_c = 0;
  iVar2 = System__LStrLen_C43AB84F(local_10);
  if (0 < iVar2) {
    local_8 = 1;
    do {
      uVar4 = (uint)*(byte *)(local_10 + -1 + local_8);
      if ((int)uVar4 < *(int *)(param_1 + 0x44)) {
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,uVar4);
        if (iVar3 != 0) {
          iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,uVar4);
          if (local_c < *(int *)(iVar3 + 0x24)) {
            iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,uVar4);
            local_c = *(int *)(iVar3 + 0x24);
          }
        }
      }
      local_8 = local_8 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  puVar1 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_55214513;
  puStack_24 = (undefined1 *)0x5521450b;
  System__LStrClr_40929B27(&local_10,uStack_28,puVar1);
  return;
}

