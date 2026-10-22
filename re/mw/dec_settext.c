void ImageLib_TImageLibraryFont_SetText_23EDC2EF
               (int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5)
{
  char cVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
                    /* 0x1451c  501  ImageLib.TImageLibraryFont.SetText@23EDC2EF */
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_552145b0;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  cVar1 = *(char *)(param_1 + 0x58);
  if (cVar1 == '\0') {
    FUN_55214320(param_1,param_2,param_3,param_4,param_5);
  }
  else if (cVar1 == '\x01') {
    puStack_18 = &stack0xfffffffc;
    SysUtils_UpperCase_435799AA(param_5,&local_8);
    FUN_55214320(param_1,param_2,param_3,param_4,local_8);
  }
  else {
    puStack_18 = &stack0xfffffffc;
    if (cVar1 == '\x02') {
      puStack_18 = &stack0xfffffffc;
      SysUtils_LowerCase_435799AA(param_5,&local_8);
      FUN_55214320(param_1,param_2,param_3,param_4,local_8);
    }
  }
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_552145b7;
  puStack_1c = (undefined1 *)0x552145af;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar2);
  return;
}
