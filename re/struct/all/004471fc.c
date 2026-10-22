
void FUN_004471fc(int param_1)

{
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  if (*(int *)(param_1 + 0x1d0) == 2) {
    *(undefined4 *)(param_1 + 0x1d0) = 3;
  }
  else {
    *(undefined4 *)(param_1 + 0x1d0) = 2;
  }
  FUN_004467ec(param_1);
  return;
}

