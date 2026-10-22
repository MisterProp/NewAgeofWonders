
void FUN_0042d790(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(*(int *)(param_1 + 0xa0) + 0x120);
  if (-1 < iVar1) {
    iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0xa0) + 0x114) + 0x14))();
    if (iVar1 < iVar3) {
      piVar2 = *(int **)(*(int *)(param_1 + 0xa0) + 0x114);
      uVar4 = (**(code **)(*piVar2 + 0x18))
                        (piVar2,*(undefined4 *)(*(int *)(param_1 + 0xa0) + 0x120));
      AoWE_TGlobalMagicControl_RemoveEnchantment_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x188),
                 *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),uVar4);
    }
  }
  return;
}

