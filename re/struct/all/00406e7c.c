
void FUN_00406e7c(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_48 [15];
  
  puVar3 = local_48;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  piVar1 = (int *)AoWE_TItemControl_FindItem_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xf4),
                             DAT_0045b060);
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0xa8))
              (piVar1,*(undefined4 *)(*(int *)(param_1 + 0x6c) + 0x84),
               *(undefined4 *)(*(int *)(param_1 + 0x6c) + 0x88),local_48);
  }
  return;
}

