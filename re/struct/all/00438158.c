
void FUN_00438158(int param_1,int param_2,undefined4 *param_3)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_58 [15];
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  puVar3 = local_58;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  cVar1 = FUN_004379f0(param_1);
  if (cVar1 != '\0') {
    local_1c = *(int *)(param_2 + 0x84) + *(int *)(*(int *)(param_1 + 0x44) + 0x84) + 1;
    local_14 = *(int *)(param_2 + 0x7c) + local_1c;
    local_18 = *(int *)(param_2 + 0x88) + *(int *)(*(int *)(param_1 + 0x44) + 0x88);
    local_10 = *(int *)(param_2 + 0x80) + local_18;
    FUN_00437f48(param_1,*(undefined4 *)(param_2 + 0xc),&local_1c,local_58);
  }
  return;
}

