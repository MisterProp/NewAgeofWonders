
void FUN_00446dcc(int param_1)

{
  undefined4 extraout_EDX;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  AoWE_THero_SetUnitDefense_23EDC2EF
            (*(int *)(param_1 + 0x1dc),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x1dc) + 0x6b)) + 1);
  FUN_004467e4(param_1);
  return;
}

