// /AoW.exe : 00440f2c

void MainBtn3Click(int param_1)

{
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  if (*(char *)(param_1 + 500) == '\0') {
    (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
    return;
  }
  FUN_0043fd4c(param_1,0);
  return;
}

