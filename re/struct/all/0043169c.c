
void FUN_0043169c(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  int *piVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  puStack_34 = &stack0xfffffffc;
  local_24 = 0;
  local_14 = 0;
  puStack_38 = &LAB_00431818;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  *(undefined1 *)(*(int *)(param_1 + 0x5c) + 0xa4) = 0;
  local_10 = AoWE_TUnitControl_FindUnit_23EDC2EF
                       (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xfc),
                        *(undefined4 *)(*(int *)(param_1 + 0x94) + 0xc));
  (**(code **)(**(int **)(*(int *)(param_1 + 100) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x70) + 0x114) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x74) + 0x114) + 0x40))();
  local_1c = AoWE_TAbilityControl_GetAbility_23EDC2EF
                       (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),0x7d);
  iVar3 = (**(code **)(**(int **)(local_1c + 0x28) + 0x54))();
  if (-1 < iVar3 + -1) {
    local_18 = 0;
    local_20 = iVar3;
    do {
      piVar4 = (int *)AoWE_TConstructionControlList_GetConstructionControl_23EDC2EF
                                (*(undefined4 *)(local_1c + 0x28),local_18);
      (**(code **)(*piVar4 + 0x4c))(piVar4,local_10,&local_14,&local_8,&local_c);
      piVar1 = *(int **)(*(int *)(param_1 + 100) + 0x114);
      (**(code **)(*piVar1 + 0x38))(piVar1,local_14,piVar4);
      if (local_c == 0) {
        piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,&LAB_00431830);
      }
      else {
        SysUtils_IntToStr_0F6FDFF6(local_c,&local_24);
        piVar1 = *(int **)(*(int *)(param_1 + 0x70) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_24);
      }
      if (local_8 == 0) {
        piVar1 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,&LAB_00431830);
      }
      else {
        SysUtils_IntToStr_0F6FDFF6(local_8,&local_24);
        piVar1 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_24);
      }
      local_18 = local_18 + 1;
      local_20 = local_20 + -1;
    } while (local_20 != 0);
  }
  puVar2 = puStack_34;
  *in_FS_OFFSET = uStack_3c;
  puStack_34 = &LAB_0043181f;
  puStack_38 = (undefined1 *)0x43180f;
  System__LStrClr_40929B27(&local_24,uStack_3c,puVar2);
  puStack_38 = (undefined1 *)0x431817;
  System__LStrClr_40929B27(&local_14);
  return;
}

