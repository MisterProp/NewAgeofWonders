
void FUN_0042d988(int param_1,undefined4 param_2,char param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  if ((param_3 == '\x01') && (-1 < *(int *)(*(int *)(param_1 + 0x110) + 0x120))) {
    iVar1 = *(int *)(param_1 + 0x110);
    iVar2 = (**(code **)(**(int **)(iVar1 + 0x114) + 0x14))();
    if (*(int *)(iVar1 + 0x120) < iVar2) {
      piVar3 = *(int **)(*(int *)(param_1 + 0x110) + 0x114);
      piVar3 = (int *)(**(code **)(*piVar3 + 0x18))
                                (piVar3,*(undefined4 *)(*(int *)(param_1 + 0x110) + 0x120));
      (**(code **)(*piVar3 + 0x58))();
    }
  }
  return;
}

