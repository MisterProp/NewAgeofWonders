// /AoWEPACK.dpl : 557ab24c

void City_TCity_Migrate_23EDC2EF(int param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
                    /* 0xab24c  6883  City.TCity.Migrate@23EDC2EF */
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_557ab2e4;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  cVar2 = City_TCity_CanMigrate_23EDC2EF(param_1,param_2,&local_8);
  if (cVar2 != '\0') {
    AoWE_TPlayerList_GetPlayers_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
               (int)*(char *)(param_1 + 0x30));
    cVar2 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar2 == '\0') {
      iVar3 = Engine_TEObject_Create_23EDC2EF(&AoWE__TProductionItem_E3617239,1,0);
      uVar4 = City_CityActionToProductionID_CFDECB0A(4);
      *(undefined4 *)(iVar3 + 8) = uVar4;
      *(int *)(iVar3 + 0xc) = (int)(char)param_2;
      AoWE_TProductionControl_Produce_23EDC2EF(*(undefined4 *)(param_1 + 0x50),iVar3,0);
      System_TObject_Free_23EDC2EF(iVar3);
    }
  }
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_557ab2eb;
  puStack_1c = (undefined1 *)0x557ab2e3;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar1);
  return;
}

