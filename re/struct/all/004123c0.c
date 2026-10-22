
void FUN_004123c0(int param_1)

{
  if ((*(char *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1ed) != '\0') &&
     (*(char *)(*(int *)(param_1 + 0xa0) + 0x75) != '\0')) {
    *(int *)(param_1 + 0x340) = *(int *)(param_1 + 0x340) + -1;
    if (*(int *)(param_1 + 0x340) < 1) {
      *(undefined4 *)(param_1 + 0x340) = 0x96;
      aowDPlayComp_TGDPlay_SendPingMessage_23EDC2EF
                (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44),0);
    }
    *(int *)(param_1 + 0x338) = *(int *)(param_1 + 0x338) + -1;
    if (*(int *)(param_1 + 0x338) < 0) {
      if (*(char *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x34) + 0x11) == '\x01') {
        AoWSetup_TSetupControl_ValidateMap_23EDC2EF();
        *(undefined4 *)(param_1 + 0x338) = 100;
      }
    }
  }
  return;
}

