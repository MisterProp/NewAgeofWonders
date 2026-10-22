
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004508e0(void)

{
  undefined4 uVar1;
  undefined4 *in_FS_OFFSET;
  undefined1 auStack_10 [12];
  
  uVar1 = *in_FS_OFFSET;
  *in_FS_OFFSET = auStack_10;
  _DAT_0045b28c = _DAT_0045b28c + 1;
  *in_FS_OFFSET = uVar1;
  return;
}

