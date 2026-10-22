
void FUN_00425a28(int param_1,char param_2)

{
  code *pcVar1;
  undefined1 *puVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 uVar7;
  uint uVar8;
  int iVar9;
  undefined4 *in_FS_OFFSET;
  undefined *puVar10;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  pcVar1 = AoWE_AoWHSMap_6B957A58_exref;
  puStack_20 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_24 = &LAB_00425c5f;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  FUN_00424984(param_1);
  *(undefined1 *)(param_1 + 0x286) = 0xff;
  *(char *)(param_1 + 0x287) = param_2;
  iVar3 = Engine_TIntegerList_IndexOf_23EDC2EF(*(undefined4 *)(param_1 + 0x28c),(int)param_2);
  piVar4 = (int *)Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3 + 0x24);
  *(undefined4 *)(piVar4[0x35] + 0x20) = 0xaf;
  (**(code **)(*piVar4 + 0xac))(piVar4,1);
  *(int *)(param_1 + 0x278) = piVar4[3];
  iVar3 = 0;
  do {
    if (iVar3 < *(int *)(*(int *)(param_1 + 0x288) + 8)) {
      uVar5 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(uVar5,1);
    }
    else {
      uVar5 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(uVar5,2);
    }
    iVar6 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3);
    if (*(int *)(iVar6 + 0x124) == 1) {
      iVar6 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3);
      (**(code **)(**(int **)(iVar6 + 0x104) + 0x40))();
      uVar5 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x288),iVar3);
      uVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)pcVar1 + 0x140),
                         (int)*(char *)(*(int *)pcVar1 + 0xa5));
      iVar6 = AoWE_TPlayer_GetFirstDaySeen_23EDC2EF(uVar7,uVar5);
      if (iVar6 != -1) {
        uVar5 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x288),iVar3);
        uVar7 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)pcVar1 + 0x144),(int)*(char *)(param_1 + 0x287));
        uVar8 = AoWE_TRace_GetPlayerRelation_23EDC2EF(uVar7,uVar5);
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)(AoWE_TStatusRStr_8E1B04C3_exref + (uVar8 & 0x7f) * 4),&local_c);
        uVar5 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x288),iVar3);
        uVar7 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)pcVar1 + 0x144),(int)*(char *)(param_1 + 0x287));
        uVar5 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar7,uVar5);
        SysUtils_IntToStr_0F6FDFF6(uVar5,&local_10);
        puVar10 = &DAT_00425c84;
        uVar7 = local_10;
        System__LStrCatN_51F89FF7(&local_8,4);
        uVar5 = local_8;
        iVar9 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3);
        iVar6 = **(int **)(iVar9 + 0x104);
        (**(code **)(iVar6 + 0x34))(*(int **)(iVar9 + 0x104),uVar5,iVar6,puVar10,uVar7);
      }
    }
    else {
      iVar6 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3);
      (**(code **)(**(int **)(iVar6 + 0x104) + 0x40))();
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 0xc);
  iVar3 = 0;
  do {
    uVar5 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3 + 0xc);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(uVar5,5);
    iVar6 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar3 + 0xc);
    (**(code **)(**(int **)(iVar6 + 0x104) + 0x40))();
    puVar2 = puStack_20;
    iVar3 = iVar3 + 1;
  } while (iVar3 != 0xc);
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_00425c66;
  puStack_24 = (undefined1 *)0x425c5e;
  System__LStrArrayClr_51F89FF7(&local_10,3,puVar2);
  return;
}

