
void FUN_00417ce4(int param_1,int param_2)

{
  int iVar1;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = AoWE_TLeader_GetSphereCount_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4));
  if (*(int *)(param_2 + 0xc) < iVar1) {
    AoWE_TLeader_DeleteSphereIndex_23EDC2EF
              (*(undefined4 *)(param_1 + 0x2e4),*(undefined4 *)(param_2 + 0xc));
  }
  FUN_00417658(param_1);
  return;
}

