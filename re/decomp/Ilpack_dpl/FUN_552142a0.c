// /Ilpack.dpl : FUN_552142a0

int FUN_552142a0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 local_10;
  
  iVar1 = System__LStrLen_C43AB84F(param_2);
  local_10 = 0;
  if (0 < iVar1) {
    iVar4 = 1;
    do {
      uVar3 = (uint)*(byte *)(param_2 + -1 + iVar4);
      if ((int)uVar3 < *(int *)(param_1 + 0x44)) {
        iVar2 = *(int *)(*(int *)(param_1 + 0x48) + uVar3 * 8);
        if (iVar2 == 0) {
          iVar2 = *(int *)(*(int *)(param_1 + 0x48) + 4 + uVar3 * 8);
          if (iVar2 == -1) {
            iVar2 = 0;
          }
          else {
            iVar2 = *(int *)(*(int *)(param_1 + 0x48) + iVar2 * 8);
          }
        }
      }
      else {
        iVar2 = 0;
      }
      if (iVar2 == 0) {
        iVar2 = *(int *)(param_1 + 0x50) + 5;
      }
      else {
        iVar2 = *(int *)(iVar2 + 0x20) + *(int *)(param_1 + 0x50);
      }
      local_10 = local_10 + iVar2;
      iVar4 = iVar4 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return local_10;
}

