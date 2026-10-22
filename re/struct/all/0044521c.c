
void FUN_0044521c(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_58 [15];
  int iStack_1c;
  int iStack_18;
  int iStack_14;
  int iStack_10;
  
  puVar3 = local_58;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  if (*(int *)(param_1 + 0xa8) != 0) {
    piVar1 = (int *)AoWE_TUnitControl_FindUnit_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xfc),
                               *(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x18));
    if (piVar1 != (int *)0x0) {
      iVar2 = *(int *)(param_1 + 0x90);
      iStack_1c = *(int *)(iVar2 + 0x84) + *(int *)(*(int *)(param_1 + 0x60) + 0x84);
      iStack_14 = *(int *)(iVar2 + 0x7c) + iStack_1c;
      iStack_18 = *(int *)(iVar2 + 0x88) + *(int *)(*(int *)(param_1 + 0x60) + 0x88);
      iStack_10 = *(int *)(iVar2 + 0x80) + iStack_18;
      iVar2 = (**(code **)(*piVar1 + 0xf4))();
      if (iVar2 != 0) {
        piVar1 = (int *)(**(code **)(*piVar1 + 0xf4))();
        (**(code **)(*piVar1 + 0x88))(piVar1,&iStack_1c,local_58);
      }
    }
  }
  return;
}

