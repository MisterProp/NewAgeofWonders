
void FUN_0043dc0c(int param_1,int param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_5c [15];
  undefined1 auStack_20 [16];
  
  puVar3 = local_5c;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  piVar1 = *(int **)(*(int *)(param_1 + 0x2b0) + 0x50);
  piVar1 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(param_1 + 0x2c4));
  if (piVar1 != (int *)0x0) {
    Classes_Rect_9923E97E
              (*(int *)(*(int *)(param_1 + 0x1bc) + 0x84) + *(int *)(param_2 + 0x84) + 2,
               *(int *)(*(int *)(param_1 + 0x1bc) + 0x88) + *(int *)(param_2 + 0x88) + 2,
               *(int *)(*(int *)(param_1 + 0x1bc) + 0x84) + *(int *)(param_2 + 0x84) +
               *(int *)(param_2 + 0x7c) + -4,auStack_20,
               *(int *)(*(int *)(param_1 + 0x1bc) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80) + -2);
    AoWE_TAbstractUnit_ShowFace_23EDC2EF(piVar1,auStack_20,local_5c);
    (**(code **)(*piVar1 + 0x2c))();
  }
  return;
}

