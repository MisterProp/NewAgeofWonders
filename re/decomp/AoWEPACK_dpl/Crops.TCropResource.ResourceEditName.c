
void Crops_TCropResource_ResourceEditName_23EDC2EF(undefined4 param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_8;
  
                    /* 0xa5fb8  6748  Crops.TCropResource.ResourceEditName@23EDC2EF */
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  puStack_14 = &LAB_557a5ffb;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  System_LoadResString_02614E1B(PTR_AoWE_CropRStr_04ECFCC5_558e8fbc,&local_8);
  AoWE_TranslateRStr_435799AA(local_8,param_2);
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_557a6002;
  puStack_14 = (undefined1 *)0x557a5ffa;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

