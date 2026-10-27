// /AoW.exe : 0x407db8

void FUN_00407db8(int param_1)

{
  int *piVar1;
  int *piVar2;
  undefined1 *puVar3;
  char cVar4;
  byte bVar5;
  undefined1 uVar6;
  int iVar7;
  undefined4 uVar8;
  uint uVar9;
  int *piVar10;
  undefined4 *in_FS_OFFSET;
  undefined *puVar11;
  undefined4 *puVar12;
  undefined4 uVar13;
  undefined4 *puVar14;
  undefined4 uStack_38;
  undefined1 *puStack_34;
  undefined1 *puStack_30;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  puStack_30 = &stack0xfffffffc;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_34 = &LAB_004094d1;
  uStack_38 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_38;
  puVar3 = &stack0xfffffffc;
  local_8 = param_1;
  if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc0) != 0) {
    FUN_00455d10(*(undefined4 *)PTR_DAT_0045a420);
    piVar1 = *(int **)(*(int *)PTR_DAT_0045a420 + 0xc0);
    if ((piVar1 != (int *)0x0) &&
       (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) == (char)piVar1[9])) {
      AoWE_TTutorialControl_ShowMessage_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x40,1,1);
    }
    cVar4 = System__IsClass_51F89FF7(piVar1,AoWE__THero_228E24B5_exref);
    piVar10 = piVar1;
    if (cVar4 == '\0') {
      piVar10 = (int *)0x0;
    }
    if ((((*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref == 0) &&
         (*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) != 0)) &&
        (iVar7 = FUN_00454100(*(undefined4 *)PTR_DAT_0045a420,piVar1), -1 < iVar7)) &&
       (-1 < *(int *)(*(int *)PTR_DAT_0045a420 + 0x88))) {
      (**(code **)(**(int **)(DAT_0045b070 + 0x214) + 0x6c))(*(int **)(DAT_0045b070 + 0x214),1);
      (**(code **)(**(int **)(DAT_0045b070 + 0x210) + 0x6c))(*(int **)(DAT_0045b070 + 0x210),1);
    }
    else {
      (**(code **)(**(int **)(DAT_0045b070 + 0x214) + 0x6c))(*(int **)(DAT_0045b070 + 0x214),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x210) + 0x6c))(*(int **)(DAT_0045b070 + 0x210),0);
    }
    (**(code **)(**(int **)(DAT_0045b070 + 0x70) + 0x6c))(*(int **)(DAT_0045b070 + 0x70),0);
    (**(code **)(**(int **)(DAT_0045b070 + 0x6c) + 0x6c))(*(int **)(DAT_0045b070 + 0x6c),0);
    (**(code **)(**(int **)(*(int *)(DAT_0045b070 + 0x4c) + 0x114) + 0x40))();
    System_LoadResString_02614E1B(PTR_PTR_0045a5b4,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    piVar2 = *(int **)(*(int *)(DAT_0045b070 + 0x4c) + 0x114);
    (**(code **)(*piVar2 + 0x34))(piVar2,local_c);
    System_LoadResString_02614E1B(PTR_PTR_0045a160,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    piVar2 = *(int **)(*(int *)(DAT_0045b070 + 0x4c) + 0x114);
    (**(code **)(*piVar2 + 0x34))(piVar2,local_c);
    cVar4 = (**(code **)(*piVar1 + 0xa4))();
    if (cVar4 < '\0') {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x1ec) + 0x6c))(*(int **)(DAT_0045b070 + 0x1ec),0);
      System_LoadResString_02614E1B(PTR_PTR_0045a134,&local_10);
      AoWE_TranslateRStr_435799AA(local_10,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1f0),local_c);
    }
    else {
      cVar4 = (**(code **)(*piVar1 + 0xa4))();
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(AoWE_RaceNamesRStr_B11890CE_exref + cVar4 * 4),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),local_c);
      (**(code **)(**(int **)(DAT_0045b070 + 0x1ec) + 0x6c))(*(int **)(DAT_0045b070 + 0x1ec),1);
      cVar4 = (**(code **)(*piVar1 + 0xa4))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),(int)cVar4);
      bVar5 = AoWE_TRace_GetPlayerRelation_23EDC2EF
                        (uVar8,CONCAT31((int3)((uint)*(int *)AoWE_AoWHSMap_6B957A58_exref >> 8),
                                        *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)
                                       ));
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1ec),bVar5 + 0xb);
      cVar4 = (**(code **)(*piVar1 + 0xa4))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),(int)cVar4);
      uVar9 = AoWE_TRace_GetPlayerRelation_23EDC2EF
                        (uVar8,CONCAT31((int3)((uint)*(int *)AoWE_AoWHSMap_6B957A58_exref >> 8),
                                        *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)
                                       ));
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(AoWE_TStatusRStr_8E1B04C3_exref + (uVar9 & 0x7f) * 4),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1f0),local_c);
    }
    if (((*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) == (char)piVar1[9]) &&
        (cVar4 = System__IsClass_51F89FF7(piVar1,AoWE__TLeader_DF642EA6_exref), cVar4 == '\0')) &&
       (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0)) {
      (**(code **)(**(int **)(DAT_0045b070 + 0x15c) + 0x6c))(*(int **)(DAT_0045b070 + 0x15c),1);
      *(undefined1 *)(*(int *)(*(int *)(DAT_0045b070 + 0x60) + 0xd4) + 0x28) = 1;
    }
    else {
      (**(code **)(**(int **)(DAT_0045b070 + 0x15c) + 0x6c))(*(int **)(DAT_0045b070 + 0x15c),0);
      *(undefined1 *)(*(int *)(*(int *)(DAT_0045b070 + 0x60) + 0xd4) + 0x28) = 4;
    }
    *(undefined1 *)(*(int *)(DAT_0045b070 + 0x60) + 0x2d) = 0;
    cVar4 = (**(code **)(*piVar1 + 0x114))();
    if (cVar4 == '\x01') {
      System_LoadResString_02614E1B(PTR_PTR_0045a348,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      uVar8 = local_10;
      System__LStrCatN_51F89FF7(&local_c,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),local_c);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe0),0x15);
    }
    else if (cVar4 == '\x02') {
      System_LoadResString_02614E1B(PTR_PTR_0045a25c,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      uVar8 = local_10;
      System__LStrCatN_51F89FF7(&local_c,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),local_c);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe0),0x16);
    }
    else {
      if (piVar10 == (int *)0x0) {
        System_LoadResString_02614E1B(PTR_PTR_0045a5a4,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_10);
        uVar8 = local_10;
        System__LStrCatN_51F89FF7(&local_c,3);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),local_c);
      }
      else {
        cVar4 = System__IsClass_51F89FF7(piVar10,AoWE__TLeader_DF642EA6_exref);
        if (cVar4 == '\0') {
          System_LoadResString_02614E1B(PTR_PTR_0045a508,&local_14);
          AoWE_TranslateRStr_435799AA(local_14,&local_10);
          uVar8 = local_10;
          System__LStrCatN_51F89FF7(&local_c,3);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),local_c);
        }
        else {
          System_LoadResString_02614E1B(PTR_PTR_0045a2bc,&local_14);
          AoWE_TranslateRStr_435799AA(local_14,&local_10);
          uVar8 = local_10;
          System__LStrCatN_51F89FF7(&local_c,3);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe4),local_c);
        }
      }
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xe0),0x17);
    }
    if (piVar10 == (int *)0x0) {
      System_LoadResString_02614E1B(PTR_PTR_0045a154,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      puVar11 = &DAT_004094e8;
      uVar13 = local_10;
      cVar4 = (**(code **)(*piVar1 + 0xa0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_14);
      uVar8 = local_14;
      System__LStrCatN_51F89FF7(&local_c,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1b8),local_c);
      iVar7 = **(int **)(DAT_0045b070 + 0xac);
      (**(code **)(iVar7 + 0x6c))(*(int **)(DAT_0045b070 + 0xac),0,iVar7,uVar8,puVar11,uVar13);
      (**(code **)(**(int **)(DAT_0045b070 + 0xa8) + 0x6c))(*(int **)(DAT_0045b070 + 0xa8),1);
      uVar8 = (**(code **)(*piVar1 + 0x168))();
      piVar2 = *(int **)(*(int *)(DAT_0045b070 + 0x68) + 0x118);
      (**(code **)(*piVar2 + 8))(piVar2,uVar8);
      AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xa4),0);
      (**(code **)(*piVar1 + 0xf8))(piVar1,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x170),local_c);
    }
    else {
      if ((char)piVar10[9] == '\0') {
        (**(code **)(**(int **)(DAT_0045b070 + 0xb4) + 0x6c))(*(int **)(DAT_0045b070 + 0xb4),0);
        System_LoadResString_02614E1B(PTR_PTR_0045a408,&local_10);
        AoWE_TranslateRStr_435799AA(local_10,&local_c);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xf4),local_c);
      }
      else {
        iVar7 = **(int **)(DAT_0045b070 + 0xb4);
        (**(code **)(iVar7 + 0x6c))(*(int **)(DAT_0045b070 + 0xb4),1,iVar7,uVar8);
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 1) {
          (**(code **)(**(int **)(DAT_0045b070 + 0xc0) + 0x6c))(*(int **)(DAT_0045b070 + 0xc0),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 0xc0) + 0x6c))(*(int **)(DAT_0045b070 + 0xc0),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),0);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 0xc0) + 0xa0))
                    (*(int **)(DAT_0045b070 + 0xc0),uVar8);
        }
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 2) {
          (**(code **)(**(int **)(DAT_0045b070 + 0xbc) + 0x6c))(*(int **)(DAT_0045b070 + 0xbc),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 0xbc) + 0x6c))(*(int **)(DAT_0045b070 + 0xbc),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),1);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 0xbc) + 0xa0))
                    (*(int **)(DAT_0045b070 + 0xbc),uVar8);
        }
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 3) {
          (**(code **)(**(int **)(DAT_0045b070 + 0xb8) + 0x6c))(*(int **)(DAT_0045b070 + 0xb8),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 0xb8) + 0x6c))(*(int **)(DAT_0045b070 + 0xb8),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),2);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 0xb8) + 0xa0))
                    (*(int **)(DAT_0045b070 + 0xb8),uVar8);
        }
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 4) {
          (**(code **)(**(int **)(DAT_0045b070 + 0xb0) + 0x6c))(*(int **)(DAT_0045b070 + 0xb0),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 0xb0) + 0x6c))(*(int **)(DAT_0045b070 + 0xb0),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),3);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 0xb0) + 0xa0))
                    (*(int **)(DAT_0045b070 + 0xb0),uVar8);
        }
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 5) {
          (**(code **)(**(int **)(DAT_0045b070 + 0xc4) + 0x6c))(*(int **)(DAT_0045b070 + 0xc4),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 0xc4) + 0x6c))(*(int **)(DAT_0045b070 + 0xc4),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),4);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 0xc4) + 0xa0))
                    (*(int **)(DAT_0045b070 + 0xc4),uVar8);
        }
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 6) {
          (**(code **)(**(int **)(DAT_0045b070 + 200) + 0x6c))(*(int **)(DAT_0045b070 + 200),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 200) + 0x6c))(*(int **)(DAT_0045b070 + 200),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),5);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 200) + 0xa0))(*(int **)(DAT_0045b070 + 200),uVar8);
        }
        iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)(char)piVar10[9]);
        iVar7 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4));
        if (iVar7 < 7) {
          (**(code **)(**(int **)(DAT_0045b070 + 0xcc) + 0x6c))(*(int **)(DAT_0045b070 + 0xcc),0);
        }
        else {
          (**(code **)(**(int **)(DAT_0045b070 + 0xcc) + 0x6c))(*(int **)(DAT_0045b070 + 0xcc),1);
          iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)(char)piVar10[9]);
          uVar8 = AoWE_TLeader_GetSpheres_23EDC2EF(*(undefined4 *)(iVar7 + 0xd4),6);
          uVar8 = FUN_00454160(*(undefined4 *)PTR_DAT_0045a420,uVar8);
          (**(code **)(**(int **)(DAT_0045b070 + 0xcc) + 0xa0))
                    (*(int **)(DAT_0045b070 + 0xcc),uVar8);
        }
        System_LoadResString_02614E1B(PTR_PTR_0045a118,&local_10);
        AoWE_TranslateRStr_435799AA(local_10,&local_c);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0xf4),local_c);
      }
      if (((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) == 0) &&
          ((char)piVar10[9] == *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5))) ||
         ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) != 0 &&
          (iVar7 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                             (*(undefined4 *)
                               (*(int *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) +
                                        0xc) + 0x38),
                              *(undefined1 *)
                               (*(int *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) +
                                        0xc) + 0x31)), *(char *)(iVar7 + 8) == (char)piVar10[9]))))
      {
        System_LoadResString_02614E1B(PTR_PTR_0045a5e0,&local_10);
        AoWE_TranslateRStr_435799AA(local_10,&local_c);
        piVar2 = *(int **)(*(int *)(DAT_0045b070 + 0x4c) + 0x114);
        (**(code **)(*piVar2 + 0x34))(piVar2,local_c);
      }
      cVar4 = (**(code **)(*piVar10 + 0x128))();
      if ('\0' < cVar4) {
        (**(code **)(**(int **)(DAT_0045b070 + 0x70) + 0x6c))(*(int **)(DAT_0045b070 + 0x70),1);
        cVar4 = (**(code **)(*piVar10 + 300))();
        SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_10);
        puVar11 = &DAT_004094f4;
        uVar13 = local_10;
        cVar4 = (**(code **)(*piVar10 + 0x128))();
        SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_14);
        uVar8 = local_14;
        System__LStrCatN_51F89FF7(&local_c,3);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x70),local_c);
        iVar7 = **(int **)(DAT_0045b070 + 0x6c);
        (**(code **)(iVar7 + 0x6c))(*(int **)(DAT_0045b070 + 0x6c),1,iVar7,uVar8,puVar11,uVar13);
      }
      (**(code **)(**(int **)(DAT_0045b070 + 0x164) + 0x6c))(*(int **)(DAT_0045b070 + 0x164),0);
      *(undefined4 *)(DAT_0045b070 + 0x234) = 0;
      System_LoadResString_02614E1B(PTR_PTR_0045a154,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      puVar11 = &DAT_004094e8;
      uVar13 = local_10;
      cVar4 = AoWE_THero_GetLevel_23EDC2EF(piVar10);
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_14);
      uVar8 = local_14;
      System__LStrCatN_51F89FF7(&local_c,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1b8),local_c);
      iVar7 = **(int **)(DAT_0045b070 + 0xac);
      (**(code **)(iVar7 + 0x6c))(*(int **)(DAT_0045b070 + 0xac),1,iVar7,uVar8,puVar11,uVar13);
      (**(code **)(**(int **)(DAT_0045b070 + 0xa8) + 0x6c))(*(int **)(DAT_0045b070 + 0xa8),0);
      (**(code **)(*piVar10 + 0x1b8))(piVar10,&local_10,*piVar10,&DAT_004094e8,piVar10[0x18]);
      System__LStrCatN_51F89FF7(&local_c,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x170),local_c);
    }
    System_LoadResString_02614E1B(PTR_PTR_0045a244,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_10);
    uVar8 = (**(code **)(*piVar1 + 0x15c))();
    SysUtils_IntToStr_0F6FDFF6(uVar8,&local_14);
    uVar8 = (**(code **)(*piVar1 + 0x164))();
    SysUtils_IntToStr_0F6FDFF6(uVar8,&local_18);
    System__LStrCatN_51F89FF7(&local_c,6);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x20c),local_c);
    AoWWinGeneric_TAOWGenericWindow_SetCursor_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x44),0);
    iVar7 = (**(code **)(**(int **)(*(int *)(DAT_0045b070 + 0x4c) + 0x114) + 0x14))();
    if ((iVar7 <= *(int *)(*(int *)(DAT_0045b070 + 0x4c) + 0x118)) ||
       (*(int *)(*(int *)(DAT_0045b070 + 0x4c) + 0x118) < 0)) {
      AoWTabPanel_TAOWTabPanel_SetIndex_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x4c),0);
    }
    (**(code **)(**(int **)(DAT_0045b070 + 0x44) + 0x6c))(*(int **)(DAT_0045b070 + 0x44),1);
    if ((piVar10 != (int *)0x0) &&
       (((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) == 0 &&
         ((char)piVar10[9] == *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5))) ||
        ((*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) != 0 &&
         (iVar7 = AoWE_TCombatPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)
                              (*(int *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) + 0xc
                                       ) + 0x38),
                             *(undefined1 *)
                              (*(int *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x120) + 0xc
                                       ) + 0x31)), *(char *)(iVar7 + 8) == (char)piVar10[9])))))) {
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],0);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0xfc) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],0);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0xfc) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],1);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x104) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],1);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x104) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],2);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x108) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],2);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x108) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],3);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x10c) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],3);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x10c) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],4);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x110) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],4);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x110) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],5);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x114) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],5);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x114) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],6);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x118) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],6);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x118) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],7);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x11c) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = (**(code **)(*(int *)piVar10[0x1d] + 0x8c))((int *)piVar10[0x1d],7);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x11c) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],0);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x130) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],0);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x130) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],2);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x134) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],2);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x134) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],1);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x128) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],1);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x128) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],4);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 300) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],4);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 300) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],3);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x138) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],3);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x138) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],5);
      if (iVar7 == 0) {
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x13c) + 0xc) = 0xffffffff;
      }
      else {
        iVar7 = AoWE_THeroItems_GetPositionItem_23EDC2EF(piVar10[0x1c],5);
        *(undefined4 *)(*(int *)(DAT_0045b070 + 0x13c) + 0xc) = *(undefined4 *)(iVar7 + 0x14);
      }
      FUN_00409510(DAT_0045b070,0);
      DAT_0045b074 = 0;
      (**(code **)(**(int **)(DAT_0045b070 + 0xfc) + 0xac))(*(int **)(DAT_0045b070 + 0xfc),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x104) + 0xac))(*(int **)(DAT_0045b070 + 0x104),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x108) + 0xac))(*(int **)(DAT_0045b070 + 0x108),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x10c) + 0xac))(*(int **)(DAT_0045b070 + 0x10c),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x110) + 0xac))(*(int **)(DAT_0045b070 + 0x110),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x114) + 0xac))(*(int **)(DAT_0045b070 + 0x114),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x118) + 0xac))(*(int **)(DAT_0045b070 + 0x118),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x11c) + 0xac))(*(int **)(DAT_0045b070 + 0x11c),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x130) + 0xac))(*(int **)(DAT_0045b070 + 0x130),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x134) + 0xac))(*(int **)(DAT_0045b070 + 0x134),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x128) + 0xac))(*(int **)(DAT_0045b070 + 0x128),0);
      (**(code **)(**(int **)(DAT_0045b070 + 300) + 0xac))(*(int **)(DAT_0045b070 + 300),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x138) + 0xac))(*(int **)(DAT_0045b070 + 0x138),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x13c) + 0xac))(*(int **)(DAT_0045b070 + 0x13c),0);
      (**(code **)(**(int **)(DAT_0045b070 + 0x164) + 0x6c))(*(int **)(DAT_0045b070 + 0x164),0);
    }
    puVar3 = puStack_30;
    if (piVar1 != (int *)0x0) {
      cVar4 = (**(code **)(*piVar1 + 0xe0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_10);
      cVar4 = (**(code **)(*piVar1 + 0xd0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_14);
      System__LStrCatN_51F89FF7(&local_c,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1a4),local_c);
      if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
        cVar4 = (**(code **)(*piVar1 + 0xd8))();
        SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_10);
        cVar4 = (**(code **)(*piVar1 + 0xd4))();
        SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_14);
        System__LStrCatN_51F89FF7(&local_c,3);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x198),local_c);
      }
      else if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) != 0) {
        SysUtils_IntToStr_0F6FDFF6
                  (*(undefined4 *)
                    (*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x5c),&local_10);
        uVar8 = (**(code **)(**(int **)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x94))()
        ;
        SysUtils_IntToStr_0F6FDFF6(uVar8,&local_14);
        System__LStrCatN_51F89FF7(&local_c,3);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x198),local_c);
      }
      cVar4 = (**(code **)(*piVar1 + 0xc0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 400),local_c);
      cVar4 = (**(code **)(*piVar1 + 200))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x19c),local_c);
      cVar4 = (**(code **)(*piVar1 + 0xc4))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x194),local_c);
      cVar4 = (**(code **)(*piVar1 + 0xcc))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1a0),local_c);
      uVar8 = (**(code **)(*piVar1 + 0xe8))();
      uVar8 = FUN_004541b0(*(undefined4 *)PTR_DAT_0045a420,uVar8);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x180),uVar8);
      iVar7 = AoWE_TAbstractUnit_GetManaUpkeep_23EDC2EF(piVar1);
      if (iVar7 < 1) {
        uVar6 = (**(code **)(*piVar1 + 0x9c))();
        SysUtils_IntToStr_0F6FDFF6(uVar6,&local_c);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1d8),local_c);
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0x1d4),0x33);
      }
      else {
        uVar8 = AoWE_TAbstractUnit_GetManaUpkeep_23EDC2EF(piVar1);
        SysUtils_IntToStr_0F6FDFF6(uVar8,&local_c);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x1d8),local_c);
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0x1d4),0x34);
      }
      cVar4 = (**(code **)(*piVar1 + 0xfc))();
      (**(code **)(**(int **)(local_8 + 0xd0) + 0x6c))(*(int **)(local_8 + 0xd0),cVar4 != '\x06');
      uVar9 = (**(code **)(*piVar1 + 0xfc))();
      switch(uVar9 & 0x7f) {
      case 0:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0xd0),0x1f);
        break;
      case 1:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0xd0),0x20);
        break;
      case 2:
      case 3:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0xd0),0x21);
        break;
      case 4:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0xd0),0x22);
        break;
      case 5:
        AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(local_8 + 0xd0),0x23);
      }
      uVar9 = (**(code **)(*piVar1 + 0xfc))();
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(AoWE_TAlignmentRStr_08846F84_exref + (uVar9 & 0x7f) * 4),&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xd4),local_c);
      (**(code **)(**(int **)(*(int *)(DAT_0045b070 + 0x74) + 0x114) + 0x40))();
      (**(code **)(**(int **)(*(int *)(DAT_0045b070 + 0x94) + 0x114) + 0x40))();
      (**(code **)(**(int **)(*(int *)(DAT_0045b070 + 0x98) + 0x114) + 0x40))();
      puVar14 = &local_c;
      uVar9 = AoWE_TAbstractUnit_GetMorale_23EDC2EF(piVar1);
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(AoWE_TMoraleRStr_7874CF1F_exref + (uVar9 & 0x7f) * 4),&local_10);
      local_20 = local_10;
      local_1c = CONCAT31(local_1c._1_3_,0xb);
      puVar12 = &local_20;
      System_LoadResString_02614E1B(AoWE_MoraleXRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_14);
      AoWE_Format_D8F48273(local_14,puVar12,0,puVar14);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0x1d0),local_c);
      cVar4 = (**(code **)(*piVar1 + 0x114))();
      (**(code **)(**(int **)(local_8 + 0x1d0) + 0x6c))(*(int **)(local_8 + 0x1d0),cVar4 != '\x02');
      if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
        AoWE_TAbstractUnit_ListAbilityInfo_23EDC2EF
                  (piVar1,*(undefined4 *)(*(int *)(local_8 + 0x74) + 0x114),
                   *(undefined4 *)(*(int *)(local_8 + 0x94) + 0x114),
                   *(undefined4 *)(*(int *)(local_8 + 0x98) + 0x114));
        local_20 = *(undefined4 *)(*(int *)(local_8 + 0x94) + 0x114);
        local_1c = *(undefined4 *)(*(int *)(local_8 + 0x98) + 0x114);
        Engine_SortStringsEx_0A456501
                  (*(undefined4 *)(*(int *)(local_8 + 0x74) + 0x114),&LAB_00407adc,&local_20,1);
      }
      else {
        iVar7 = *(int *)(*(int *)PTR_DAT_0045a420 + 0xc4);
        if (((iVar7 != 0) && (*(int *)(iVar7 + 0x1c) != 0)) &&
           (*(int *)(*(int *)(iVar7 + 0x1c) + 0x4c) != 0)) {
          AoWE_TAbstractUnit_ListAbilityInfo_23EDC2EF
                    (*(undefined4 *)(*(int *)(iVar7 + 0x1c) + 0x4c),
                     *(undefined4 *)(*(int *)(local_8 + 0x74) + 0x114),
                     *(undefined4 *)(*(int *)(local_8 + 0x94) + 0x114),
                     *(undefined4 *)(*(int *)(local_8 + 0x98) + 0x114));
          local_20 = *(undefined4 *)(*(int *)(local_8 + 0x94) + 0x114);
          local_1c = *(undefined4 *)(*(int *)(local_8 + 0x98) + 0x114);
          Engine_SortStringsEx_0A456501
                    (*(undefined4 *)(*(int *)(local_8 + 0x74) + 0x114),&LAB_00407adc,&local_20,1);
        }
      }
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(DAT_0045b070 + 0x74),0xffffffff);
      FUN_00409798(DAT_0045b070,*(undefined4 *)(local_8 + 0x74));
      *(undefined1 *)(*(int *)(local_8 + 0x44) + 0xa4) = 0;
      puVar3 = puStack_30;
    }
  }
  puStack_30 = puVar3;
  puVar3 = puStack_30;
  *in_FS_OFFSET = uStack_38;
  puStack_30 = &LAB_004094d8;
  puStack_34 = (undefined1 *)0x4094d0;
  System__LStrArrayClr_51F89FF7(&local_18,4,puVar3);
  return;
}

