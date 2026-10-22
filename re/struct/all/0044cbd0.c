
undefined4 FUN_0044cbd0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if (*(int *)(param_1 + 0xac) != 0) {
    HSEngine_TMapContainer_GetFieldXYL_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x10),
               *(undefined4 *)(*(int *)(param_1 + 0xac) + 0x24));
    iVar1 = AoWE_TAoWMapField_GetArmyHS_23EDC2EF();
    if ((iVar1 != 0) &&
       (*(char *)(*(int *)(iVar1 + 0x1c) + 0x12) ==
        *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5))) {
      uVar2 = AoWE_TArmy_Hero_23EDC2EF
                        (*(undefined4 *)(iVar1 + 0x1c),
                         CONCAT31((int3)((uint)*(int *)(iVar1 + 0x1c) >> 8),0xff),
                         *AoWE_htAll_E90514D2_exref);
    }
  }
  return uVar2;
}

