// AoWSetup.TSetupControl.hSetCustomizeLeaders (AoWEPACK.dpl 0x557e1e28)


void AoWSetup_TSetupControl_hSetCustomizeLeaders_23EDC2EF(int param_1,char param_2)

{
  char cVar1;
  
                    /* 0xe1e28  8694  AoWSetup.TSetupControl.hSetCustomizeLeaders@23EDC2EF */
  cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if (((cVar1 != '\0') && (param_2 != *(char *)(*(int *)(param_1 + 0x28) + 0x2a))) &&
     (*(int *)(*(int *)(param_1 + 0x28) + 0x20) == 0)) {
    *(char *)(*(int *)(param_1 + 0x28) + 0x2a) = param_2;
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

