
void FUN_00436218(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x144);
  if ((iVar1 != 0) && ((*(byte *)(iVar1 + 4) & 1) != 0)) {
    AoWE_TCombatViewer_SetPosition_23EDC2EF(iVar1,*(undefined4 *)(*(int *)(param_1 + 0xec) + 0x148))
    ;
    FUN_004352dc(param_1,1);
  }
  return;
}

