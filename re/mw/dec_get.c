int * ImageLib_TCustomImageLibrary_GetSub_23EDC2EF(int param_1,int param_2,int param_3)
{
  int iVar1;
  int *piVar2;
                    /* 0x1b5bc  244  ImageLib.TCustomImageLibrary.GetSub@23EDC2EF */
  piVar2 = *(int **)(*(int *)(param_1 + 0x48) + param_2 * 8);
  if ((piVar2 == (int *)0x0) &&
     (iVar1 = *(int *)(*(int *)(param_1 + 0x48) + 4 + param_2 * 8), iVar1 != -1)) {
    piVar2 = *(int **)(*(int *)(param_1 + 0x48) + iVar1 * 8);
  }
  if (piVar2 == (int *)0x0) {
    piVar2 = (int *)0x0;
  }
  else {
    iVar1 = (**(code **)(*piVar2 + 0x68))();
    if (iVar1 == 0x100) {
      piVar2 = *(int **)(piVar2[0x10] + param_3 * 4);
    }
    else if (param_3 != 0) {
      return (int *)0x0;
    }
  }
  return piVar2;
}
