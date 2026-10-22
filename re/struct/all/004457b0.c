
void FUN_004457b0(int param_1)

{
  undefined4 uVar1;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  uVar1 = AoWE_TFaceResourceList_PrevFace_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x74),
                     *(undefined4 *)(*(int *)(param_1 + 0x8c) + 0x14));
  *(undefined4 *)(*(int *)(param_1 + 0x8c) + 0x14) = uVar1;
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0xa4) = 0;
  return;
}

