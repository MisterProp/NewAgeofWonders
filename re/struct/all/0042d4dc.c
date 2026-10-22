
void FUN_0042d4dc(int param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar1 = *(int *)(*(int *)(param_1 + 0x70) + 0x120);
  if (-1 < iVar1) {
    iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x114) + 0x14))();
    if (iVar1 < iVar2) {
      piVar3 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
      piVar3 = (int *)(**(code **)(*piVar3 + 0x18))
                                (piVar3,*(undefined4 *)(*(int *)(param_1 + 0x70) + 0x120));
      (**(code **)(*piVar3 + 0x6c))();
    }
  }
  return;
}

