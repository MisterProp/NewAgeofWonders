
void FUN_0043e02c(int param_1,int param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_5c [15];
  undefined1 local_20 [16];
  
  puVar3 = local_5c;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  Classes_Rect_9923E97E
            (*(int *)(*(int *)(param_1 + 0x158) + 0x84) + *(int *)(param_2 + 0x84),
             *(int *)(*(int *)(param_1 + 0x158) + 0x88) + *(int *)(param_2 + 0x88),
             *(int *)(*(int *)(param_1 + 0x158) + 0x84) + *(int *)(param_2 + 0x84) +
             *(int *)(param_2 + 0x7c),local_20,
             *(int *)(*(int *)(param_1 + 0x158) + 0x88) + *(int *)(param_2 + 0x88) +
             *(int *)(param_2 + 0x80));
  AoWE_TRaceResourceList_GetRaceResource_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x54),
             (int)*(char *)(*(int *)(param_1 + 0x2b0) + 0x45));
  piVar1 = (int *)AoWE_TRaceResource_GetFace_23EDC2EF();
  (**(code **)(*piVar1 + 0x88))(piVar1,local_20,local_5c);
  return;
}

