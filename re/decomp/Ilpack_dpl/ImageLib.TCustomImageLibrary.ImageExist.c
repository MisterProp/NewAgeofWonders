
undefined4 ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  bool bVar3;
  
                    /* 0x1a91c  266  ImageLib.TCustomImageLibrary.ImageExist@23EDC2EF */
  if ((param_2 < *(int *)(param_1 + 0x44)) && (-1 < param_2)) {
    bVar3 = *(int *)(*(int *)(param_1 + 0x48) + param_2 * 8) != 0;
    uVar2 = CONCAT31((int3)((uint)*(int *)(param_1 + 0x48) >> 8),bVar3);
    if (!bVar3) {
      iVar1 = *(int *)(*(int *)(param_1 + 0x48) + 4 + param_2 * 8);
      if ((iVar1 < 0) || (*(int *)(param_1 + 0x44) <= iVar1)) {
        return 0;
      }
      uVar2 = CONCAT31((int3)((uint)*(int *)(param_1 + 0x48) >> 8),
                       *(int *)(*(int *)(param_1 + 0x48) + iVar1 * 8) != 0);
    }
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

