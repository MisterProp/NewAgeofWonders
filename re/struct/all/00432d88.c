
void FUN_00432d88(int param_1)

{
  int iVar1;
  undefined4 local_4;
  
  iVar1 = *(int *)(param_1 + 0xa8);
  if (iVar1 == 0) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x78) + 0x114);
    Engine_SortStringsAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x7c) + 0x114),&local_4,0);
    return;
  }
  if (iVar1 == 1) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x78) + 0x114);
    Engine_SortStringsDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x7c) + 0x114),&local_4,0);
    return;
  }
  if (iVar1 == 2) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x7c) + 0x114);
    Engine_SortStringsAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x78) + 0x114),&local_4,0);
    return;
  }
  if (iVar1 == 3) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x7c) + 0x114);
    Engine_SortStringsDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x78) + 0x114),&local_4,0);
  }
  return;
}

