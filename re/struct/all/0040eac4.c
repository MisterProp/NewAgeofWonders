
void FUN_0040eac4(void)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_14 = &LAB_0040eb65;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  FUN_0040e7e4(DAT_0045b090);
  piVar1 = *(int **)(*(int *)(DAT_0045b090 + 0x80) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,0);
  System_LoadResString_02614E1B(AoWE_DemoVersionMessageRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  piVar1 = *(int **)(*(int *)(DAT_0045b090 + 0x80) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  (**(code **)(**(int **)(DAT_0045b090 + 0x70) + 0x6c))(*(int **)(DAT_0045b090 + 0x70),1);
  (**(code **)(**(int **)(DAT_0045b090 + 0x44) + 0x6c))(*(int **)(DAT_0045b090 + 0x44),1);
  puVar2 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_0040eb6c;
  puStack_14 = (undefined1 *)0x40eb64;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

