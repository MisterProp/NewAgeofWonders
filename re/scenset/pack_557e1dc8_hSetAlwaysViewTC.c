// AoWSetup.TSetupControl.hSetAlwaysViewTC (AoWEPACK.dpl 0x557e1dc8)


void AoWSetup_TSetupControl_hSetAlwaysViewTC_23EDC2EF(int param_1,char param_2)

{
  char cVar1;
  
                    /* 0xe1dc8  8696  AoWSetup.TSetupControl.hSetAlwaysViewTC@23EDC2EF */
  cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if ((cVar1 != '\0') && (param_2 != *(char *)(*(int *)(param_1 + 0x28) + 0x3e))) {
    *(char *)(*(int *)(param_1 + 0x28) + 0x3e) = param_2;
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

