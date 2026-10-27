// AoWSetup.TSetupControl.hSetPlayMode (AoWEPACK.dpl 0x557e2028)


void AoWSetup_TSetupControl_hSetPlayMode_23EDC2EF(int param_1,char param_2)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
                    /* 0xe2028  8690  AoWSetup.TSetupControl.hSetPlayMode@23EDC2EF */
  cVar2 = AoWSetup_TSetupControl_GetHost_23EDC2EF(param_1);
  if ((cVar2 != '\0') && (param_2 != *(char *)(*(int *)(param_1 + 0x28) + 0x24))) {
    *(char *)(*(int *)(param_1 + 0x28) + 0x24) = param_2;
    cVar2 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(param_1);
    if ((cVar2 == '\0') && (param_2 == '\x02')) {
      bVar1 = true;
      iVar3 = (**(code **)(**(int **)(param_1 + 0x28) + 0x54))();
      if (-1 < iVar3 + -1) {
        iVar5 = 0;
        do {
          iVar4 = AoWSetup_TSetupSettings_GetSettings_23EDC2EF
                            (*(undefined4 *)(param_1 + 0x28),iVar5);
          if (*(char *)(iVar4 + 8) == '\0') {
            if (!bVar1) {
              AoWSetup_TSetupControl_ResetPlayerSettings_23EDC2EF(param_1,iVar5);
            }
            bVar1 = false;
          }
          iVar5 = iVar5 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
    }
    AoWSetup_TSetupControl_UpdateSettings_23EDC2EF(param_1);
  }
  return;
}

