// /AoW.exe : 0x0042b588

void FUN_0042b588(int param_1)

{
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
  AoWSetup_TSetupControl_SetupHost_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x48),0,0,0);
  if (*(int *)(param_1 + 0xec) != 0) {
    AoWSetup_TSetupControl_hPickMap_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x48),*(undefined4 *)(param_1 + 0xec));
  }
  FUN_00410bd8(*(undefined4 *)PTR_DAT_0045a30c,*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x48));
  return;
}

