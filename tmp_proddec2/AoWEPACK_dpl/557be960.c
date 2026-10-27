// /AoWEPACK.dpl : 557be960

void PrdPlace_TProductionPlace_GetProductionName_23EDC2EF
               (int *param_1,int param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  char cVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
                    /* 0xbe960  7465  PrdPlace.TProductionPlace.GetProductionName@23EDC2EF */
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_557be9ec;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  if (*(int *)(param_2 + 8) == 10) {
    puStack_14 = &stack0xfffffffc;
    cVar2 = (**(code **)(*param_1 + 0x230))();
    if (cVar2 == '\b') {
      System_LoadResString_02614E1B(PTR_AoWE_ProduceMerchandiseRStr_04ECFCC5_558e90f0,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,param_3);
    }
    else {
      System_LoadResString_02614E1B(PTR_AoWE_GenerateExtraPowerRStr_04ECFCC5_558e8ee4,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,param_3);
    }
  }
  else {
    System_LoadResString_02614E1B(PTR_AoWE_UnknownRStr_8276FCC3_558e915c,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_3);
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_557be9f3;
  puStack_18 = (undefined1 *)0x557be9eb;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

