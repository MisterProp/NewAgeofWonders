
void FUN_004352ec(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x150) != 0) {
    System__Assert_E17CE8DC("Assertion failure","D:\\AoWDev\\FastCombatWin.pas",0x9f);
  }
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
  *(int **)(param_1 + 0x150) = param_2;
  (**(code **)(*param_2 + 0x28))();
  *(undefined4 *)(param_1 + 0x148) = param_4;
  *(undefined4 *)(param_1 + 0x14c) = param_5;
  iVar1 = param_2[6];
  *(int *)(param_1 + 0x130) = iVar1;
  FUN_004353a8(param_1,iVar1);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  return;
}

