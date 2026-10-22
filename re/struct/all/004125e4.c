
void FUN_004125e4(int param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(*(int *)(param_1 + 0x5c) + 0x120);
  if (-1 < iVar1) {
    piVar2 = *(int **)(*(int *)(param_1 + 0x5c) + 0x114);
    uVar3 = (**(code **)(*piVar2 + 0x18))(piVar2,iVar1);
    AoWSetup_TSetupControl_hSetHostRemoteID_23EDC2EF(*(undefined4 *)(param_1 + 0x33c),uVar3);
  }
  return;
}

