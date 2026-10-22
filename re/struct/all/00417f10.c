
void FUN_00417f10(int param_1,undefined4 param_2,char param_3)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00417f9a;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  puVar1 = &stack0xfffffffc;
  if ((param_3 == '\x01') &&
     (puVar1 = &stack0xfffffffc, *(int *)(*(int *)(param_1 + 100) + 0x120) != -1)) {
    piVar3 = *(int **)(*(int *)(param_1 + 100) + 0x114);
    uVar2 = (**(code **)(*piVar3 + 0x18))(piVar3,*(undefined4 *)(*(int *)(param_1 + 100) + 0x120));
    piVar3 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                              (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),uVar2);
    puVar1 = puStack_14;
    if (piVar3 != (int *)0x0) {
      (**(code **)(*piVar3 + 0x58))(piVar3,0,&local_8);
      FUN_0040701c(*(undefined4 *)PTR_DAT_0045a3d4,local_8,piVar3[4]);
      puVar1 = puStack_14;
    }
  }
  puStack_14 = puVar1;
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00417fa1;
  puStack_18 = (undefined1 *)0x417f99;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

