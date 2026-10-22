
void FUN_0041e768(int param_1,undefined4 param_2,undefined4 *param_3)

{
  undefined1 uVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_58 [15];
  undefined1 local_1c [16];
  
  puVar4 = local_58;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  iVar3 = *(int *)(param_1 + 0x120);
  Classes_Rect_9923E97E
            (*(int *)(*(int *)(param_1 + 0x8c) + 0x84) + *(int *)(iVar3 + 0x84),
             *(int *)(*(int *)(param_1 + 0x8c) + 0x88) + *(int *)(iVar3 + 0x88),
             *(int *)(*(int *)(param_1 + 0x8c) + 0x84) + *(int *)(*(int *)(param_1 + 0x120) + 0x84)
             + *(int *)(iVar3 + 0x7c),local_1c,
             *(int *)(*(int *)(param_1 + 0x8c) + 0x88) + *(int *)(*(int *)(param_1 + 0x120) + 0x88)
             + *(int *)(iVar3 + 0x80));
  iVar3 = FUN_0041bbb8(param_1);
  if (0 < *(int *)(*(int *)(iVar3 + 0x18) + 8)) {
    iVar3 = FUN_0041bbb8(param_1);
    uVar1 = Engine_TByteList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x18),0);
    AoWE_TRaceResourceList_GetRaceResource_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x54),uVar1);
    piVar2 = (int *)AoWE_TRaceResource_GetFace_23EDC2EF();
    (**(code **)(*piVar2 + 0x88))(piVar2,local_1c,local_58);
  }
  return;
}

