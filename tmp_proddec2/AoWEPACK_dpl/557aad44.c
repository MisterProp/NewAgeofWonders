// /AoWEPACK.dpl : 557aad44

uint City_TCity_FortifyCost_23EDC2EF(int param_1)

{
  int iVar1;
  
                    /* 0xaad44  6902  City.TCity.FortifyCost@23EDC2EF */
  if (*(char *)(param_1 + 0x4c) == '\0') {
    iVar1 = City_TCity_GetSize_23EDC2EF(param_1);
    return (uint)(byte)(&DAT_558e8de7)[iVar1];
  }
  if (*(char *)(param_1 + 0x4c) != '\x01') {
    return 0xffffffff;
  }
  iVar1 = City_TCity_GetSize_23EDC2EF(param_1);
  return (uint)(byte)(&DAT_558e8de3)[iVar1];
}

