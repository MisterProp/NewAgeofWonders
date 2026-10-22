
void FUN_004444b4(int param_1,undefined4 param_2,int *param_3,undefined4 param_4)

{
  int iVar1;
  int *piVar2;
  
  if (*(int *)(param_1 + 0x1b0) != 0) {
    iVar1 = (**(code **)(**(int **)(*(int *)(param_1 + 0x1b0) + 0xc) + 0x54))();
    if (*(int *)(*(int *)(param_1 + 0x178) + 0x15c) < iVar1) {
      piVar2 = *(int **)(*(int *)(param_1 + 0x1b0) + 0xc);
      iVar1 = *piVar2;
      piVar2 = (int *)(**(code **)(iVar1 + 0x8c))
                                (piVar2,*(undefined4 *)(*(int *)(param_1 + 0x178) + 0x15c),iVar1,
                                 param_4);
      (**(code **)(*piVar2 + 0xa8))(piVar2,*param_3 + 10,param_3[1] + 10);
    }
  }
  return;
}

