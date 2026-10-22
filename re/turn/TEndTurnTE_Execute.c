
void AoWE_TEndTurnTE_Execute_23EDC2EF(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined *puVar4;
  undefined4 uVar5;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x473c4  2974  AoWE.TEndTurnTE.Execute@23EDC2EF */
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_20 = &LAB_55747512;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  NetworkE_TTokenEvent_Execute_23EDC2EF(param_1);
  if (AoWE_AoWHSMap_6B957A58[0x5d] == *(int *)(param_1 + 0x14)) {
    if ((((*(char *)(param_1 + 0x18) == '\0') &&
         (iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (AoWE_AoWHSMap_6B957A58[0x50],(int)*(char *)(param_1 + 0x10)),
         *(char *)(iVar2 + 0x2c) == '\0')) &&
        ((**(code **)(*(int *)AoWE_AoWHSMap_6B957A58[0x91] + 0x84))
                   ((int *)AoWE_AoWHSMap_6B957A58[0x91],*(undefined1 *)(param_1 + 0x10)),
        *(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x239) == '\x02')) &&
       (iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (AoWE_AoWHSMap_6B957A58[0x50],(int)*(char *)(param_1 + 0x10)),
       *(char *)(iVar2 + 0xa5) != -1)) {
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (AoWE_AoWHSMap_6B957A58[0x50],(int)*(char *)(param_1 + 0x10));
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(&AoWE_RaceNamesPluralRStr_B11890CE + *(char *)(iVar2 + 0xa5) * 4),
                 &local_c);
      puVar4 = &DAT_55747528;
      uVar5 = local_c;
      System_LoadResString_02614E1B(&AoWE_FinishedTheirTurnRStr_8276FCC3,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      uVar3 = local_10;
      System__LStrCatN_51F89FF7(&local_8,3);
      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x110))
                (AoWE_AoWHSMap_6B957A58,local_8,*AoWE_AoWHSMap_6B957A58,uVar3,puVar4,uVar5);
    }
    if (*(char *)(param_1 + 0x18) == '\x01') {
      uVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (AoWE_AoWHSMap_6B957A58[0x50],(int)*(char *)(param_1 + 0x10));
      AoWE_TPlayer_SetEndTurn_23EDC2EF(uVar3,0);
    }
  }
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_55747519;
  puStack_20 = (undefined1 *)0x55747511;
  System__LStrArrayClr_51F89FF7(&local_14,4,puVar1);
  return;
}

