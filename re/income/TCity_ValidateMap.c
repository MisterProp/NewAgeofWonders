

void City_TCity_ValidateMap_23EDC2EF(int param_1,int *param_2)



{

  undefined1 *puVar1;

  int iVar2;

  undefined4 *in_FS_OFFSET;

  undefined4 uVar3;

  undefined *puVar4;

  undefined4 uVar5;

  undefined4 uStack_2c;

  undefined1 *puStack_28;

  undefined1 *puStack_24;

  undefined4 local_14;

  undefined4 local_10;

  undefined4 local_c;

  undefined4 local_8;

  

                    /* 0xa96f4  6936  City.TCity.ValidateMap@23EDC2EF */

  puStack_24 = &stack0xfffffffc;

  local_8 = 0;

  local_c = 0;

  local_10 = 0;

  local_14 = 0;

  puStack_28 = &LAB_557a97b9;

  uStack_2c = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_2c;

  AoWE_TStructure_ValidateMap_23EDC2EF(param_1,param_2);

  System_LoadResString_02614E1B(PTR_AoWE_NonameRStr_04ECFCC5_558e9388,&local_c);

  AoWE_TranslateRStr_435799AA(local_c,&local_8);

  iVar2 = SysUtils_CompareText_8779A639(*(undefined4 *)(param_1 + 0x48),local_8);

  if ((iVar2 == 0) || (*(int *)(param_1 + 0x48) == 0)) {

    System_LoadResString_02614E1B(PTR_AoWE_WarningRStr_8276FCC3_558e9528,&local_10);

    AoWE_TranslateRStr_435799AA(local_10,&local_c);

    puVar4 = &DAT_557a97d4;

    uVar5 = local_c;

    System_LoadResString_02614E1B(PTR_AoWE_NoNameSpecifiedRStr_04ECFCC5_558e939c,&local_14);

    AoWE_TranslateRStr_435799AA(local_14,&local_10);

    uVar3 = local_10;

    System__LStrCatN_51F89FF7(&local_8,3);

    (**(code **)(*param_2 + 0x34))(param_2,local_8,*param_2,uVar3,puVar4,uVar5);

  }

  puVar1 = puStack_24;

  *in_FS_OFFSET = uStack_2c;

  puStack_24 = &LAB_557a97c0;

  puStack_28 = (undefined1 *)0x557a97b8;

  System__LStrArrayClr_51F89FF7(&local_14,4,puVar1);

  return;

}




