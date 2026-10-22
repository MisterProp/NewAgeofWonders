
void FUN_0042b248(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar3;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_14;
  undefined1 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_0042b38b;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar2 = FUN_0042b14c(param_1,2);
  if (iVar2 == 0) {
    aowDPlayComp_TGDPlay_SetPlayerName_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44),
               *(undefined4 *)(*(int *)(param_1 + 0x90) + 0x90));
    puVar3 = &local_8;
    System_LoadResString_02614E1B(AoWE_XsGameRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_c);
    local_14 = *(undefined4 *)(*(int *)(param_1 + 0x90) + 0x90);
    local_10 = 0xb;
    AoWE_Format_D8F48273(local_c,&local_14,0,puVar3);
    System__LStrAsg_51F89FF7(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x24,local_8);
    iVar2 = aowDPlayComp_TGDPlay_CreateSession_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44));
    if (iVar2 == 0) {
      (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
      AoWSetup_TSetupControl_SetupHost_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x48),
                 *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x28),1,
                 *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 500));
      if (*(int *)(param_1 + 0xec) != 0) {
        AoWSetup_TSetupControl_hPickMap_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x48),*(undefined4 *)(param_1 + 0xec))
        ;
      }
      FUN_00410bd8(*(undefined4 *)PTR_DAT_0045a30c,*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x48))
      ;
    }
  }
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_0042b392;
  puStack_20 = (undefined1 *)0x42b38a;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

