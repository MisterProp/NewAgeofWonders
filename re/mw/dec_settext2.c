void FUN_55214320(int param_1,int param_2,undefined4 param_3,undefined4 param_4,int param_5)
{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int local_14;
  int local_10;
  int local_8;
  local_14 = System__LStrLen_C43AB84F(param_5);
  if (0 < local_14) {
    local_10 = 1;
    local_8 = param_2;
    do {
      uVar1 = (uint)*(byte *)(param_5 + -1 + local_10);
      if ((int)uVar1 < *(int *)(param_1 + 0x44)) {
        piVar3 = *(int **)(*(int *)(param_1 + 0x48) + uVar1 * 8);
        if (piVar3 == (int *)0x0) {
          iVar2 = *(int *)(*(int *)(param_1 + 0x48) + 4 + uVar1 * 8);
          if (iVar2 == -1) {
            piVar3 = (int *)0x0;
          }
          else {
            piVar3 = *(int **)(*(int *)(param_1 + 0x48) + iVar2 * 8);
          }
        }
      }
      else {
        piVar3 = (int *)0x0;
      }
      if (piVar3 == (int *)0x0) {
        iVar2 = *(int *)(param_1 + 0x50) + 5;
      }
      else {
        (**(code **)(*piVar3 + 0x84))(piVar3,local_8,param_3,param_4);
        iVar2 = piVar3[8] + *(int *)(param_1 + 0x50);
      }
      local_8 = local_8 + iVar2;
      local_10 = local_10 + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  return;
}
