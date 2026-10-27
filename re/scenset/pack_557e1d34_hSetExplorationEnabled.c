// AoWSetup.TSetupControl.hSetExplorationEnabled (AoWEPACK.dpl 0x557e1d34)


void AoWSetup_TSetupControl_hSetExplorationEnabled_23EDC2EF(int param_1,char param_2)

{
  char cVar1;
  
                    /* 0xe1d34  8699  AoWSetup.TSetupControl.hSetExplorationEnabled@23EDC2EF */
  cVar1 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if (((cVar1 != '\0') && (param_2 != *(char *)(*(int *)(param_1 + 0x28) + 0x25))) &&
     (*(int *)(*(int *)(param_1 + 0x28) + 0x20) == 0)) {
    *(char *)(*(int *)(param_1 + 0x28) + 0x25) = param_2;
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

