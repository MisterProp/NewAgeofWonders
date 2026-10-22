
void FUN_0040f678(int param_1,int param_2)

{
  char cVar1;
  int *piVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_10 = 0;
  local_14 = 0;
  puStack_28 = &LAB_0040f7d2;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  local_8 = param_1;
  FUN_0040f440(param_1);
  *(int *)(local_8 + 0xe4) = param_2;
  local_c = param_2;
  iVar4 = (**(code **)(**(int **)(param_2 + 0x28) + 0x54))();
  if (-1 < iVar4 + -1) {
    iVar6 = 0;
    do {
      iVar5 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(*(undefined4 *)(local_c + 0x28),iVar6);
      if (*(char *)(iVar5 + 8) == '\0') {
        iVar5 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF(*(undefined4 *)(local_c + 0x28),iVar6);
        cVar1 = *(char *)(iVar5 + 0x18);
        AoWReg_TAoWRegistry_GetLocalEmailAddress_23EDC2EF
                  (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),&local_10);
        (**(code **)(**(int **)(local_8 + 0xe0) + 0x38))
                  (*(int **)(local_8 + 0xe0),local_10,(int)cVar1);
      }
      iVar6 = iVar6 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  iVar4 = (**(code **)(**(int **)(local_8 + 0xe0) + 0x14))();
  if (iVar4 < 2) {
    FUN_0040e7e4(*(undefined4 *)PTR_DAT_0045a1a0);
    piVar2 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar2 + 0x34))(piVar2,0);
    System_LoadResString_02614E1B
              (AoWE_YouNeed2HumanPlayersToPlayAnEmailGameRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_10);
    piVar2 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar2 + 0x34))(piVar2,local_10);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70),1);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44),1);
  }
  else {
    FUN_0040f584(local_8);
    (**(code **)(**(int **)(local_8 + 0x54) + 0x6c))(*(int **)(local_8 + 0x54),1);
  }
  puVar3 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_0040f7d9;
  puStack_28 = (undefined1 *)0x40f7d1;
  System__LStrArrayClr_51F89FF7(&local_14,2,puVar3);
  return;
}

