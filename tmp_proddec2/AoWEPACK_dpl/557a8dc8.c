// /AoWEPACK.dpl : 557a8dc8

void City_TCityProductionControl_UnitResearched_23EDC2EF(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar8;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined1 local_14;
  undefined4 local_10;
  undefined1 local_c;
  undefined4 local_8;
  
                    /* 0xa8dc8  6952  City.TCityProductionControl.UnitResearched@23EDC2EF */
  puStack_34 = &stack0xfffffffc;
  local_8 = 0;
  local_1c = 0;
  local_20 = 0;
  local_24 = 0;
  puStack_38 = &LAB_557a8f00;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  AoWE_TProductionControl_UnitResearched_23EDC2EF(param_1);
  piVar1 = *(int **)(param_1 + 0x28);
  piVar3 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                            (&City__TCityProductionEventLog_56E7A6ED,1,0);
  *(char *)(piVar3 + 4) = (char)piVar1[0xc];
  puVar8 = &local_8;
  iVar4 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1c),
                     *(undefined4 *)(*(int *)(param_1 + 0x24) + 0x28));
  uVar5 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x5c),
                     *(undefined4 *)(iVar4 + 8));
  AoWE_TUnitResource_GetUnitName_23EDC2EF(uVar5,&local_1c);
  local_18 = local_1c;
  local_14 = 0xb;
  iVar4 = **(int **)(param_1 + 0x28);
  (**(code **)(iVar4 + 0x148))(*(int **)(param_1 + 0x28),&local_20,iVar4,puVar8);
  local_10 = local_20;
  local_c = 0xb;
  System_LoadResString_02614E1B(PTR_AoWE_XInstalledAtYRStr_04ECFCC5_558e8fa4,&local_8);
  AoWE_TranslateRStr_435799AA(local_8,&local_24);
  SysUtils_Format_D8F48273(local_24,&local_18,1,puVar8);
  System__LStrAsg_51F89FF7(piVar3 + 6,local_8);
  uVar5 = (**(code **)(*piVar1 + 0x7c))();
  uVar6 = (**(code **)(*piVar1 + 0x78))();
  uVar7 = (**(code **)(*piVar1 + 0x74))();
  EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar3,uVar7,uVar6,uVar5);
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                     (int)(char)piVar3[4]);
  EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar4 + 0xd8),piVar3);
  (**(code **)(*piVar3 + 0x2c))();
  puVar2 = puStack_34;
  *in_FS_OFFSET = uStack_3c;
  puStack_34 = &LAB_557a8f07;
  puStack_38 = (undefined1 *)0x557a8ef7;
  System__LStrArrayClr_51F89FF7(&local_24,3,puVar2);
  puStack_38 = (undefined1 *)0x557a8eff;
  System__LStrClr_40929B27(&local_8);
  return;
}

