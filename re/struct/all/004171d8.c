
void FUN_004171d8(int param_1)

{
  undefined4 extraout_EDX;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  AoWE_THero_SetUnitAttack_23EDC2EF
            (*(int *)(param_1 + 0x2e4),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x2e4) + 0x6a)) + 1);
  FUN_00415d08(param_1);
  return;
}

