

void PrdPlace_TProductionPlaceControl_UnitProduced_23EDC2EF(int param_1)



{

  int *piVar1;

  int *piVar2;

  undefined1 *puVar3;

  char cVar4;

  char cVar5;

  int iVar6;

  int *piVar7;

  undefined4 uVar8;

  undefined4 uVar9;

  undefined4 uVar10;

  undefined4 *in_FS_OFFSET;

  undefined4 *puVar11;

  undefined4 *puVar12;

  undefined4 uStack_40;

  undefined1 *puStack_3c;

  undefined1 *puStack_38;

  undefined4 local_28;

  undefined4 local_24;

  undefined4 local_20;

  undefined4 local_1c;

  undefined1 local_18;

  undefined4 local_14;

  undefined1 local_10;

  undefined4 local_c;

  undefined1 local_5;

  

                    /* 0xbdcb0  7488  PrdPlace.TProductionPlaceControl.UnitProduced@23EDC2EF */

  puStack_38 = &stack0xfffffffc;

  local_c = 0;

  local_20 = 0;

  local_24 = 0;

  local_28 = 0;

  puStack_3c = &LAB_557bde82;

  uStack_40 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_40;

  piVar1 = *(int **)(param_1 + 0x28);

  cVar4 = (**(code **)(*piVar1 + 0x7c))();

  iVar6 = (int)cVar4;

  uVar8 = CONCAT31(cVar4 >> 7,(char)piVar1[0xc]);

  uVar9 = CONCAT31(cVar4 >> 7,1);

  cVar4 = (**(code **)(*piVar1 + 0x78))();

  cVar5 = (**(code **)(*piVar1 + 0x74))();

  iVar6 = AoWE_TAbstractUnit_Place_23EDC2EF

                    (*(undefined4 *)(*(int *)(param_1 + 0x24) + 0x24),(int)cVar5,(int)cVar4,uVar9,

                     uVar8,iVar6);

  local_5 = iVar6 != 0;

  if ((bool)local_5) {

    cVar4 = (**(code **)(**(int **)(param_1 + 0x28) + 0x230))();

    if (cVar4 != '\b') {

      piVar2 = *(int **)(*(int *)(param_1 + 0x24) + 0x24);

      cVar4 = (**(code **)(*piVar2 + 0x94))(piVar2,0x9b);

      if (cVar4 != '\0') {

        iVar6 = AoWE_TAbilityOwner_GetAbilityData_23EDC2EF

                          (*(undefined4 *)(*(int *)(param_1 + 0x24) + 0x24),0x9b);

        piVar2 = *(int **)(iVar6 + 0x10);

        cVar4 = (**(code **)(**(int **)(*(int *)(param_1 + 0x24) + 0x24) + 0xa0))();

        (**(code **)(*piVar2 + 0x54))(piVar2,(cVar4 + 1) * 2);

        (**(code **)(*piVar2 + 0x58))(piVar2,*(undefined1 *)(*(int *)(param_1 + 0x28) + 0x30));

        cVar4 = (**(code **)(**(int **)(*(int *)(param_1 + 0x24) + 0x24) + 0xa0))();

        *(char *)(piVar2 + 5) = cVar4 + '\x03';

      }

    }

  }

  piVar7 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF

                            (&PrdPlace__TProductionPlaceEventLog_2A16DC7C,1,0);

  *(char *)(piVar7 + 4) = (char)piVar1[0xc];

  puVar12 = &local_c;

  piVar2 = *(int **)(*(int *)(param_1 + 0x24) + 0x24);

  iVar6 = *piVar2;

  (**(code **)(iVar6 + 0xf8))(piVar2,&local_20,iVar6,puVar12);

  local_1c = local_20;

  local_18 = 0xb;

  (**(code **)(**(int **)(param_1 + 0x28) + 0x148))(*(int **)(param_1 + 0x28),&local_24);

  local_14 = local_24;

  local_10 = 0xb;

  puVar11 = &local_1c;

  System_LoadResString_02614E1B(PTR_AoWE_XProducedAtYRStr_04ECFCC5_558e9050,&local_c);

  AoWE_TranslateRStr_435799AA(local_c,&local_28);

  SysUtils_Format_D8F48273(local_28,puVar11,1,puVar12);

  System__LStrAsg_51F89FF7(piVar7 + 6,local_c);

  uVar8 = (**(code **)(*piVar1 + 0x7c))();

  uVar9 = (**(code **)(*piVar1 + 0x78))();

  uVar10 = (**(code **)(*piVar1 + 0x74))();

  EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar7,uVar10,uVar9,uVar8);

  iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                     (int)(char)piVar7[4]);

  EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar6 + 0xd8),piVar7);

  (**(code **)(*piVar7 + 0x2c))();

  puVar3 = puStack_38;

  *in_FS_OFFSET = uStack_40;

  puStack_38 = &LAB_557bde89;

  puStack_3c = (undefined1 *)0x557bde79;

  System__LStrArrayClr_51F89FF7(&local_28,3,puVar3);

  puStack_3c = (undefined1 *)0x557bde81;

  System__LStrClr_40929B27(&local_c);

  return;

}




