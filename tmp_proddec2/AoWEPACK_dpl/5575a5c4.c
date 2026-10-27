// /AoWEPACK.dpl : 5575a5c4

void AoWE_FmtTurnsLeftStr_0F6FDFF6(int param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x5a5c4  2441  AoWE.FmtTurnsLeftStr@0F6FDFF6 */
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_28 = &LAB_5575a676;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  if (param_1 == 1) {
    puStack_24 = &stack0xfffffffc;
    System_LoadResString_02614E1B(&AoWE_XTurnLeftRStr_8276FCC3,&local_c,0,param_2);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    local_14 = CONCAT31(local_14._1_3_,0x31);
    local_10 = CONCAT31(local_10._1_3_,2);
    AoWE_Format_D8F48273(local_8,&local_14,0,param_2);
  }
  else {
    SysUtils_IntToStr_0F6FDFF6(param_1,&local_8,0,param_2);
    local_14 = local_8;
    local_10 = CONCAT31(local_10._1_3_,0xb);
    System_LoadResString_02614E1B(&AoWE_XTurnsLeftRStr_8276FCC3,&local_18);
    AoWE_TranslateRStr_435799AA(local_18,&local_c);
    AoWE_Format_D8F48273(local_c,&local_14,0,param_2);
  }
  puVar1 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_5575a67d;
  puStack_28 = (undefined1 *)0x5575a668;
  System__LStrClr_40929B27(&local_18,uStack_2c,puVar1);
  puStack_28 = (undefined1 *)0x5575a675;
  System__LStrArrayClr_51F89FF7(&local_c,2);
  return;
}

