
void FUN_0042cba4(int param_1)

{
  undefined4 local_8;
  undefined4 local_4;
  
  switch(*(undefined4 *)(param_1 + 0x144)) {
  case 0:
    local_8 = *(undefined4 *)(*(int *)(param_1 + 0xb0) + 0x114);
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x114);
    Engine_SortStringsAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x114),&local_8,1);
    return;
  case 1:
    local_8 = *(undefined4 *)(*(int *)(param_1 + 0xb0) + 0x114);
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x114);
    Engine_SortStringsDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x114),&local_8,1);
    return;
  case 2:
    local_8 = *(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x114);
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x114);
    Engine_SortStringsAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0xb0) + 0x114),&local_8,1);
    return;
  case 3:
    local_8 = *(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x114);
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x114);
    Engine_SortStringsDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0xb0) + 0x114),&local_8,1);
    return;
  case 4:
    local_8 = *(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x114);
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0xb0) + 0x114);
    Engine_SortStringsNumericalAscending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x114),&local_8,1);
    return;
  case 5:
    local_8 = *(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x114);
    local_4 = *(undefined4 *)(*(int *)(param_1 + 0xb0) + 0x114);
    Engine_SortStringsNumericalDescending_BF0AA92C
              (*(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x114),&local_8,1);
  }
  return;
}

