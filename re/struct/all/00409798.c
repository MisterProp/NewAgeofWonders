
void FUN_00409798(int param_1,int param_2)

{
  undefined1 *puVar1;
  char cVar2;
  undefined4 uVar3;
  int *piVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_28 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_2c = &LAB_00409be8;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0x74),*(undefined4 *)(param_2 + 0x120));
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0x94),*(undefined4 *)(param_2 + 0x120));
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0x98),*(undefined4 *)(param_2 + 0x120));
  if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
    if ((*(char *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc0) + 0x24) !=
         *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) ||
       (*(int *)(*(int *)(param_1 + 0x74) + 0x120) < 0)) {
      (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
      goto LAB_00409bc5;
    }
    piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
    uVar3 = (**(code **)(*piVar4 + 0x18))(piVar4,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120));
    piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),uVar3);
    if (piVar4 != (int *)0x0) {
      cVar2 = (**(code **)(*piVar4 + 0x74))(piVar4,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0))
      ;
      if (cVar2 != '\0') {
        if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
          uVar5 = (**(code **)(*piVar4 + 0x98))
                            (piVar4,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0));
          (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))
                    (*(int **)(param_1 + 0x78),(uVar5 & 1) != 0);
        }
        else {
          piVar6 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
          uVar3 = (**(code **)(*piVar6 + 0x18))
                            (piVar6,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120));
          piVar6 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                     uVar3);
          cVar2 = (**(code **)(*piVar6 + 0x84))
                            (piVar6,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0));
          if (cVar2 == '\0') {
LAB_00409942:
            uVar3 = 0;
          }
          else {
            piVar6 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
            uVar3 = (**(code **)(*piVar6 + 0x18))
                              (piVar6,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120));
            piVar6 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                      (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                       uVar3);
            cVar2 = (**(code **)(*piVar6 + 0x84))
                              (piVar6,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0));
            if (cVar2 == '\x01') goto LAB_00409942;
            uVar3 = 1;
          }
          (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),uVar3);
        }
        if (*(char *)(*(int *)(param_1 + 0x78) + 0x75) != '\0') {
          (**(code **)(*piVar4 + 0x58))
                    (piVar4,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0),&local_14,&local_8);
          local_10 = local_14;
          local_c = CONCAT31(local_c._1_3_,0xb);
          System_LoadResString_02614E1B(AoWE_UseXRStr_04ECFCC5_exref,&local_8);
          AoWE_TranslateRStr_435799AA(local_8,&local_18);
          AoWE_Format_D8F48273(local_18,&local_10,0);
          AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
        }
        goto LAB_00409bc5;
      }
    }
    (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
    goto LAB_00409bc5;
  }
  if ((-1 < *(int *)(*(int *)(param_1 + 0x74) + 0x120)) &&
     (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) != 0)) {
    piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
    uVar3 = (**(code **)(*piVar4 + 0x18))(piVar4,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120));
    piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),uVar3);
    uVar5 = (**(code **)(*piVar4 + 0x98))
                      (piVar4,*(undefined4 *)
                               (*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x4c));
    if ((uVar5 & 2) != 0) {
      cVar2 = AoWE_TCombatObject_GetPlayer_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c));
      if ((cVar2 == *(char *)(*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref + 0xa5)) &&
         (0 < *(int *)(*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x5c))) {
        piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
        uVar3 = (**(code **)(*piVar4 + 0x18))
                          (piVar4,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120));
        piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                   uVar3);
        cVar2 = (**(code **)(*piVar4 + 0x84))
                          (piVar4,*(undefined4 *)
                                   (*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) +
                                   0x4c));
        if ((byte)(cVar2 - 3U) < 3) {
LAB_00409af3:
          cVar2 = (**(code **)(**(int **)(*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c)
                                         + 0x4c) + 0xd4))();
          if ((int)cVar2 !=
              *(int *)(*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x5c))
          goto LAB_00409bbb;
        }
        else {
          piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
          iVar7 = (**(code **)(*piVar4 + 0x18))
                            (piVar4,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120));
          if (iVar7 == 0x34) goto LAB_00409af3;
        }
        (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),1);
        piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
        iVar7 = *piVar4;
        uVar3 = (**(code **)(iVar7 + 0x18))
                          (piVar4,*(undefined4 *)(*(int *)(param_1 + 0x74) + 0x120),iVar7,&local_8);
        piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                   uVar3);
        (**(code **)(*piVar4 + 0x58))
                  (piVar4,*(undefined4 *)
                           (*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x4c),
                   &local_14);
        local_10 = local_14;
        local_c = CONCAT31(local_c._1_3_,0xb);
        System_LoadResString_02614E1B(AoWE_UseXRStr_04ECFCC5_exref,&local_8);
        AoWE_TranslateRStr_435799AA(local_8,&local_18);
        AoWE_Format_D8F48273(local_18,&local_10,0);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
        goto LAB_00409bc5;
      }
    }
  }
LAB_00409bbb:
  (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
LAB_00409bc5:
  puVar1 = puStack_28;
  *in_FS_OFFSET = uStack_30;
  puStack_28 = &LAB_00409bef;
  puStack_2c = (undefined1 *)0x409bdf;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar1);
  puStack_2c = (undefined1 *)0x409be7;
  System__LStrClr_40929B27(&local_8);
  return;
}

