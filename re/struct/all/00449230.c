
void FUN_00449230(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  uint uVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_10 = 0;
  puStack_24 = &LAB_004496fa;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x74) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x78) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x7c) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x80) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x84) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x88) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0xbc) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0xb4) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0xac) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x8c) + 0x114) + 0x40))();
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  iVar4 = (**(code **)(**(int **)(iVar4 + 0x1c) + 0x54))();
  if (-1 < iVar4 + -1) {
    local_8 = 0;
    local_c = iVar4;
    do {
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      piVar5 = (int *)AoWE_TPlayerStructureList_GetStructure_23EDC2EF
                                (*(undefined4 *)(iVar4 + 0x1c),local_8);
      cVar3 = System__IsClass_51F89FF7(piVar5,City__TCity_01CDE5D7_exref);
      if (cVar3 != '\0') {
        (**(code **)(*piVar5 + 0x148))(piVar5,&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
        (**(code **)(*piVar1 + 0x38))(piVar1,local_10,piVar5);
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)
                    (AoWE_RaceNamesPluralRStr_B11890CE_exref + *(char *)((int)piVar5 + 0x45) * 4),
                   &local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x78) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
        SysUtils_IntToStr_0F6FDFF6((char)piVar5[0xf],&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
        SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)((int)piVar5 + 0x4d),&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x80) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
        if ((char)piVar5[0x13] == '\x01') {
          piVar1 = *(int **)(*(int *)(param_1 + 0x84) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449710);
        }
        else if ((char)piVar5[0x13] == '\x02') {
          piVar1 = *(int **)(*(int *)(param_1 + 0x84) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_0044971c);
        }
        else {
          piVar1 = *(int **)(*(int *)(param_1 + 0x84) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449728);
        }
        cVar3 = City_TCity_GetCivilStatus_23EDC2EF(piVar5);
        if (cVar3 == '\x01') {
          piVar1 = *(int **)(*(int *)(param_1 + 0xbc) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449710);
        }
        else if (cVar3 == '\x02') {
          piVar1 = *(int **)(*(int *)(param_1 + 0xbc) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_0044971c);
        }
        else if (cVar3 == '\x03') {
          piVar1 = *(int **)(*(int *)(param_1 + 0xbc) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449734);
        }
        else if (cVar3 == '\x04') {
          piVar1 = *(int **)(*(int *)(param_1 + 0xbc) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449740);
        }
        else {
          piVar1 = *(int **)(*(int *)(param_1 + 0xbc) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449728);
        }
        uVar6 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),
                           (int)*(char *)((int)piVar5 + 0x45));
        uVar7 = AoWE_TRace_GetPlayerRelation_23EDC2EF
                          (uVar6,CONCAT31((int3)((uint)*(int *)AoWE_AoWHSMap_6B957A58_exref >> 8),
                                          *(undefined1 *)
                                           (*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)));
        switch(uVar7 & 0x7f) {
        case 0:
          piVar1 = *(int **)(*(int *)(param_1 + 0xb4) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_0044974c);
          break;
        case 1:
          piVar1 = *(int **)(*(int *)(param_1 + 0xb4) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449710);
          break;
        case 2:
          piVar1 = *(int **)(*(int *)(param_1 + 0xb4) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_0044971c);
          break;
        case 3:
          piVar1 = *(int **)(*(int *)(param_1 + 0xb4) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449734);
          break;
        case 4:
          piVar1 = *(int **)(*(int *)(param_1 + 0xb4) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449740);
          break;
        default:
          piVar1 = *(int **)(*(int *)(param_1 + 0xb4) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449728);
        }
        uVar7 = City_TCity_GetActiveAction_23EDC2EF(piVar5);
        switch(uVar7 & 0x7f) {
        default:
          piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449728);
          break;
        case 2:
          piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_0044971c);
          break;
        case 3:
          if ((char)piVar5[0x13] == '\x01') {
            piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
            (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449710);
          }
          else {
            piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
            (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_0044974c);
          }
          break;
        case 4:
          piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449740);
          break;
        case 5:
          piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449734);
          break;
        case 7:
        case 8:
          piVar1 = *(int **)(*(int *)(param_1 + 0xac) + 0x114);
          (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449758);
        }
        piVar1 = *(int **)(*(int *)(param_1 + 0x8c) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,&DAT_00449728);
        uVar6 = City_TCity_GetSize_23EDC2EF(piVar5);
        SysUtils_IntToStr_0F6FDFF6(uVar6,&local_10);
        piVar5 = *(int **)(*(int *)(param_1 + 0x88) + 0x114);
        (**(code **)(*piVar5 + 0x34))(piVar5,local_10);
      }
      local_8 = local_8 + 1;
      local_c = local_c + -1;
    } while (local_c != 0);
  }
  puVar2 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_00449701;
  puStack_24 = (undefined1 *)0x4496f9;
  System__LStrClr_40929B27(&local_10,uStack_28,puVar2);
  return;
}

