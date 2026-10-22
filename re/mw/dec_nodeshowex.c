void ImageLib_TImageNode_ShowEx_23EDC2EF
               (int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5)
{
  int *piVar1;
  int iVar2;
  int iVar3;
                    /* 0xd888  667  ImageLib.TImageNode.ShowEx@23EDC2EF */
  iVar2 = *(int *)(param_1 + 0x48);
  if (-1 < iVar2 + -1) {
    iVar3 = 0;
    do {
      piVar1 = *(int **)(*(int *)(param_1 + 0x40) + iVar3 * 4);
      (**(code **)(*piVar1 + 0xa0))(piVar1,param_2,param_3,param_4,param_5);
      iVar3 = iVar3 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  return;
}
