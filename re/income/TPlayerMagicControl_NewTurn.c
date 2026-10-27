

void AoWE_TPlayerMagicControl_NewTurn_23EDC2EF(int param_1)



{

  uint uVar1;

  char cVar2;

  int iVar3;

  int iVar4;

  int iVar5;

  int iVar6;

  undefined4 uVar7;

  int *piVar8;

  int iVar9;

  char local_18;

  

                    /* 0x7cc2c  1197  AoWE.TPlayerMagicControl.NewTurn@23EDC2EF */

  if (((*(int *)(AoWE_AoWHSMap_6B957A58 + 0x174) == 1) &&

      (*(char *)(*(int *)(param_1 + 0x10) + 0xa6) != '\0')) &&

     ((*(char *)(AoWE_AoWHSMap_6B957A58 + 0x11a) != '\x01' ||

      (*(int *)(*(int *)(param_1 + 0x30) + 8) == 0)))) {

    iVar3 = System_TObject_Create_23EDC2EF(Engine__TIntegerList_D8218C48_exref,1);

    local_18 = '\0';

    do {

      Engine_TIntegerList_Clear_23EDC2EF(iVar3);

      iVar4 = AoWE_TSpellControl_GetCount_23EDC2EF(*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x84));

      if (-1 < iVar4 + -1) {

        iVar9 = 0;

        do {

          iVar5 = AoWE_TSpellControl_GetSpell_23EDC2EF

                            (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x84),iVar9);

          if (iVar5 != 0) {

            iVar6 = Engine_TIntegerList_IndexOf_23EDC2EF(*(undefined4 *)(param_1 + 0x30),iVar9);

            if ((iVar6 == -1) && (*(char *)(iVar5 + 0x22) == local_18)) {

              if (*(char *)(iVar5 + 0x20) != '\0') {

                cVar2 = AoWE_TLeader_GetSpherePicks_23EDC2EF

                                  (*(undefined4 *)(*(int *)(param_1 + 0x10) + 0xd4),

                                   *(undefined1 *)(iVar5 + 0x20));

                if (cVar2 == '\0') goto LAB_5577cd02;

              }

              if (*(char *)(iVar5 + 0x21) == '\x01') {

                Engine_TIntegerList_Add_23EDC2EF(iVar3,iVar9);

              }

            }

          }

LAB_5577cd02:

          iVar9 = iVar9 + 1;

          iVar4 = iVar4 + -1;

        } while (iVar4 != 0);

      }

      if (0 < *(int *)(iVar3 + 8)) {

        uVar7 = AoWE_TAoWHSMap_Random_23EDC2EF(AoWE_AoWHSMap_6B957A58,*(int *)(iVar3 + 8));

        uVar7 = Engine_TIntegerList_Get_23EDC2EF(iVar3,uVar7);

        Engine_TIntegerList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x30),uVar7);

      }

      local_18 = local_18 + '\x01';

    } while (local_18 != '\x03');

    System_TObject_Free_23EDC2EF(iVar3);

  }

  if (*(int *)(param_1 + 0x34) == 0) {

    iVar3 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(param_1);

    *(int *)(param_1 + 0x24) = *(int *)(param_1 + 0x24) + iVar3;

  }

  else {

    iVar3 = Engine_TIntegerList_IndexOf_23EDC2EF

                      (*(undefined4 *)(param_1 + 0x30),*(int *)(param_1 + 0x34));

    if (iVar3 == -1) {

      iVar3 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(param_1);

      *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) - iVar3;

      uVar1 = *(uint *)(param_1 + 0x38);

      if ((int)uVar1 < 1) {

        if ((int)uVar1 < 0) {

          *(int *)(param_1 + 0x24) =

               *(int *)(param_1 + 0x24) + ((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f));

        }

        piVar8 = (int *)AoWE_TPlayerMagicEventLog_Create_23EDC2EF

                                  (&AoWE__TPlayerMagicEventLog_A81505B3,1,0);

        *(undefined1 *)(piVar8 + 4) = *(undefined1 *)(*(int *)(param_1 + 0x10) + 0xa6);

        *(undefined1 *)(piVar8 + 6) = 0;

        piVar8[8] = *(int *)(param_1 + 0x34);

        EventLog_TEventLogbook_AddEvent_23EDC2EF

                  (*(undefined4 *)(*(int *)(param_1 + 0x10) + 0xd8),piVar8);

        AoWE_TPlayerMagicControl_ExecuteSpellResearched_23EDC2EF

                  (param_1,*(undefined4 *)(param_1 + 0x34));

        if ((*(char *)(*(int *)(param_1 + 0x10) + 0xa6) == *(char *)(AoWE_AoWHSMap_6B957A58 + 0xa5))

           && (*(char *)(*(int *)(param_1 + 0x10) + 0xa7) == '\0')) {

          (**(code **)(*piVar8 + 0x70))(piVar8,0,0,0,0);

        }

        (**(code **)(*piVar8 + 0x2c))();

      }

    }

    else {

      *(undefined4 *)(param_1 + 0x34) = 0;

      iVar3 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(param_1);

      *(int *)(param_1 + 0x24) = *(int *)(param_1 + 0x24) + iVar3;

    }

  }

  iVar4 = AoWE_TPlayerMagicControl_GetManaIncome_23EDC2EF(param_1);

  iVar3 = *(int *)(param_1 + 0x24);

  iVar9 = AoWE_TPlayerMagicControl_GetManaUpkeep_23EDC2EF(param_1);

  iVar9 = (iVar4 + iVar3) - iVar9;

  *(int *)(param_1 + 0x24) = iVar9;

  if (iVar9 < 0) {

    AoWE_TPlayerMagicControl_OutOfMana_23EDC2EF(param_1,-iVar9);

    *(undefined4 *)(param_1 + 0x24) = 0;

  }

  AoWE_TPlayerMagicControl_SetMana_23EDC2EF(param_1,*(undefined4 *)(param_1 + 0x24));

  AoWE_TPlayerMagicControl_Update_23EDC2EF(param_1);

  return;

}




