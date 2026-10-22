
void FUN_00412534(int param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  iVar1 = *(int *)(*(int *)(param_1 + 0x278) + 0x138);
  if (-1 < iVar1) {
    piVar2 = *(int **)(*(int *)(param_1 + 0x278) + 0x11c);
    uVar3 = (**(code **)(*piVar2 + 0x18))(piVar2,iVar1);
    AoWSetup_TSetupControl_SetPreferredRace_23EDC2EF(*(undefined4 *)(param_1 + 0x33c),uVar3);
  }
  return;
}

