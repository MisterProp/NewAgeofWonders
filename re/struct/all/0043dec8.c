
int FUN_0043dec8(int param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_68 [15];
  undefined1 uStack_2c;
  undefined1 uStack_2b;
  undefined1 uStack_2a;
  undefined4 uStack_28;
  undefined1 uStack_24;
  int iStack_20;
  int iStack_1c;
  
  puVar4 = local_68;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  iVar1 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x14) + 0x54))();
  iVar3 = 0;
  if (iVar1 != 0) {
    iVar1 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x14),0);
    iVar3 = *(int *)(iVar1 + 8) + -3;
    if (*(int *)(iVar1 + 8) - 1U < 2) {
      Classes_Rect_9923E97E
                (*(int *)(*(int *)(param_1 + 0x1a4) + 0x84) + *(int *)(param_2 + 0x84),
                 *(int *)(*(int *)(param_1 + 0x1a4) + 0x88) + *(int *)(param_2 + 0x88),
                 *(int *)(*(int *)(param_1 + 0x1a4) + 0x84) + *(int *)(param_2 + 0x84) +
                 *(int *)(param_2 + 0x7c),&iStack_20,
                 *(int *)(*(int *)(param_1 + 0x1a4) + 0x88) + *(int *)(param_2 + 0x88) +
                 *(int *)(param_2 + 0x80));
      piVar2 = *(int **)(*(int *)(param_1 + 0x2b0) + 0x50);
      iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar2[5],0);
      piVar2 = (int *)(**(code **)(*piVar2 + 0x9c))(piVar2,*(undefined4 *)(iVar3 + 0xc));
      iVar3 = 0;
      if (piVar2 != (int *)0x0) {
        uStack_2c = 3;
        uStack_2b = 0;
        uStack_2a = 0;
        uStack_28 = 0;
        uStack_24 = 0;
        iVar3 = *(int *)(*(int *)(param_1 + 0x2b0) + 0x50);
        iVar1 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(iVar3 + 0x14),0);
        iVar3 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                          (*(undefined4 *)(iVar3 + 0x1c),*(undefined4 *)(iVar1 + 0xc));
        if (*(char *)(iVar3 + 0xc) == '\0') {
          uStack_2c = 5;
        }
        (**(code **)(*piVar2 + 0x1a4))(piVar2,iStack_20 + -8,iStack_1c + -5,&uStack_2c,local_68);
        iVar3 = (**(code **)(*piVar2 + 0x2c))();
      }
    }
  }
  return iVar3;
}

