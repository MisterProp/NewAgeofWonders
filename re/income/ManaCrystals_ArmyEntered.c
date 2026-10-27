

void ManaCrystals_TManaCrystals_ArmyEntered_23EDC2EF(int *param_1,int param_2)



{

  undefined1 *puVar1;

  char cVar2;

  undefined4 uVar3;

  undefined4 uVar4;

  undefined4 uVar5;

  int iVar6;

  int *piVar7;

  undefined4 *in_FS_OFFSET;

  undefined4 uVar8;

  undefined4 *puVar9;

  undefined4 *puVar10;

  undefined4 uStack_30;

  undefined1 *puStack_2c;

  undefined1 *puStack_28;

  undefined4 local_18;

  undefined4 local_14;

  undefined4 local_10;

  undefined4 local_c;

  undefined4 local_8;

  

                    /* 0xb4d20  7190  ManaCrystals.TManaCrystals.ArmyEntered@23EDC2EF */

  puStack_28 = &stack0xfffffffc;

  local_8 = 0;

  local_c = 0;

  local_10 = 0;

  local_14 = 0;

  local_18 = 0;

  puStack_2c = &LAB_557b4ea2;

  uStack_30 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_30;

  puVar1 = &stack0xfffffffc;

  if (*(char *)(*(int *)(param_2 + 0x1c) + 0x12) == '\0') goto LAB_557b4e7f;

  puVar1 = &stack0xfffffffc;

  if (*(char *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0xa5) ==

      *(char *)(*(int *)(param_2 + 0x1c) + 0x12)) {

LAB_557b4d8d:

    puStack_28 = puVar1;

    Sound_PlaySample("SFX\\EVENTS\\MANA.WAV");

  }

  else {

    uVar3 = (**(code **)(*param_1 + 0x7c))();

    uVar8 = 0;

    uVar4 = (**(code **)(*param_1 + 0x78))();

    uVar5 = (**(code **)(*param_1 + 0x74))();

    cVar2 = AoWE_TAoWHSMap_Watching_23EDC2EF

                      (*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,uVar5,uVar4,uVar8,uVar3);

    puVar1 = puStack_28;

    if (cVar2 != '\0') goto LAB_557b4d8d;

  }

  iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                     (int)*(char *)(*(int *)(param_2 + 0x1c) + 0x12));

  AoWE_TPlayerMagicControl_SetMana_23EDC2EF

            (*(int *)(iVar6 + 0x54),*(int *)(*(int *)(iVar6 + 0x54) + 0x24) + param_1[7]);

  piVar7 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF

                            (&EventLog__TLocationTextEventLog_4746E6CB,1,0);

  *(undefined1 *)(piVar7 + 4) = *(undefined1 *)(*(int *)(param_2 + 0x1c) + 0x12);

  puVar10 = &local_8;

  SysUtils_IntToStr_0F6FDFF6(param_1[7],&local_14);

  local_10 = local_14;

  local_c = CONCAT31(local_c._1_3_,0xb);

  puVar9 = &local_10;

  System_LoadResString_02614E1B(PTR_AoWE_ManaCrystalsFoundRStr_04ECFCC5_558e8ffc,&local_8);

  AoWE_TranslateRStr_435799AA(local_8,&local_18);

  SysUtils_Format_D8F48273(local_18,puVar9,0,puVar10);

  System__LStrAsg_51F89FF7(piVar7 + 6,local_8);

  uVar3 = (**(code **)(*param_1 + 0x7c))();

  uVar4 = (**(code **)(*param_1 + 0x78))();

  uVar5 = (**(code **)(*param_1 + 0x74))();

  EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar7,uVar5,uVar4,uVar3);

  iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                     (int)*(char *)(*(int *)(param_2 + 0x1c) + 0x12));

  EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar6 + 0xd8),piVar7);

  (**(code **)(*piVar7 + 0x2c))();

  (**(code **)(*param_1 + -4))(param_1,1);

  puVar1 = puStack_28;

LAB_557b4e7f:

  puStack_28 = puVar1;

  puVar1 = puStack_28;

  *in_FS_OFFSET = uStack_30;

  puStack_28 = &LAB_557b4ea9;

  puStack_2c = (undefined1 *)0x557b4e99;

  System__LStrArrayClr_51F89FF7(&local_18,2,puVar1);

  puStack_2c = (undefined1 *)0x557b4ea1;

  System__LStrClr_40929B27(&local_8);

  return;

}




