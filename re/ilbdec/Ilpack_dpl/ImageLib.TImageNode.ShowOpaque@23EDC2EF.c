// /Ilpack.dpl : ImageLib.TImageNode.ShowOpaque@23EDC2EF

void ImageLib_TImageNode_ShowOpaque_23EDC2EF
               (int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
                    /* 0xd69c  671  ImageLib.TImageNode.ShowOpaque@23EDC2EF */
  iVar2 = *(int *)(param_1 + 0x48);
  if (-1 < iVar2 + -1) {
    iVar3 = 0;
    do {
      piVar1 = *(int **)(*(int *)(param_1 + 0x40) + iVar3 * 4);
      (**(code **)(*piVar1 + 0x8c))(piVar1,param_2,param_3,param_4);
      iVar3 = iVar3 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  return;
}

