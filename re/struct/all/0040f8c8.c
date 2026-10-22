
void FUN_0040f8c8(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_10 = 0;
  local_14 = 0;
  puStack_28 = &LAB_0040fa36;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  cVar3 = FUN_0040f828(param_1);
  if (cVar3 == '\0') {
    FUN_0040e7e4(*(undefined4 *)PTR_DAT_0045a1a0);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,0);
    System_LoadResString_02614E1B(AoWE_PleaseFillInAllSettingsRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_10);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70),1);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44),1);
  }
  else if (*(int *)(param_1 + 0xe4) != 0) {
    iVar4 = (**(code **)(**(int **)(param_1 + 0xe0) + 0x14))();
    if (-1 < iVar4 + -1) {
      iVar6 = 0;
      local_c = iVar4;
      do {
        uVar5 = (**(code **)(**(int **)(param_1 + 0xe0) + 0x18))(*(int **)(param_1 + 0xe0),iVar6);
        local_8 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF
                            (*(undefined4 *)(*(int *)(param_1 + 0xe4) + 0x28),uVar5);
        iVar4 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF
                          (*(undefined4 *)(*(int *)(param_1 + 0xe4) + 0x28),local_8);
        (**(code **)(**(int **)(param_1 + 0xe0) + 0xc))(*(int **)(param_1 + 0xe0),iVar6,&local_10);
        System__LStrAsg_51F89FF7(iVar4 + 0x24,local_10);
        iVar6 = iVar6 + 1;
        local_c = local_c + -1;
      } while (local_c != 0);
    }
    System__LStrAsg_51F89FF7
              (*(int *)(*(int *)(param_1 + 0xe4) + 0x28) + 0x40,
               *(undefined4 *)(*(int *)(param_1 + 0x9c) + 0x90));
    *PTR_DAT_0045a040 = 1;
    (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
  }
  puVar2 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_0040fa3d;
  puStack_28 = (undefined1 *)0x40fa35;
  System__LStrArrayClr_51F89FF7(&local_14,2,puVar2);
  return;
}

