

void PrdPlace_TProductionPlace_ListIncomeInfo_23EDC2EF(int *param_1,int *param_2,int *param_3)



{

  undefined1 *puVar1;

  char cVar2;

  int iVar3;

  undefined4 uVar4;

  uint uVar5;

  uint uVar6;

  undefined4 *in_FS_OFFSET;

  undefined4 *puVar7;

  undefined4 uStack_48;

  undefined1 *puStack_44;

  undefined1 *puStack_40;

  undefined4 local_30;

  undefined4 local_2c;

  undefined4 local_28;

  undefined1 local_24;

  undefined4 local_20;

  undefined1 local_1c;

  int local_18;

  undefined4 local_14;

  undefined4 local_10;

  int *local_c;

  int *local_8;

  

                    /* 0xbe59c  7467  PrdPlace.TProductionPlace.ListIncomeInfo@23EDC2EF */

  puStack_40 = &stack0xfffffffc;

  local_10 = 0;

  local_14 = 0;

  local_2c = 0;

  local_30 = 0;

  puStack_44 = &LAB_557be815;

  uStack_48 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_48;

  local_c = param_3;

  local_8 = param_2;

  cVar2 = (**(code **)(*param_1 + 0x230))();

  if (cVar2 == '\b') {

    iVar3 = (**(code **)(*param_1 + 0x22c))();

    if (0 < iVar3) {

      System_LoadResString_02614E1B(PTR_AoWE_BaseIncomeRStr_04ECFCC5_558e9254,&local_14);

      AoWE_TranslateRStr_435799AA(local_14,&local_10);

      (**(code **)(*local_8 + 0x34))(local_8,local_10);

      uVar4 = (**(code **)(*param_1 + 0x22c))();

      SysUtils_IntToStr_0F6FDFF6(uVar4,&local_10);

      (**(code **)(*local_c + 0x34))(local_c,local_10);

      if (*(char *)(param_1[0x11] + 0x20) == '\x02') {

        iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(param_1[0x11] + 0x14),0);

        if (*(int *)(iVar3 + 8) != 10) goto LAB_557be7ed;

        iVar3 = (**(code **)(*param_1 + 0x22c))();

        if (iVar3 < 1) goto LAB_557be7ed;

      }

      System_LoadResString_02614E1B(PTR_AoWE_ProducingMerchandiseRStr_04ECFCC5_558e8fc0,&local_14);

      AoWE_TranslateRStr_435799AA(local_14,&local_10);

      (**(code **)(*local_8 + 0x34))(local_8,local_10);

      local_18 = (**(code **)(*param_1 + 0x22c))();

      uVar4 = System__ROUND_51F89FF7();

      SysUtils_IntToStr_0F6FDFF6(uVar4,&local_10);

      (**(code **)(*local_c + 0x34))(local_c,local_10);

    }

  }

  else if ('\0' < (char)param_1[0xc]) {

    uVar4 = (**(code **)(*param_1 + 0x230))();

    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                       (int)(char)param_1[0xc]);

    uVar5 = AoWE_TLeader_GetSpherePicks_23EDC2EF(*(undefined4 *)(iVar3 + 0xd4),uVar4);

    uVar5 = uVar5 & 0xff;

    puVar7 = &local_10;

    SysUtils_IntToStr_0F6FDFF6(uVar5,&local_14);

    local_28 = local_14;

    local_24 = 0xb;

    uVar6 = (**(code **)(*param_1 + 0x230))();

    AoWE_TranslateRStr_435799AA

              (*(undefined4 *)(PTR_AoWE_TMagicSphereRStr_D4FDF8A3_558e8fb4 + (uVar6 & 0x7f) * 4),

               &local_10);

    SysUtils_LowerCase_435799AA(local_10,&local_2c);

    local_20 = local_2c;

    local_1c = 0xb;

    System_LoadResString_02614E1B(PTR_AoWE_XtimeYSphereRStr_04ECFCC5_558e8f18,&local_10);

    AoWE_TranslateRStr_435799AA(local_10,&local_30);

    SysUtils_Format_D8F48273(local_30,&local_28,1,puVar7);

    (**(code **)(*local_8 + 0x34))(local_8,local_10);

    SysUtils_IntToStr_0F6FDFF6(uVar5 * 5,&local_10);

    (**(code **)(*local_c + 0x34))(local_c,local_10);

    if (*(char *)(param_1[0x11] + 0x20) == '\x02') {

      iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(param_1[0x11] + 0x14),0);

      if (*(int *)(iVar3 + 8) != 10) goto LAB_557be7ed;

    }

    System_LoadResString_02614E1B(PTR_AoWE_ExtraPowerRStr_04ECFCC5_558e9158,&local_14);

    AoWE_TranslateRStr_435799AA(local_14,&local_10);

    (**(code **)(*local_8 + 0x34))(local_8,local_10);

    local_18 = uVar5 * 5;

    uVar4 = System__ROUND_51F89FF7();

    SysUtils_IntToStr_0F6FDFF6(uVar4,&local_10);

    (**(code **)(*local_c + 0x34))(local_c,local_10);

  }

LAB_557be7ed:

  puVar1 = puStack_40;

  *in_FS_OFFSET = uStack_48;

  puStack_40 = &LAB_557be81c;

  puStack_44 = (undefined1 *)0x557be807;

  System__LStrArrayClr_51F89FF7(&local_30,2,puVar1);

  puStack_44 = (undefined1 *)0x557be814;

  System__LStrArrayClr_51F89FF7(&local_14,2);

  return;

}




