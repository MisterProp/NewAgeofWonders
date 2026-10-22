
void FUN_0040f4bc(int param_1,int param_2,undefined4 param_3,int param_4)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_10 = 0;
  puStack_24 = &LAB_0040f571;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  local_c = param_3;
  local_8 = param_2;
  iVar2 = (**(code **)(**(int **)(param_1 + 0xe0) + 0x14))();
  if (local_8 < iVar2) {
    *(undefined1 *)(param_4 + 0xc0) = 1;
    (**(code **)(**(int **)(param_1 + 0xe0) + 0xc))(*(int **)(param_1 + 0xe0),local_8,&local_10);
    AoWEdit_TAOWEdit_SetGText_23EDC2EF(param_4,local_10);
    iVar2 = (**(code **)(**(int **)(param_1 + 0xe0) + 0x18))(*(int **)(param_1 + 0xe0),local_8);
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)(AoWE_RaceNamesPluralRStr_B11890CE_exref + iVar2 * 4),&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(local_c,local_10);
  }
  else {
    *(undefined1 *)(param_4 + 0xc0) = 0;
    AoWEdit_TAOWEdit_SetGText_23EDC2EF(param_4,0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(local_c,0);
  }
  puVar1 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_0040f578;
  puStack_24 = (undefined1 *)0x40f570;
  System__LStrClr_40929B27(&local_10,uStack_28,puVar1);
  return;
}

