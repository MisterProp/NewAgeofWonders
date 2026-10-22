
void FUN_0041f140(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  LPCSTR lpPathName;
  
  *(undefined4 *)(param_1 + 0x140) = param_4;
  *(undefined4 *)(param_1 + 0x144) = param_5;
  *(int **)(param_1 + 0x13c) = param_2;
  (**(code **)(*param_2 + 0x28))();
  *(undefined4 *)(param_1 + 0x148) = 1;
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  if (param_2[7] != 0) {
    lpPathName = (LPCSTR)System__LStrToPChar_51F89FF7(param_2[7]);
    SetCurrentDirectoryA(lpPathName);
  }
  AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xdc),param_2[8]);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x54),param_2[10]);
  FUN_0041f430(param_1);
  return;
}

