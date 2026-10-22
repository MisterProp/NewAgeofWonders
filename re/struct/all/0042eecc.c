
undefined4 FUN_0042eecc(int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)(param_1 + 0x230) == 0) {
    uVar1 = 1;
  }
  else {
    uVar1 = AoWE_TUnitControl_FindUnit_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xfc),
                       *(undefined4 *)(*(int *)(param_1 + 0x230) + 0xc));
    *(undefined4 *)(param_1 + 0x22c) = uVar1;
    if ((*(int *)(param_1 + 0x22c) != 0) &&
       ((*(byte *)(*(int *)(param_1 + 0x22c) + 0x25) & 0x10) == 0)) {
      return 1;
    }
    uVar1 = 0;
  }
  return uVar1;
}

