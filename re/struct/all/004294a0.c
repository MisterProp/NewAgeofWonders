
void FUN_004294a0(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_58 [15];
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  puVar2 = local_58;
  for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    local_14 = *(int *)(*(int *)(param_1 + 0x44) + 0x84) + *(int *)(*(int *)(param_1 + 0x4c) + 0x84)
    ;
    local_1c = local_14 + 0x2d;
    local_14 = local_14 + 0x94;
    local_10 = *(int *)(*(int *)(param_1 + 0x44) + 0x88) + *(int *)(*(int *)(param_1 + 0x4c) + 0x88)
    ;
    local_18 = local_10 + 0x23;
    local_10 = local_10 + 0xa3;
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    AoWE_TAbstractUnit_ShowFace_23EDC2EF(*(undefined4 *)(iVar1 + 0xd4),&local_1c,local_58);
  }
  return;
}

