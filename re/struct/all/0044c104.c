
void FUN_0044c104(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  if (*(int *)(param_1 + 0xa8) != 0) {
    System__Assert_E17CE8DC("Assertion failure","D:\\AoWDev\\AoWArenaDlg.pas",0x5e);
  }
  FUN_0044c01c(param_1);
  *(undefined4 *)(param_1 + 0xac) = param_4;
  *(undefined4 *)(param_1 + 0xb0) = param_5;
  *(int **)(param_1 + 0xa8) = param_2;
  (**(code **)(*param_2 + 0x28))();
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),1);
  return;
}

