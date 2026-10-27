// /AoWEPACK.dpl : 557ab038

void City_TCity_Loot_23EDC2EF(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
                    /* 0xab038  6886  City.TCity.Loot@23EDC2EF */
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_557ab0c5;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  cVar2 = City_TCity_CanLoot_23EDC2EF(param_1,&local_8);
  if (cVar2 != '\0') {
    AoWE_TPlayerList_GetPlayers_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
               (int)*(char *)(param_1 + 0x30));
    cVar2 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar2 == '\0') {
      iVar3 = Engine_TEObject_Create_23EDC2EF(&AoWE__TProductionItem_E3617239,1,0);
      uVar4 = City_CityActionToProductionID_CFDECB0A(5);
      *(undefined4 *)(iVar3 + 8) = uVar4;
      AoWE_TProductionControl_Produce_23EDC2EF(*(undefined4 *)(param_1 + 0x50),iVar3,0);
      System_TObject_Free_23EDC2EF(iVar3);
    }
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_557ab0cc;
  puStack_18 = (undefined1 *)0x557ab0c4;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

