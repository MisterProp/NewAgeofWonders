
void FUN_00412a24(int param_1,int param_2)

{
  int iVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar5;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined1 local_10 [8];
  int local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_14 = 0;
  puStack_28 = &LAB_00412ab7;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  local_8 = param_2;
  (**(code **)(**(int **)(param_2 + 0x11c) + 0xc))
            (*(int **)(param_2 + 0x11c),*(undefined4 *)(param_2 + 0x138),&local_14);
  cVar3 = AoWSetup_TSetupControl_PlayerNameToPlayerID_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x33c),local_14,local_10);
  if (cVar3 != '\0') {
    uVar5 = 1;
    iVar1 = *(int *)(param_1 + 0x33c);
    uVar4 = AoWSetup_TSetupSettings_IndexOfRace_23EDC2EF
                      (*(undefined4 *)(iVar1 + 0x28),*(undefined1 *)(param_2 + 0xc));
    AoWSetup_TSetupControl_hSetPlayerID_23EDC2EF(iVar1,uVar4,local_10,uVar5);
  }
  puVar2 = puStack_24;
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_00412abe;
  puStack_28 = (undefined1 *)0x412ab6;
  System__LStrClr_40929B27(&local_14,uStack_2c,puVar2);
  return;
}

