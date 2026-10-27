// /AoWEPACK.dpl : 557a7da4

void City_TCityProductionControl_CanExecuteProduction_23EDC2EF
               (int *param_1,int param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0xa7da4  6963  City.TCityProductionControl.CanExecuteProduction@23EDC2EF */
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_557a7ef9;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  uVar3 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(param_2 + 8));
  switch(uVar3 & 0x7f) {
  default:
    AoWE_TProductionControl_CanExecuteProduction_23EDC2EF(param_1,param_2,param_3);
    break;
  case 1:
    break;
  case 2:
    cVar2 = (**(code **)(*param_1 + 0x7c))();
    iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       (int)cVar2);
    iVar4 = *(int *)(iVar4 + 0xc4);
    iVar5 = City_TCity_UpgradeCost_23EDC2EF(param_1[10]);
    if (iVar4 < iVar5) {
      System_LoadResString_02614E1B(PTR_AoWE_NotEnoughGoldRStr_8276FCC3_558e91b4,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      System__LStrAsg_51F89FF7(param_3,local_8);
    }
    break;
  case 3:
    cVar2 = (**(code **)(*param_1 + 0x7c))();
    iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       (int)cVar2);
    iVar4 = *(int *)(iVar4 + 0xc4);
    iVar5 = City_TCity_FortifyCost_23EDC2EF(param_1[10]);
    if (iVar4 < iVar5) {
      System_LoadResString_02614E1B(PTR_AoWE_NotEnoughGoldRStr_8276FCC3_558e91b4,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      System__LStrAsg_51F89FF7(param_3,local_8);
    }
    break;
  case 4:
    City_TCity_CanMigrate_23EDC2EF(param_1[10],*(undefined1 *)(param_2 + 0xc),param_3);
    break;
  case 5:
    City_TCity_CanLoot_23EDC2EF(param_1[10],param_3);
  }
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_557a7f00;
  puStack_20 = (undefined1 *)0x557a7ef8;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

