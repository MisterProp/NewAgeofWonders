
void FUN_00411f50(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  
  puStack_14 = &stack0xfffffffc;
  puStack_18 = &LAB_00412049;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  cVar1 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  if (cVar1 != '\0') {
    cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
    if (cVar1 != '\0') {
      iVar3 = *(int *)(*(int *)(param_1 + 0x33c) + 0x28);
      iVar4 = 0;
      while( true ) {
        iVar2 = (**(code **)(**(int **)(iVar3 + 0x38) + 0x54))();
        if (iVar4 == iVar2) break;
        iVar2 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(iVar3 + 0x38),iVar4);
        if (*(int *)(iVar2 + 0xc) != *(int *)(iVar3 + 0xc)) break;
        iVar4 = iVar4 + 1;
      }
      iVar2 = (**(code **)(**(int **)(iVar3 + 0x38) + 0x54))();
      if (iVar4 != iVar2) {
        iVar3 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(iVar3 + 0x38),iVar4);
        AoWSetup_TSetupControl_hSetHostRemoteID_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x33c),*(undefined4 *)(iVar3 + 0xc));
      }
    }
    if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1ed) != '\0') {
      aowDPlayComp_TGDPlay_CloseSession_23EDC2EF
                (*(int *)(*(int *)PTR_DAT_0045a454 + 0x44),PTR_DAT_0045a454);
    }
  }
  aowDPlayComp_TGDPlay_DisConnect_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44));
  FUN_00410f10(param_1);
  if (*(char *)(*(int *)PTR_DAT_0045a454 + 0x4c) == '\0') {
    FUN_0042abf0(*(undefined4 *)PTR_DAT_0045a598,0);
  }
  else {
    Forms_TCustomForm_Close_23EDC2EF(*(undefined4 *)PTR_DAT_0045a168);
  }
  *in_FS_OFFSET = uStack_1c;
  return;
}

