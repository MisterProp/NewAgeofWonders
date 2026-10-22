
void FUN_0040b08c(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined *puVar4;
  undefined1 *puVar5;
  undefined4 uVar6;
  undefined *puVar7;
  undefined4 uVar8;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  undefined4 local_c;
  int local_8;
  
  puStack_2c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_30 = &LAB_0040b241;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x114) + 0x40))();
  System_LoadResString_02614E1B(AoWE_DesktopWindowRStr_04ECFCC5_exref,&local_18);
  AoWE_TranslateRStr_435799AA(local_18,&local_14);
  piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
  (**(code **)(*piVar1 + 0x38))(piVar1,local_14,0xffffffff);
  iVar2 = GFXE_TDisplayModeList_GetCount_23EDC2EF(*(undefined4 *)(param_1 + 0x8c));
  if (-1 < iVar2 + -1) {
    local_8 = 0;
    local_10 = iVar2;
    do {
      iVar2 = GFXE_TDisplayModeList_GetItems_23EDC2EF(*(undefined4 *)(param_1 + 0x8c),local_8);
      if ((0x27f < *(int *)(iVar2 + 8)) && (0x1df < *(int *)(iVar2 + 0xc))) {
        System_LoadResString_02614E1B(AoWE_FullScreenRStr_04ECFCC5_exref,&local_1c);
        AoWE_TranslateRStr_435799AA(local_1c,&local_18);
        puVar7 = &DAT_0040b258;
        uVar8 = local_18;
        SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar2 + 8),&local_1c);
        puVar4 = &DAT_0040b264;
        uVar6 = local_1c;
        SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar2 + 0xc),&local_20);
        uVar3 = local_20;
        System__LStrCatN_51F89FF7(&local_14,5);
        piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
        (**(code **)(*piVar1 + 0x38))(piVar1,local_14,local_8,uVar3,puVar4,uVar6,puVar7,uVar8);
      }
      local_8 = local_8 + 1;
      local_10 = local_10 + -1;
    } while (local_10 != 0);
  }
  uVar3 = FUN_0040b5cc(param_1);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x70),uVar3);
  if (*(int *)(*(int *)(param_1 + 0x70) + 0x120) != -1) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
    (**(code **)(*piVar1 + 0xc))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x70) + 0x120),&local_c);
    puVar4 = &DAT_0040b270;
    uVar6 = local_c;
    System_LoadResString_02614E1B(AoWE_CurrentRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_18);
    SysUtils_LowerCase_435799AA(local_18,&local_14);
    puVar5 = &LAB_0040b27c;
    uVar3 = local_14;
    System__LStrCatN_51F89FF7(&local_c,4);
    piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
    (**(code **)(*piVar1 + 0x20))
              (piVar1,*(undefined4 *)(*(int *)(param_1 + 0x70) + 0x120),local_c,puVar5,uVar3,puVar4,
               uVar6);
  }
  puVar5 = puStack_2c;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_0040b248;
  puStack_30 = (undefined1 *)0x40b238;
  System__LStrArrayClr_51F89FF7(&local_20,4,puVar5);
  puStack_30 = (undefined1 *)0x40b240;
  System__LStrClr_40929B27(&local_c);
  return;
}

