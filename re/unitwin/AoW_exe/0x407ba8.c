// /AoW.exe : 0x407ba8

void FUN_00407ba8(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  bool bVar1;
  char cVar2;
  undefined4 uVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  
  piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
  uVar3 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
  piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),uVar3);
  if (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0) {
    if (((piVar4 != (int *)0x0) && (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc0) != 0)) &&
       (*(char *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc0) + 0x24) ==
        *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5))) {
      uVar5 = (**(code **)(*piVar4 + 0x98))(piVar4,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0))
      ;
      if ((uVar5 & 1) != 0) {
        bVar1 = true;
        goto LAB_00407d8d;
      }
    }
    bVar1 = false;
    goto LAB_00407d8d;
  }
  if ((piVar4 != (int *)0x0) && (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) != 0)) {
    piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
    uVar3 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
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
        uVar3 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
        piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                   uVar3);
        cVar2 = (**(code **)(*piVar4 + 0x84))
                          (piVar4,*(undefined4 *)
                                   (*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) +
                                   0x4c));
        if ((byte)(cVar2 - 3U) < 3) {
LAB_00407d53:
          cVar2 = (**(code **)(**(int **)(*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c)
                                         + 0x4c) + 0xd4))();
          if ((int)cVar2 !=
              *(int *)(*(int *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xc4) + 0x1c) + 0x5c))
          goto LAB_00407d87;
        }
        else {
          piVar4 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
          iVar6 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
          if (iVar6 == 0x34) goto LAB_00407d53;
        }
        bVar1 = true;
        goto LAB_00407d8d;
      }
    }
  }
LAB_00407d87:
  bVar1 = false;
LAB_00407d8d:
  if (bVar1) {
    (**(code **)(*param_2 + 0x70))(param_2,0xdffeff);
  }
  else {
    (**(code **)(*param_2 + 0x70))(param_2,0x8fa9a0);
  }
  return;
}

