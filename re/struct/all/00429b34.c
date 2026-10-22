
void FUN_00429b34(int param_1)

{
  FUN_0040b06c(*(undefined4 *)PTR_DAT_0045a478,0x280,0x1e0);
  FUN_0040b034(*(undefined4 *)PTR_DAT_0045a478,2);
  FUN_0040b390(*(undefined4 *)PTR_DAT_0045a478);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  (**(code **)(**(int **)(param_1 + 0x4c) + 0xa4))
            (*(int **)(param_1 + 0x4c),*(undefined4 *)(param_1 + 0x50));
  AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),0);
  (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),1);
  *(undefined4 *)(param_1 + 0x54) = 0x1c2;
  *(undefined4 *)(param_1 + 0x58) = 0;
  return;
}

