
void FUN_0041610c(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  undefined4 *in_FS_OFFSET;
  int iStackY_48;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_1c;
  undefined4 local_18;
  int local_14;
  int *local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_2c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = (int *)0x0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  puStack_30 = &LAB_00416472;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  (**(code **)(**(int **)(*(int *)(param_1 + 100) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x23c) + 0x114) + 0x40))();
  iVar3 = AoWE_TAbilityControl_GetCount_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80));
  if (-1 < iVar3 + -1) {
    iVar7 = 0;
    local_14 = iVar3;
    do {
      iVar3 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),iVar7);
      if (iVar3 != 0) {
        iVar3 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),iVar7);
        if ((*(ushort *)(iVar3 + 0x20) & 0x180) != 0) {
          piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                     iVar7);
          cVar2 = (**(code **)(*piVar4 + 0xc4))(piVar4,*(undefined4 *)(param_1 + 0x2e4));
          if (cVar2 != '\0') {
            piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                      (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                       iVar7);
            (**(code **)(*piVar4 + 0xcc))(piVar4,*(undefined4 *)(param_1 + 0x2e4),&local_18);
            piVar4 = *(int **)(*(int *)(param_1 + 100) + 0x114);
            (**(code **)(*piVar4 + 0x38))(piVar4,local_18,iVar7);
            piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                      (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                       iVar7);
            uVar5 = (**(code **)(*piVar4 + 200))(piVar4,*(undefined4 *)(param_1 + 0x2e4));
            SysUtils_IntToStr_0F6FDFF6(uVar5,&local_18);
            piVar4 = *(int **)(*(int *)(param_1 + 0x23c) + 0x114);
            (**(code **)(*piVar4 + 0x34))(piVar4,local_18);
          }
        }
      }
      iVar7 = iVar7 + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  (**(code **)(**(int **)(*(int *)(param_1 + 0x44) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x238) + 0x114) + 0x40))();
  iVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0x54))();
  if (-1 < iVar3 + -1) {
    iVar7 = 0;
    local_14 = iVar3;
    do {
      local_10 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                   iVar7);
      cVar2 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0x4c))(*(int **)(param_1 + 0x2e4),iVar7);
      if (cVar2 != '\0') {
        piVar4 = *(int **)(*(int *)(*(int *)(param_1 + 0x2e4) + 0x40) + 0x2c);
        cVar2 = (**(code **)(*piVar4 + 0x4c))(piVar4,iVar7);
        if ((cVar2 != '\0') || ((*(ushort *)(local_10 + 8) & 0x180) != 0)) {
          piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                     iVar7);
          (**(code **)(*piVar4 + 0x58))(piVar4,*(undefined4 *)(param_1 + 0x2e4),&local_8);
          piVar4 = *(int **)(*(int *)(*(int *)(param_1 + 0x2e4) + 0x40) + 0x2c);
          cVar2 = (**(code **)(*piVar4 + 0x4c))(piVar4,iVar7);
          if (cVar2 == '\0') {
LAB_0041639e:
            uVar5 = (**(code **)(*local_10 + 0x80))(local_10,*(undefined4 *)(param_1 + 0x2e4));
            iVar3 = (**(code **)(*local_10 + 0x80))
                              (local_10,*(undefined4 *)
                                         (*(int *)(*(int *)(param_1 + 0x2e4) + 0x40) + 0x2c),
                               *local_10,uVar5);
            SysUtils_IntToStr_0F6FDFF6(iStackY_48 - iVar3,&local_c);
          }
          else {
            piVar4 = *(int **)(*(int *)(*(int *)(param_1 + 0x2e4) + 0x40) + 0x2c);
            iVar3 = (**(code **)(*piVar4 + 0x84))(piVar4,iVar7);
            iVar6 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0x144))
                              (*(int **)(param_1 + 0x2e4),iVar7);
            if (iVar3 != iVar6) goto LAB_0041639e;
            System_LoadResString_02614E1B(AoWE_FixedRStr_04ECFCC5_exref,&local_1c);
            AoWE_TranslateRStr_435799AA(local_1c,&local_18);
            System__LStrCatN_51F89FF7(&local_8,4);
            System__LStrLAsg_51F89FF7(&local_c,&LAB_004164a8);
          }
          piVar4 = *(int **)(*(int *)(param_1 + 0x238) + 0x114);
          (**(code **)(*piVar4 + 0x34))(piVar4,local_c);
          piVar4 = *(int **)(*(int *)(param_1 + 0x44) + 0x114);
          iStackY_48 = 0x4163fa;
          (**(code **)(*piVar4 + 0x38))(piVar4,local_8,iVar7);
        }
      }
      iVar7 = iVar7 + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 100) + 0x114) + 0x14))();
  AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF(*(undefined4 *)(param_1 + 0x1c0),iVar3 + -1);
  iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0x44) + 0x114) + 0x14))();
  AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF(*(undefined4 *)(param_1 + 0x1c4),iVar3 + -1);
  FUN_00416064(param_1);
  FUN_00415fbc(param_1);
  puVar1 = puStack_2c;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_00416479;
  puStack_30 = (undefined1 *)0x416464;
  System__LStrArrayClr_51F89FF7(&local_1c,2,puVar1);
  puStack_30 = (undefined1 *)0x416471;
  System__LStrArrayClr_51F89FF7(&local_c,2);
  return;
}

