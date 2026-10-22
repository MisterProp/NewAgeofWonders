
void FUN_00415fbc(int param_1)

{
  int iVar1;
  undefined4 local_4;
  
  iVar1 = *(int *)(param_1 + 0x2e8);
  if (iVar1 == 0) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x238) + 0x114);
    Engine_SortStringsAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x44) + 0x114),&local_4,0);
    return;
  }
  if (iVar1 == 1) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x238) + 0x114);
    Engine_SortStringsDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x44) + 0x114),&local_4,0);
    return;
  }
  if (iVar1 == 2) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x44) + 0x114);
    Engine_SortStringsNumericalAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x238) + 0x114),&local_4,0);
    return;
  }
  if (iVar1 == 3) {
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0x44) + 0x114);
    Engine_SortStringsNumericalDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0x238) + 0x114),&local_4,0);
  }
  return;
}

