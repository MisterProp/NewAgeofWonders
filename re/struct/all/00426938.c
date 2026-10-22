
void FUN_00426938(int param_1,int param_2,undefined4 *param_3)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_48 [15];
  
  puVar4 = local_48;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  if ((*(char *)(param_1 + 0x286) != -1) &&
     (*(int *)(param_2 + 0xc) + -0x14 < *(int *)(*(int *)(param_1 + 0x28c) + 8))) {
    uVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    iVar3 = AoWE_TPlayer_GetFirstDaySeen_23EDC2EF(uVar2,*(undefined1 *)(param_1 + 0x286));
    if (iVar3 != -1) {
      puVar4 = local_48;
      uVar2 = Engine_TIntegerList_Get_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x28c),*(int *)(param_2 + 0xc) + -0x14);
      uVar2 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),uVar2);
      bVar1 = AoWE_TRace_GetPlayerRelation_23EDC2EF(uVar2,*(undefined1 *)(param_1 + 0x286));
      uVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x98),bVar1 + 0xb);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                (uVar2,*(undefined4 *)(param_2 + 0x84),*(int *)(param_2 + 0x88) + 0xf,puVar4);
    }
  }
  return;
}

