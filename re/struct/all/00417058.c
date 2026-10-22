
void FUN_00417058(int param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_5c [15];
  undefined1 local_20 [16];
  
  puVar4 = local_5c;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  iVar3 = *(int *)(param_1 + 0x2d8);
  if (1 < *(int *)(param_1 + 0x2dc)) {
    Classes_Rect_9923E97E
              (*(int *)(*(int *)(param_1 + 0x4c) + 0x84) + *(int *)(param_2 + 0x84) + 2,
               *(int *)(*(int *)(param_1 + 0x4c) + 0x88) + *(int *)(param_2 + 0x88),
               *(int *)(*(int *)(param_1 + 0x4c) + 0x84) + *(int *)(param_2 + 0x84) +
               *(int *)(param_2 + 0x7c) + -2,local_20,
               *(int *)(*(int *)(param_1 + 0x4c) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80) + -2);
    iVar1 = (**(code **)(**(int **)(iVar3 + 0x14) + 0xf4))();
    if (iVar1 != 0) {
      piVar2 = (int *)(**(code **)(**(int **)(iVar3 + 0x14) + 0xf4))();
      (**(code **)(*piVar2 + 0x88))(piVar2,local_20,local_5c);
    }
  }
  return;
}

