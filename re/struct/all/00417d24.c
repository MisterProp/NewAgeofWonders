
void FUN_00417d24(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0x2d0) + 0x11c);
  uVar2 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x2d0) + 0x138));
  AoWE_THero_SetRace_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),uVar2);
  return;
}

