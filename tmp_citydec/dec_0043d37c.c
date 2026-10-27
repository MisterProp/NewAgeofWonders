
void CityBtn1Click(int param_1)

{
  undefined1 uVar1;
  int *piVar2;
  undefined1 *puVar3;
  char cVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_0043d6ef;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  cVar4 = *(char *)(param_1 + 0x2cc);
  if (cVar4 == '\0') {
    FUN_0043bb14(param_1,1);
    FUN_0043c414(param_1);
    AoWE_TTutorialControl_ShowMessage_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x39,1,1);
  }
  else if (cVar4 == '\x01') {
    puStack_18 = &stack0xfffffffc;
    switch(*(undefined1 *)(param_1 + 0x2c0)) {
    case 1:
      puStack_18 = &stack0xfffffffc;
      cVar4 = City_TCity_CanTrade_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),0,&local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        City_TCity_Trade_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),0);
        FUN_0043bb14(param_1,0);
      }
      break;
    case 2:
      puStack_18 = &stack0xfffffffc;
      cVar4 = City_TCity_CanUpgrade_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),0,&local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        City_TCity_Upgrade_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),0);
        FUN_0043bb14(param_1,0);
      }
      break;
    case 3:
      puStack_18 = &stack0xfffffffc;
      cVar4 = City_TCity_CanFortify_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),0,&local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        City_TCity_Fortify_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),0);
        FUN_0043bb14(param_1,0);
      }
      break;
    case 4:
      piVar2 = *(int **)(*(int *)(param_1 + 0x230) + 0x114);
      puStack_18 = &stack0xfffffffc;
      iVar5 = (**(code **)(*piVar2 + 0x18))
                        (piVar2,*(undefined4 *)(*(int *)(param_1 + 0x230) + 0x120));
      uVar1 = *(undefined1 *)(iVar5 + 8);
      cVar4 = City_TCity_CanMigrate_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),uVar1,&local_8);
      if (cVar4 != '\0') {
        AoWE_TTutorialControl_ShowMessage_23EDC2EF
                  (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x42,1,1);
        City_TCity_Migrate_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),uVar1);
        FUN_0043bb14(param_1,0);
      }
      break;
    case 5:
      puStack_18 = &stack0xfffffffc;
      cVar4 = City_TCity_CanLoot_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0),&local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        City_TCity_Loot_23EDC2EF(*(undefined4 *)(param_1 + 0x2b0));
        FUN_0043bb14(param_1,0);
      }
      break;
    case 6:
      puStack_18 = &stack0xfffffffc;
      cVar4 = (**(code **)(**(int **)(param_1 + 0x2b0) + 0x1ec))
                        (*(int **)(param_1 + 0x2b0),
                         *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),&local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x2b0) + 0x1e8))
                  (*(int **)(param_1 + 0x2b0),
                   *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
        (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
      }
      break;
    case 7:
      puStack_18 = &stack0xfffffffc;
      cVar4 = City_TCity_CanResearch_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x2b0),*(undefined4 *)(param_1 + 0x2c4),0,
                         &local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        City_TCity_Research_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x2b0),*(undefined4 *)(param_1 + 0x2c4),0);
        FUN_0043bb14(param_1,0);
      }
      break;
    case 8:
      puStack_18 = &stack0xfffffffc;
      cVar4 = City_TCity_CanProduce_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x2b0),*(undefined4 *)(param_1 + 0x2c4),0,
                         &local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        AoWE_TTutorialControl_ShowMessage_23EDC2EF
                  (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x3b,1,1);
        City_TCity_Produce_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x2b0),*(undefined4 *)(param_1 + 0x2c4),0);
        FUN_0043bb14(param_1,0);
      }
    }
  }
  else {
    puStack_18 = &stack0xfffffffc;
    if ((cVar4 == '\x02') && (puStack_18 = &stack0xfffffffc, *(int *)(param_1 + 0x2c8) != -1)) {
      piVar2 = *(int **)(*(int *)(param_1 + 0x2b0) + 0x50);
      puStack_18 = &stack0xfffffffc;
      (**(code **)(*piVar2 + 0xb4))(piVar2,*(int *)(param_1 + 0x2c8));
    }
  }
  puVar3 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0043d6f6;
  puStack_1c = (undefined1 *)0x43d6ee;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar3);
  return;
}

