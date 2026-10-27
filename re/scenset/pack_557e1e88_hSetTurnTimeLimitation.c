// AoWSetup.TSetupControl.hSetTurnTimeLimitation (AoWEPACK.dpl 0x557e1e88)


void AoWSetup_TSetupControl_hSetTurnTimeLimitation_23EDC2EF(int param_1,int param_2)

{
  char cVar1;
  
                    /* 0xe1e88  8692  AoWSetup.TSetupControl.hSetTurnTimeLimitation@23EDC2EF */
  cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if ((cVar1 != '\0') && (param_2 != *(int *)(*(int *)(param_1 + 0x28) + 0x34))) {
    *(int *)(*(int *)(param_1 + 0x28) + 0x34) = param_2;
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

