
void FUN_0044124c(int param_1,int param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_68 [15];
  undefined1 uStack_2c;
  undefined1 uStack_2b;
  undefined1 uStack_2a;
  undefined4 uStack_28;
  undefined1 uStack_24;
  int iStack_20;
  int iStack_1c;
  
  puVar3 = local_68;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  iVar2 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14) + 0x54))();
  if (iVar2 != 0) {
    iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),0);
    if ((*(int *)(iVar2 + 8) != 1) &&
       (iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF
                          (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x14),0),
       *(int *)(iVar2 + 8) != 2)) {
      return;
    }
    Classes_Rect_9923E97E
              (*(int *)(*(int *)(param_1 + 0x160) + 0x84) + *(int *)(param_2 + 0x84),
               *(int *)(*(int *)(param_1 + 0x160) + 0x88) + *(int *)(param_2 + 0x88),
               *(int *)(*(int *)(param_1 + 0x160) + 0x84) + *(int *)(param_2 + 0x84) +
               *(int *)(param_2 + 0x7c),&iStack_20,
               *(int *)(*(int *)(param_1 + 0x160) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80));
    iVar2 = *(int *)(*(int *)(param_1 + 0x1e8) + 0x44);
    iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                      (*(undefined4 *)(iVar2 + 0x1c),*(undefined4 *)(*(int *)(iVar2 + 0x24) + 0x28))
    ;
    if (iVar2 != 0) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
      piVar1 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(piVar1[9] + 0x28));
      if (piVar1 != (int *)0x0) {
        uStack_2c = 3;
        uStack_2b = 0;
        uStack_2a = 0;
        uStack_28 = 0;
        uStack_24 = 0;
        (**(code **)(*piVar1 + 0x1a4))(piVar1,iStack_20 + -0xc,iStack_1c + -5,&uStack_2c,local_68);
        (**(code **)(*piVar1 + 0x2c))();
      }
    }
  }
  return;
}

