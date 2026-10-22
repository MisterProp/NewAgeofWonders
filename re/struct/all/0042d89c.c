
void FUN_0042d89c(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_48 [15];
  
  puVar3 = local_48;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  if (*(int *)(*(int *)(iVar2 + 0x54) + 0x34) != 0) {
    iVar2 = AoWE_TSpellControl_GetSpell_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84));
    if (iVar2 != 0) {
      iVar1 = ImageLib_TImageSequenceList_GetImageSequence_23EDC2EF
                        (*(undefined4 *)(iVar2 + 0x2c),10);
      if (iVar1 != 0) {
        ImageLib_TImageSequenceList_Show_23EDC2EF
                  (*(undefined4 *)(iVar2 + 0x2c),10,
                   *(int *)(param_2 + 0x84) + *(int *)(*(int *)(param_2 + 0x10c) + 0x84),local_48,0,
                   *(int *)(*(int *)(param_2 + 0x10c) + 0x88) + *(int *)(param_2 + 0x88));
      }
    }
  }
  return;
}

