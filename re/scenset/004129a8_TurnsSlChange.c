// TScenSetup.TurnsSlChange  (0x004129a8) - decompiled via GhidraMCP


void FUN_004129a8(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(*(int *)(param_1 + 0xd0) + 0x148);
  if (iVar1 == 0) {
    AoWSetup_TSetupControl_hSetTurnLimitation_23EDC2EF(*(undefined4 *)(param_1 + 0x33c),0);
  }
  else {
    AoWSetup_TSetupControl_hSetTurnLimitation_23EDC2EF(*(undefined4 *)(param_1 + 0x33c),iVar1 * 10);
  }
  FUN_004128b4(param_1);
  return;
}

