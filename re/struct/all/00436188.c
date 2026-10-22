
void FUN_00436188(int param_1)

{
  int iVar1;
  int iVar2;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(param_1 + 0x144);
  if ((iVar1 != 0) && ((*(byte *)(iVar1 + 4) & 1) != 0)) {
    iVar2 = AoWE_TCombatViewer_GetEndPosition_23EDC2EF(iVar1);
    AoWE_TCombatViewer_SetPosition_23EDC2EF(iVar1,iVar2 + 1);
    FUN_004352dc(param_1,3);
  }
  return;
}

