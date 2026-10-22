
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00451e78(void)

{
  undefined4 uVar1;
  undefined4 *in_FS_OFFSET;
  undefined1 auStack_10 [12];
  
  uVar1 = *in_FS_OFFSET;
  *in_FS_OFFSET = auStack_10;
  _DAT_0045b2a4 = _DAT_0045b2a4 + 1;
  *in_FS_OFFSET = uVar1;
  return;
}

