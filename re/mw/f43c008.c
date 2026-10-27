
void FUN_0043c008(int param_1,int param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  int *in_FS_OFFSET;
  undefined4 *puStackY_44;
  undefined4 *puVar5;
  undefined1 *puStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined1 local_14;
  undefined4 local_10;
  undefined1 local_c;
  undefined4 local_8;
  
  puStack_34 = &stack0xfffffffc;
  local_8 = 0;
  local_1c = 0;
  local_20 = 0;
  local_24 = 0;
  puStack_38 = &LAB_0043c268;
  puStack_3c = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = (int)&puStack_3c;
  puVar2 = &stack0xfffffffc;
  if (-1 < param_2) {
    iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0x230) + 0x114) + 0x14))();
    puVar2 = puStack_34;
    if (param_2 < iVar3) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x230) + 0x114);
      iVar4 = (**(code **)(*piVar1 + 0x18))(piVar1,param_2);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x230),param_2);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x238),param_2);
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x234),param_2);
      SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar4 + 0xc),&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x224),local_8);
      puVar5 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_RaceNamesRStr_B11890CE_exref +
                  *(char *)(*(int *)(param_1 + 0x2b0) + 0x45) * 4),&local_1c);
      local_18 = local_1c;
      local_14 = 0xb;
      local_10 = 0xfffffff1;
      local_c = 0;
      System_LoadResString_02614E1B(AoWE_XRelationMinusYRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_20);
      AoWE_Format_D8F48273(local_20,&local_18,1,puVar5);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x298),local_8);
      iVar3 = *(int *)(param_1 + 0x298);
      System__LStrCat3_51F89FF7(&local_8,*(undefined4 *)(iVar3 + 0x90),&DAT_0043c280);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(iVar3,local_8);
      puStackY_44 = &local_1c;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(AoWE_RaceNamesRStr_B11890CE_exref + *(char *)(iVar4 + 8) * 4),
                 &local_20);
      local_18 = local_20;
      local_14 = 0xb;
      local_10 = 10;
      local_c = 0;
      System_LoadResString_02614E1B(AoWE_XRelationPlusYRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_24);
      AoWE_Format_D8F48273(local_24,&local_18,1);
      iVar3 = *(int *)(param_1 + 0x298);
      System__LStrCat3_51F89FF7(&local_8,*(undefined4 *)(iVar3 + 0x90),local_1c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(iVar3,local_8);
      (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),1);
      System_LoadResString_02614E1B(AoWE_MigrateRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_8);
      goto LAB_0043c245;
    }
  }
  puStack_34 = puVar2;
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x230),0xffffffff);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x238),0xffffffff);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x234),0xffffffff);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x298),0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x224),&DAT_0043c28c);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x270),0);
  (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),0);
LAB_0043c245:
  puVar2 = puStack_3c;
  *in_FS_OFFSET = (int)puStackY_44;
  puStack_3c = &LAB_0043c26f;
  System__LStrArrayClr_51F89FF7(&local_24,3,puVar2);
  System__LStrClr_40929B27(&local_8);
  return;
}

