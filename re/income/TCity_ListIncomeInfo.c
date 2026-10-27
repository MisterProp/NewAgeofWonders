

void City_TCity_ListIncomeInfo_23EDC2EF(int param_1,int *param_2,int *param_3)



{

  undefined1 *puVar1;

  byte bVar2;

  int iVar3;

  int iVar4;

  undefined4 uVar5;

  undefined4 *in_FS_OFFSET;

  undefined4 *puVar6;

  undefined4 uStack_38;

  undefined1 *puStack_34;

  undefined1 *puStack_30;

  undefined4 local_20;

  undefined4 local_1c;

  undefined1 local_18;

  undefined4 local_14;

  undefined4 local_10;

  int *local_c;

  int *local_8;

  

                    /* 0xab9dc  6870  City.TCity.ListIncomeInfo@23EDC2EF */

  puStack_30 = &stack0xfffffffc;

  local_10 = 0;

  local_14 = 0;

  local_20 = 0;

  puStack_34 = &LAB_557abb83;

  uStack_38 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_38;

  local_c = param_3;

  local_8 = param_2;

  iVar3 = City_TCity_GetSize_23EDC2EF(param_1);

  System_LoadResString_02614E1B(PTR_AoWE_BaseIncomeRStr_04ECFCC5_558e9254,&local_14);

  AoWE_TranslateRStr_435799AA(local_14,&local_10);

  (**(code **)(*local_8 + 0x34))(local_8,local_10);

  SysUtils_IntToStr_0F6FDFF6(iVar3 * 5,&local_10);

  (**(code **)(*local_c + 0x34))(local_c,local_10);

  iVar4 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);

  iVar4 = iVar3 * 5 + iVar4 * 2;

  puVar6 = &local_10;

  uVar5 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);

  SysUtils_IntToStr_0F6FDFF6(uVar5,&local_14);

  local_1c = local_14;

  local_18 = 0xb;

  System_LoadResString_02614E1B(PTR_AoWE_XCropFieldsRStr_04ECFCC5_558e948c,&local_10);

  AoWE_TranslateRStr_435799AA(local_10,&local_20);

  SysUtils_Format_D8F48273(local_20,&local_1c,0,puVar6);

  (**(code **)(*local_8 + 0x34))(local_8,local_10);

  iVar3 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);

  SysUtils_IntToStr_0F6FDFF6(iVar3 * 2,&local_10);

  (**(code **)(*local_c + 0x34))(local_c,local_10);

  bVar2 = City_TCity_GetActiveAction_23EDC2EF(param_1);

  if (bVar2 < 2) {

    System_LoadResString_02614E1B(PTR_AoWE_ProducingMerchandiseRStr_04ECFCC5_558e8fc0,&local_14);

    AoWE_TranslateRStr_435799AA(local_14,&local_10);

    (**(code **)(*local_8 + 0x34))(local_8,local_10);

    uVar5 = System__ROUND_51F89FF7();

    SysUtils_IntToStr_0F6FDFF6(uVar5,&local_10);

    (**(code **)(*local_c + 0x34))(local_c,local_10);

    iVar3 = System__ROUND_51F89FF7();

    iVar4 = iVar4 + iVar3;

  }

  if ((*(byte *)(param_1 + 0x5c) & 2) != 0) {

    System_LoadResString_02614E1B(PTR_AoWE_GoldRushRStr_04ECFCC5_558e9228,&local_14);

    AoWE_TranslateRStr_435799AA(local_14,&local_10);

    (**(code **)(*local_8 + 0x34))(local_8,local_10);

    SysUtils_IntToStr_0F6FDFF6(iVar4,&local_10);

    (**(code **)(*local_c + 0x34))(local_c,local_10);

  }

  puVar1 = puStack_30;

  *in_FS_OFFSET = uStack_38;

  puStack_30 = &LAB_557abb8a;

  puStack_34 = (undefined1 *)0x557abb75;

  System__LStrClr_40929B27(&local_20,uStack_38,puVar1);

  puStack_34 = (undefined1 *)0x557abb82;

  System__LStrArrayClr_51F89FF7(&local_14,2);

  return;

}




