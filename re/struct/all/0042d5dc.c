
void FUN_0042d5dc(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x114) + 0x14))();
  if (iVar1 <= param_2) {
    param_2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x114) + 0x14))();
    param_2 = param_2 + -1;
  }
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x70),param_2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x80),param_2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x78),param_2);
  (**(code **)(**(int **)(param_1 + 0xc4) + 0x6c))(*(int **)(param_1 + 0xc4),param_2 != -1);
  return;
}

