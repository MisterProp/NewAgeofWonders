
void FUN_00413d10(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_1c = &LAB_00413dbc;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  puVar1 = &stack0xfffffffc;
  if (*(int *)(*(int *)(*(int *)PTR_DAT_0045a24c + 0x58) + 0x11c) < param_3) {
    SysUtils_IntToStr_0F6FDFF6(param_3 / 1000,&local_c);
    SysUtils_IntToStr_0F6FDFF6(param_4 / 1000,&local_10,1000,"k of ",local_c);
    System__LStrCatN_51F89FF7(&local_8,4);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x50),local_8);
    AOWGauge_TAoWGauge_SetPos_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x58),param_3);
    puVar1 = puStack_18;
  }
  puStack_18 = puVar1;
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00413dc3;
  puStack_1c = (undefined1 *)0x413dbb;
  System__LStrArrayClr_51F89FF7(&local_10,3,puVar1);
  return;
}

