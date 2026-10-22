
void FUN_00447b38(int param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_58 [15];
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  puVar2 = local_58;
  for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  local_1c = *(int *)(param_2 + 0x84) + *(int *)(*(int *)(param_1 + 0x44) + 0x84) + 1;
  local_14 = *(int *)(param_2 + 0x7c) + local_1c;
  local_18 = *(int *)(param_2 + 0x88) + *(int *)(*(int *)(param_1 + 0x44) + 0x88);
  local_10 = *(int *)(param_2 + 0x80) + local_18;
  FUN_00447940(param_1,*(undefined4 *)(param_2 + 0xc),&local_1c,local_58);
  return;
}

