void ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF(int param_1,undefined4 param_2)
{
  char cVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
                    /* 0x143bc  503  ImageLib.TImageLibraryFont.GetTextWidth@23EDC2EF */
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_55214439;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  cVar1 = *(char *)(param_1 + 0x58);
  if (cVar1 == '\0') {
    FUN_552142a0(param_1,param_2);
  }
  else if (cVar1 == '\x01') {
    puStack_14 = &stack0xfffffffc;
    SysUtils_UpperCase_435799AA(param_2,&local_8);
    FUN_552142a0(param_1,local_8);
  }
  else {
    puStack_14 = &stack0xfffffffc;
    if (cVar1 == '\x02') {
      puStack_14 = &stack0xfffffffc;
      SysUtils_LowerCase_435799AA(param_2,&local_8);
      FUN_552142a0(param_1,local_8);
    }
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_55214440;
  puStack_18 = (undefined1 *)0x55214438;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}
