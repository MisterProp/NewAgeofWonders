// AoWSetup.TSetupControl.hSetAlliedVictory (AoWEPACK.dpl 0x557e1df4)


void AoWSetup_TSetupControl_hSetAlliedVictory_23EDC2EF(int param_1,char param_2)

{
  char cVar1;
  
                    /* 0xe1df4  8695  AoWSetup.TSetupControl.hSetAlliedVictory@23EDC2EF */
  cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if (((cVar1 != '\0') && (param_2 != *(char *)(*(int *)(param_1 + 0x28) + 0x27))) &&
     (*(int *)(*(int *)(param_1 + 0x28) + 0x20) == 0)) {
    *(char *)(*(int *)(param_1 + 0x28) + 0x27) = param_2;
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

