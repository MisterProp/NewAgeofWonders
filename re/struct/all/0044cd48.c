
void FUN_0044cd48(int param_1,int param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_4c [15];
  
  puVar4 = local_4c;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  iVar3 = *(int *)(param_1 + 0xac);
  if ((iVar3 != 0) &&
     (*(int *)(param_2 + 0xc) + *(int *)(param_1 + 0xa0) < *(int *)(*(int *)(iVar3 + 0x18) + 8))) {
    uVar1 = Engine_TIntegerList_Get_23EDC2EF
                      (*(undefined4 *)(iVar3 + 0x18),
                       *(int *)(param_2 + 0xc) + *(int *)(param_1 + 0xa0));
    piVar2 = (int *)AoWE_TItemControl_FindItem_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xf4),uVar1);
    if (piVar2 != (int *)0x0) {
      (**(code **)(*piVar2 + 0xa8))
                (piVar2,*(int *)(*(int *)(param_1 + 0x44) + 0x84) + *(int *)(param_2 + 0x84),
                 *(int *)(*(int *)(param_1 + 0x44) + 0x88) + *(int *)(param_2 + 0x88),local_4c);
    }
  }
  return;
}

