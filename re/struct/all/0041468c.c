
void FUN_0041468c(int param_1,undefined4 param_2)

{
  char cVar1;
  
  cVar1 = AoWSetup_TSetupControl_GetRemote_23EDC2EF(*(undefined4 *)(param_1 + 0x48));
  if ((cVar1 != '\0') && (*(char *)(*(int *)(param_1 + 0x44) + 0x1ed) != '\0')) {
    aowDPlayComp_TGDPlay_SetPlayerName_23EDC2EF(*(undefined4 *)(param_1 + 0x44),param_2);
  }
  return;
}

