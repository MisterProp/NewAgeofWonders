
void FUN_00449894(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 *in_FS_OFFSET;
  undefined *puVar9;
  undefined4 uStack_38;
  undefined1 *puStack_34;
  undefined1 *puStack_30;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  puStack_30 = &stack0xfffffffc;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_34 = &LAB_00449b11;
  uStack_38 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_38;
  local_8 = param_1;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x110) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(local_8 + 0x114) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(local_8 + 0x118) + 0x114) + 0x40))();
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  iVar4 = (**(code **)(**(int **)(iVar4 + 0x28) + 0x54))();
  if (-1 < iVar4 + -1) {
    local_c = 0;
    local_14 = iVar4;
    do {
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      piVar5 = (int *)AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar4 + 0x28),local_c);
      iVar4 = (**(code **)(*piVar5 + 0x54))();
      if (-1 < iVar4 + -1) {
        local_10 = 0;
        do {
          iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
          uVar7 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar6 + 0x28),local_c);
          piVar5 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF(uVar7,local_10);
          cVar3 = System__IsClass_51F89FF7(piVar5,AoWE__THero_228E24B5_exref);
          if (cVar3 != '\0') {
            (**(code **)(*piVar5 + 0xf8))(piVar5,&local_18);
            uVar7 = local_18;
            iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                               (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
            uVar8 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar6 + 0x28),local_c);
            piVar1 = *(int **)(*(int *)(local_8 + 0x110) + 0x114);
            (**(code **)(*piVar1 + 0x38))(piVar1,uVar7,uVar8);
            cVar3 = (**(code **)(*piVar5 + 0xe0))();
            SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_1c);
            puVar9 = &DAT_00449b28;
            uVar8 = local_1c;
            cVar3 = (**(code **)(*piVar5 + 0xd0))();
            SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_20);
            uVar7 = local_20;
            System__LStrCatN_51F89FF7(&local_18,3);
            piVar1 = *(int **)(*(int *)(local_8 + 0x114) + 0x114);
            (**(code **)(*piVar1 + 0x38))(piVar1,local_18,piVar5,uVar7,puVar9,uVar8);
            cVar3 = AoWE_THero_GetLevel_23EDC2EF(piVar5);
            SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_18);
            piVar5 = *(int **)(*(int *)(local_8 + 0x118) + 0x114);
            (**(code **)(*piVar5 + 0x34))(piVar5,local_18);
          }
          local_10 = local_10 + 1;
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
      }
      local_c = local_c + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  iVar4 = (**(code **)(**(int **)(*(int *)(local_8 + 0x110) + 0x114) + 0x14))();
  if ((0 < iVar4) && (*(int *)(*(int *)(local_8 + 0x110) + 0x120) == -1)) {
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(local_8 + 0x110),0);
    FUN_0044b100(local_8,*(undefined4 *)(local_8 + 0x110));
  }
  puVar2 = puStack_30;
  *in_FS_OFFSET = uStack_38;
  puStack_30 = &LAB_00449b18;
  puStack_34 = (undefined1 *)0x449b10;
  System__LStrArrayClr_51F89FF7(&local_20,3,puVar2);
  return;
}

