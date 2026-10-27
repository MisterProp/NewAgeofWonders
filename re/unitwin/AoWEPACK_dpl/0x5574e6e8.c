// /AoWEPACK.dpl : 0x5574e6e8

void AoWE_TAbility_ShowInfo_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x4e698  2783  AoWE.TAbility.ShowInfo@23EDC2EF */
  puStack_18 = &stack0xfffffffc;
  local_c = 0;
  puStack_1c = &LAB_5574e6f2;
  puStack_20 = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = &puStack_20;
  local_8 = param_3;
  (**(code **)(*param_1 + 0x58))(param_1,param_2,&local_c);
  ImageLib_TImageLibraryFont_SetText_23EDC2EF(local_8,param_6,param_5,param_4,local_c);
  puVar1 = puStack_20;
  *in_FS_OFFSET = param_4;
  puStack_20 = &LAB_5574e6f9;
  System__LStrClr_40929B27(&local_c,param_4,puVar1);
  return;
}

