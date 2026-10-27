
undefined4 AoWE_TAoWHSMap_Random_23EDC2EF(int param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0x7827c  1398  AoWE.TAoWHSMap.Random@23EDC2EF */
  if ((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0x3c) & 8) == 0) {
    cVar1 = AoWE_TAoWHSMap_GetSynchronised_23EDC2EF(param_1);
    if (cVar1 == '\0') {
      Dialogs_ShowMessage_D425663E("Invalid AoWHSMap.Random use");
      return 0;
    }
    *(undefined4 *)System_RandSeed_A0665B6E_exref = *(undefined4 *)(param_1 + 0x230);
    uVar2 = System__RandInt_51F89FF7(param_2);
    *(undefined4 *)(param_1 + 0x230) = *(undefined4 *)System_RandSeed_A0665B6E_exref;
  }
  else {
    uVar2 = System__RandInt_51F89FF7(param_2);
  }
  return uVar2;
}

