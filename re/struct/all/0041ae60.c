
void FUN_0041ae60(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  undefined4 uVar1;
  
  *(undefined4 *)(param_1 + 0x70) = param_5;
  *(undefined4 *)(param_1 + 0x74) = param_4;
  if (*(int *)(param_1 + 0x80) != -1) {
    *(undefined1 *)(param_1 + 0x94) = 1;
  }
  uVar1 = FUN_00419488(param_1);
  *(undefined4 *)(param_1 + 0x80) = uVar1;
  return;
}

