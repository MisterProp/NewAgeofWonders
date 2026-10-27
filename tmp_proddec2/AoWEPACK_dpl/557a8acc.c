// /AoWEPACK.dpl : 557a8acc

void City_TCityProductionControl_GetProductionName_23EDC2EF
               (undefined4 param_1,int param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  uint uVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0xa8acc  6957  City.TCityProductionControl.GetProductionName@23EDC2EF */
  puStack_28 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_2c = &LAB_557a8bfd;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  AoWE_TProductionControl_GetProductionName_23EDC2EF(param_1,param_2,param_3);
  uVar2 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(param_2 + 8));
  switch(uVar2 & 0x7f) {
  case 1:
    System_LoadResString_02614E1B(PTR_AoWE_ProduceMerchandiseRStr_04ECFCC5_558e90f0,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_3);
    break;
  case 2:
    System_LoadResString_02614E1B(PTR_AoWE_UpgradeRStr_04ECFCC5_558e8fc4,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_3);
    break;
  case 3:
    System_LoadResString_02614E1B(PTR_AoWE_FortifyRStr_04ECFCC5_558e90a8,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_3);
    break;
  case 4:
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)
                (PTR_AoWE_RaceNamesPluralRStr_B11890CE_558e9480 + *(int *)(param_2 + 0xc) * 4),
               &local_8);
    local_10 = local_8;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(PTR_AoWE_MigrateToXRStr_04ECFCC5_558e8ec4,&local_18);
    AoWE_TranslateRStr_435799AA(local_18,&local_14);
    SysUtils_Format_D8F48273(local_14,&local_10,0,param_3);
    break;
  case 5:
    System_LoadResString_02614E1B(PTR_AoWE_LootRStr_04ECFCC5_558e93b0,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_3);
  }
  puVar1 = puStack_28;
  *in_FS_OFFSET = uStack_30;
  puStack_28 = &LAB_557a8c04;
  puStack_2c = (undefined1 *)0x557a8bf4;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar1);
  puStack_2c = (undefined1 *)0x557a8bfc;
  System__LStrClr_40929B27(&local_8);
  return;
}

