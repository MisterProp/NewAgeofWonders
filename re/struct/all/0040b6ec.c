
void FUN_0040b6ec(int param_1,int param_2)

{
  if (*(char *)(param_2 + 0xb0) == '\0') {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,2);
  }
  else {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,1);
  }
  AoWReg_TAoWRegistry_SetAllowResolutionSwitches_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),
             *(undefined1 *)(*(int *)(param_1 + 0x74) + 0xb0));
  return;
}

