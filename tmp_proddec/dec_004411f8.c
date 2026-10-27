
void B4CBUpdate(int param_1,int param_2)

{
  int iVar1;
  
  if (*(int *)(param_2 + 0xc) < 4) {
    iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c),
                       *(int *)(param_2 + 0xc));
    *(bool *)(param_2 + 0xc0) = iVar1 != 0;
  }
  return;
}

