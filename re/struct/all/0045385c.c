
void FUN_0045385c(int param_1)

{
  int *piVar1;
  
  piVar1 = *(int **)(*(int *)PTR_DAT_0045a420 + 0x44);
  (**(code **)(*piVar1 + 0x60))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x1dc) + 0x18c));
  (**(code **)(*piVar1 + 100))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x1dc) + 400));
  if (piVar1[0x1f] < 0x280) {
    (**(code **)(*piVar1 + 0x60))(piVar1,0x280);
  }
  if (piVar1[0x20] < 0x1e0) {
    (**(code **)(*piVar1 + 100))(piVar1,0x1e0);
  }
  FUN_0045431c(*(undefined4 *)PTR_DAT_0045a420);
  return;
}

