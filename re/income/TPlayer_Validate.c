

void AoWE_TPlayer_Validate_23EDC2EF(int param_1,int *param_2)



{

  undefined1 *puVar1;

  int iVar2;

  undefined4 *in_FS_OFFSET;

  undefined4 uVar3;

  undefined4 *puVar4;

  undefined *puVar5;

  undefined4 *puVar6;

  undefined4 uVar7;

  undefined4 uStack_34;

  undefined1 *puStack_30;

  undefined1 *puStack_2c;

  undefined4 local_20;

  undefined4 local_1c;

  undefined4 local_18;

  undefined4 local_14;

  undefined4 local_10;

  undefined4 local_c;

  int *local_8;

  

                    /* 0x51c00  2632  AoWE.TPlayer.Validate@23EDC2EF */

  puStack_2c = &stack0xfffffffc;

  local_c = 0;

  local_10 = 0;

  local_14 = 0;

  local_18 = 0;

  local_1c = 0;

  local_20 = 0;

  puStack_30 = &LAB_55751e05;

  uStack_34 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_34;

  puVar1 = &stack0xfffffffc;

  local_8 = param_2;

  if ('\0' < *(char *)(param_1 + 0xa6)) {

    iVar2 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(param_1 + 0xd4));

    if (iVar2 != *(int *)(AoWE_AoWHSMap_6B957A58 + 0xec)) {

      puVar6 = &local_10;

      AoWE_TranslateRStr_435799AA

                (*(undefined4 *)(&AoWE_RaceNamesRStr_B11890CE + *(char *)(param_1 + 0xa5) * 4),

                 &local_1c);

      local_18 = local_1c;

      local_14 = CONCAT31(local_14._1_3_,0xb);

      puVar4 = &local_18;

      System_LoadResString_02614E1B(&AoWE_XLeaderRStr_8276FCC3,&local_20);

      AoWE_Format_D8F48273(local_20,puVar4,0,puVar6);

      puVar5 = &DAT_55751e1c;

      uVar7 = local_10;

      System_LoadResString_02614E1B(&AoWE_NotAllSpheresAssignedRStr_8276FCC3,&local_20);

      AoWE_TranslateRStr_435799AA(local_20,&local_1c);

      uVar3 = local_1c;

      System__LStrCatN_51F89FF7(&local_c,3);

      (**(code **)(*local_8 + 0x34))(local_8,local_c,*local_8,uVar3,puVar5,uVar7);

    }

    if (*(int *)(*(int *)(param_1 + 0xd4) + 4) == 0) {

      puVar6 = &local_10;

      AoWE_TranslateRStr_435799AA

                (*(undefined4 *)(&AoWE_RaceNamesRStr_B11890CE + *(char *)(param_1 + 0xa5) * 4),

                 &local_1c);

      local_18 = local_1c;

      local_14 = CONCAT31(local_14._1_3_,0xb);

      puVar4 = &local_18;

      System_LoadResString_02614E1B(&AoWE_XLeaderRStr_8276FCC3,&local_20);

      AoWE_Format_D8F48273(local_20,puVar4,0,puVar6);

      puVar5 = &DAT_55751e1c;

      uVar7 = local_10;

      System_LoadResString_02614E1B(&AoWE_NotPlacedOnMapRStr_8276FCC3,&local_20);

      AoWE_TranslateRStr_435799AA(local_20,&local_1c);

      uVar3 = local_1c;

      System__LStrCatN_51F89FF7(&local_c,3);

      (**(code **)(*local_8 + 0x34))(local_8,local_c,*local_8,uVar3,puVar5,uVar7);

    }

    iVar2 = AoWE_THero_GetSkillPoints_23EDC2EF(*(undefined4 *)(param_1 + 0xd4));

    puVar1 = puStack_2c;

    if (iVar2 != 0) {

      puVar6 = &local_10;

      AoWE_TranslateRStr_435799AA

                (*(undefined4 *)(&AoWE_RaceNamesRStr_B11890CE + *(char *)(param_1 + 0xa5) * 4),

                 &local_1c);

      local_18 = local_1c;

      local_14 = CONCAT31(local_14._1_3_,0xb);

      puVar4 = &local_18;

      System_LoadResString_02614E1B(&AoWE_XLeaderRStr_8276FCC3,&local_20);

      AoWE_Format_D8F48273(local_20,puVar4,0,puVar6);

      puVar5 = &DAT_55751e1c;

      uVar7 = local_10;

      System_LoadResString_02614E1B(&AoWE_NotAllSkillPointsAssignedRStr_8276FCC3,&local_20);

      AoWE_TranslateRStr_435799AA(local_20,&local_1c);

      uVar3 = local_1c;

      System__LStrCatN_51F89FF7(&local_c,3);

      (**(code **)(*local_8 + 0x34))(local_8,local_c,*local_8,uVar3,puVar5,uVar7);

      puVar1 = puStack_2c;

    }

  }

  puStack_2c = puVar1;

  puVar1 = puStack_2c;

  *in_FS_OFFSET = uStack_34;

  puStack_2c = &LAB_55751e0c;

  puStack_30 = (undefined1 *)0x55751df7;

  System__LStrArrayClr_51F89FF7(&local_20,2,puVar1);

  puStack_30 = (undefined1 *)0x55751e04;

  System__LStrArrayClr_51F89FF7(&local_10,2);

  return;

}




