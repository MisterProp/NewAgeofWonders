
void FUN_00442f08(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_0044305e;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0x18c) + 0x6c))(*(int **)(param_1 + 0x18c),1);
  uVar4 = (**(code **)(**(int **)(*(int *)PTR_DAT_0045a420 + 0xa8) + 0x144))();
  piVar1 = *(int **)(*(int *)(param_1 + 400) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,uVar4);
  System_LoadResString_02614E1B(AoWE_StatusRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  System__LStrCat_51F89FF7(&local_8,&DAT_00443074);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x198),local_8);
  cVar3 = (**(code **)(*param_2 + 0x1f0))();
  if (cVar3 == '\0') {
    System_LoadResString_02614E1B(AoWE_UnexploredRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x19c),local_8);
  }
  else {
    System_LoadResString_02614E1B(AoWE_ExploredRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x19c),local_8);
  }
  uVar4 = ExploreS_TExplorationSite_CanSearch_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa8),
                     *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),uVar4);
  System_LoadResString_02614E1B(AoWE_SearchRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_8);
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00443065;
  puStack_1c = (undefined1 *)0x44305d;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

