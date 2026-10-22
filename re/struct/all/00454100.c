
int FUN_00454100(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = -1;
  if ((-1 < *(int *)(param_1 + 0x84)) && (*(int *)(param_1 + 0xa4) != 0)) {
    iVar2 = 0;
    do {
      if (-1 < *(int *)(param_1 + 0x84 + iVar2 * 4)) {
        iVar1 = (**(code **)(**(int **)(param_1 + 0xa4) + 0x54))();
        if (iVar2 < iVar1) {
          iVar1 = AoWE_TUnitList_GetUnit_23EDC2EF
                            (*(undefined4 *)(param_1 + 0xa4),
                             *(undefined4 *)(param_1 + 0x84 + iVar2 * 4));
          if (param_2 == iVar1) {
            iVar3 = iVar2;
          }
        }
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 != 8);
  }
  return iVar3;
}

