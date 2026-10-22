
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00420dc0(void)

{
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_10;
  undefined1 *puStack_c;
  undefined1 *puStack_8;
  
  puStack_8 = &stack0xfffffffc;
  puStack_c = &LAB_00420df1;
  uStack_10 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_10;
  _DAT_0045b118 = _DAT_0045b118 + 1;
  if (_DAT_0045b118 == 0) {
    System__LStrClr_40929B27(&DAT_0045b114);
  }
  *in_FS_OFFSET = uStack_10;
  return;
}

