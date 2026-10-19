
void FUN_004060d8(int param_1,int param_2)

{
  FUN_00406130(param_1);
  AoWE_TSelectedArmy_SetSelection_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8),
             *(undefined1 *)(param_1 + 0x178));
  FUN_004062a0(param_1);
  if (*(char *)(param_2 + 0xb0) != '\0') {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,1);
    return;
  }
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,2);
  return;
}

