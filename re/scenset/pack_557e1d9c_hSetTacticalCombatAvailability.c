// AoWSetup.TSetupControl.hSetTacticalCombatAvailability (AoWEPACK.dpl 0x557e1d9c)


void AoWSetup_TSetupControl_hSetTacticalCombatAvailability_23EDC2EF(int param_1,char param_2)

{
  char cVar1;
  
                    /* 0xe1d9c  8697  AoWSetup.TSetupControl.hSetTacticalCombatAvailability@23EDC2EF
                        */
  cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if ((cVar1 != '\0') && (param_2 != *(char *)(*(int *)(param_1 + 0x28) + 0x3d))) {
    *(char *)(*(int *)(param_1 + 0x28) + 0x3d) = param_2;
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

