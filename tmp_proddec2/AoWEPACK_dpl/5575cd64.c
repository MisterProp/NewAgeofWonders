// /AoWEPACK.dpl : 5575cd64

void AoWE_TProductionControl_ExecuteCancelProduction_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
                    /* 0x5cd64  2364  AoWE.TProductionControl.ExecuteCancelProduction@23EDC2EF */
  if ((char)param_1[8] == '\x02') {
    if (*(int *)(param_1[9] + 0x20) != 0) {
      AoWE_TProductionControl_UnregisterGroupProduction_23EDC2EF(param_1);
    }
    cVar1 = (**(code **)(*param_1 + 0x7c))();
    if (-1 < cVar1) {
      iVar5 = (int)cVar1;
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);
      iVar2 = *(int *)(iVar2 + 0xc4);
      iVar3 = (**(code **)(*param_1 + 0x4c))();
      uVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);
      AoWE_TPlayer_SetGems_23EDC2EF(uVar4,iVar2 + iVar3);
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);
      iVar2 = *(int *)(*(int *)(iVar2 + 0x54) + 0x24);
      iVar3 = (**(code **)(*param_1 + 0x50))();
      iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);
      AoWE_TPlayerMagicControl_SetMana_23EDC2EF(*(undefined4 *)(iVar5 + 0x54),iVar2 + iVar3);
    }
    (**(code **)(*(int *)param_1[9] + 0x30))();
    *(undefined1 *)(param_1 + 8) = 1;
    (**(code **)(*param_1 + 0x98))();
  }
  return;
}

