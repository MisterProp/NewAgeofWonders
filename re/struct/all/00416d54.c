
void FUN_00416d54(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(param_1 + 0x2e4);
  uVar2 = AoWE_TFaceResourceList_NextFace_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x74),
                     *(undefined4 *)(iVar1 + 0x3c));
  AoWE_THero_SetFaceIndex_23EDC2EF(iVar1,uVar2);
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a28c + 0x44) + 0xa4) = 0;
  return;
}

