
void FUN_00417bcc(int param_1)

{
  char cVar1;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  cVar1 = AoWE_TLeader_CanAddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),5);
  if (cVar1 != '\0') {
    AoWE_TLeader_AddSphere_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4),5);
    FUN_00417658(param_1);
  }
  return;
}

