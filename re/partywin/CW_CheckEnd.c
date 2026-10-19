
void FUN_004066bc(int param_1,undefined4 param_2,char param_3)

{
  if (param_3 == '\x01') {
    FUN_00406130(param_1);
    AoWE_TSelectedArmy_SetSelection_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8),
               ~*(byte *)(param_1 + 0x178));
    FUN_004062a0(param_1);
  }
  return;
}

