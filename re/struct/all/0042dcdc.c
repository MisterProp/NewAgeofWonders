
void FUN_0042dcdc(int param_1,undefined4 param_2,char param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  if (param_3 == '\x01') {
    iVar1 = *(int *)(*(int *)(param_1 + 0xa0) + 0x120);
    if (-1 < iVar1) {
      iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0xa0) + 0x114) + 0x14))();
      if (iVar1 < iVar2) {
        piVar3 = *(int **)(*(int *)(param_1 + 0xa0) + 0x114);
        piVar3 = (int *)(**(code **)(*piVar3 + 0x18))
                                  (piVar3,*(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x120));
        (**(code **)(*piVar3 + 0x88))();
      }
    }
  }
  return;
}

