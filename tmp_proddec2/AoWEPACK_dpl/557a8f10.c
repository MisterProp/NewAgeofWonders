// /AoWEPACK.dpl : 557a8f10

void City_TCityProductionControl_UnitProduced_23EDC2EF(int param_1)

{
  int *piVar1;
  int *piVar2;
  undefined1 *puVar3;
  char cVar4;
  char cVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 *in_FS_OFFSET;
  bool bVar11;
  undefined4 *puVar12;
  undefined4 uVar13;
  undefined4 *puVar14;
  undefined4 uStack_64;
  undefined1 *puStack_60;
  undefined1 *puStack_5c;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined1 local_3c;
  undefined4 local_38;
  undefined1 local_34;
  undefined4 local_30;
  undefined1 local_2c [8];
  int local_24;
  int local_1c;
  int local_18;
  int local_c;
  int local_8;
  
                    /* 0xa8f10  6951  City.TCityProductionControl.UnitProduced@23EDC2EF */
  puStack_5c = &stack0xfffffffc;
  local_30 = 0;
  local_44 = 0;
  local_48 = 0;
  local_4c = 0;
  puStack_60 = &LAB_557a911a;
  uStack_64 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_64;
  piVar1 = *(int **)(param_1 + 0x28);
  bVar11 = false;
  local_8 = param_1;
  local_c = (**(code **)(*piVar1 + 0x54))();
  iVar7 = *(int *)(piVar1[1] + 4);
  uVar13 = 0;
  cVar4 = (**(code **)(*piVar1 + 0x78))();
  iVar6 = (int)cVar4;
  cVar4 = (**(code **)(*piVar1 + 0x74))();
  HSEngine_InitHNWalk_3DC7E9B4(local_2c,(int)cVar4,iVar6,uVar13);
  while ((local_24 < local_c && (!bVar11))) {
    cVar4 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(*(undefined4 *)(piVar1[2] + 0x28),local_24);
    if (((-1 < cVar4) &&
        (((-1 < local_1c && (-1 < local_18)) && (local_1c < *(int *)(iVar7 + 0xc))))) &&
       (local_18 < *(int *)(iVar7 + 0x10))) {
      cVar4 = (**(code **)(*piVar1 + 0x7c))();
      iVar6 = AoWE_TAbstractUnit_Place_23EDC2EF
                        (*(undefined4 *)(*(int *)(local_8 + 0x24) + 0x24),local_1c,local_18,
                         (uint)(uint3)(cVar4 >> 7) << 8,CONCAT31(cVar4 >> 7,(char)piVar1[0xc]),
                         (int)cVar4);
      bVar11 = iVar6 != 0;
    }
    HSEngine_WalkToNextHN_77BE26A7(local_2c);
  }
  if (!bVar11) {
    cVar4 = (**(code **)(*piVar1 + 0x7c))();
    iVar7 = (int)cVar4;
    uVar13 = CONCAT31(cVar4 >> 7,(char)piVar1[0xc]);
    uVar9 = CONCAT31(cVar4 >> 7,1);
    cVar4 = (**(code **)(*piVar1 + 0x78))();
    cVar5 = (**(code **)(*piVar1 + 0x74))();
    AoWE_TAbstractUnit_Place_23EDC2EF
              (*(undefined4 *)(*(int *)(local_8 + 0x24) + 0x24),(int)cVar5,(int)cVar4,uVar9,uVar13,
               iVar7);
  }
  piVar8 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                            (&City__TCityProductionEventLog_56E7A6ED,1,0);
  *(char *)(piVar8 + 4) = (char)piVar1[0xc];
  puVar14 = &local_30;
  piVar2 = *(int **)(*(int *)(local_8 + 0x24) + 0x24);
  iVar7 = *piVar2;
  (**(code **)(iVar7 + 0xf8))(piVar2,&local_44,iVar7,puVar14);
  local_40 = local_44;
  local_3c = 0xb;
  (**(code **)(**(int **)(local_8 + 0x28) + 0x148))(*(int **)(local_8 + 0x28),&local_48);
  local_38 = local_48;
  local_34 = 0xb;
  puVar12 = &local_40;
  System_LoadResString_02614E1B(PTR_AoWE_XProducedAtYRStr_04ECFCC5_558e9050,&local_30);
  AoWE_TranslateRStr_435799AA(local_30,&local_4c);
  SysUtils_Format_D8F48273(local_4c,puVar12,1,puVar14);
  System__LStrAsg_51F89FF7(piVar8 + 6,local_30);
  uVar13 = (**(code **)(*piVar1 + 0x7c))();
  uVar9 = (**(code **)(*piVar1 + 0x78))();
  uVar10 = (**(code **)(*piVar1 + 0x74))();
  EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar8,uVar10,uVar9,uVar13);
  iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                     (int)(char)piVar8[4]);
  EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar7 + 0xd8),piVar8);
  (**(code **)(*piVar8 + 0x2c))();
  puVar3 = puStack_5c;
  *in_FS_OFFSET = uStack_64;
  puStack_5c = &LAB_557a9121;
  puStack_60 = (undefined1 *)0x557a9111;
  System__LStrArrayClr_51F89FF7(&local_4c,3,puVar3);
  puStack_60 = (undefined1 *)0x557a9119;
  System__LStrClr_40929B27(&local_30);
  return;
}

