
void FUN_00416e28(int param_1)

{
  int iVar1;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(param_1 + 0x2dc);
  if (iVar1 == 1) {
    if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 8) != 0) {
      FUN_00416dec(param_1);
      return;
    }
  }
  else if (iVar1 == 2) {
    FUN_00415a94(param_1);
    if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 1) != 0) {
      FUN_00416650(param_1,1);
      return;
    }
  }
  else if (iVar1 == 3) {
    FUN_00415a94(param_1);
    if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 2) == 0) {
      if ((*(byte *)(*(int *)(param_1 + 0x2d8) + 0x10) & 1) != 0) {
        FUN_00416650(param_1,1);
      }
    }
    else {
      FUN_00416650(param_1,2);
    }
  }
  return;
}

