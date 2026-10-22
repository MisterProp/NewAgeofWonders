
void FUN_0044eddc(void)

{
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  
  puStack_14 = &stack0xfffffffc;
  puStack_18 = &LAB_0044ee4b;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_0045660c(*(undefined4 *)PTR_DAT_0045a420);
  FUN_00455a34(*(undefined4 *)PTR_DAT_0045a420);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x28))
            (*(int **)(*(int *)PTR_DAT_0045a5bc + 0x48),0);
  aowDPlayComp_TGDPlay_DisConnect_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44));
  if (*(char *)(*(int *)PTR_DAT_0045a454 + 0x4c) != '\0') {
    Forms_TCustomForm_Close_23EDC2EF(*(undefined4 *)PTR_DAT_0045a168);
  }
  *in_FS_OFFSET = uStack_1c;
  return;
}

