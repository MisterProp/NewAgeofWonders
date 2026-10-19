
int AoWE_TArmy_SelectionCount_23EDC2EF(int param_1,byte param_2)

{
  int iVar1;
  uint uVar2;
  
                    /* 0x8d450  408  AoWE.TArmy.SelectionCount@23EDC2EF */
  param_2 = param_2 & (&DAT_558e8dcc)[*(int *)(*(int *)(param_1 + 8) + 8)];
  uVar2 = (uint)param_2;
  iVar1 = 0;
  while (param_2 != 0) {
    if ((uVar2 & 1) != 0) {
      iVar1 = iVar1 + 1;
    }
    uVar2 = uVar2 >> 1;
    param_2 = (byte)uVar2;
  }
  return iVar1;
}

