
int ImageLib_TCustomImageLibrary_Get_23EDC2EF(int param_1,int param_2)

{
  int iVar1;
  
                    /* 0x1a8b8  268  ImageLib.TCustomImageLibrary.Get@23EDC2EF */
  iVar1 = *(int *)(*(int *)(param_1 + 0x48) + param_2 * 8);
  if (iVar1 == 0) {
    iVar1 = *(int *)(*(int *)(param_1 + 0x48) + 4 + param_2 * 8);
    if (iVar1 == -1) {
      iVar1 = 0;
    }
    else {
      iVar1 = *(int *)(*(int *)(param_1 + 0x48) + iVar1 * 8);
    }
  }
  return iVar1;
}

