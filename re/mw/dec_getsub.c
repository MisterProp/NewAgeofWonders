int * ImageLib_TCustomImageLibrary_GetSubImage_23EDC2EF(int *param_1,int param_2,int param_3)
{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
                    /* 0x1b640  242  ImageLib.TCustomImageLibrary.GetSubImage@23EDC2EF */
  piVar4 = *(int **)(param_1[0x12] + param_2 * 8);
  if (piVar4 != (int *)0x0) {
    iVar2 = (**(code **)(*piVar4 + 0x68))();
    if (iVar2 == 0x100) {
      if ((param_3 < 0) || (piVar4[0x12] <= param_3)) {
        piVar4 = (int *)0x0;
      }
      else {
        piVar1 = *(int **)(piVar4[0x10] + param_3 * 4);
        (**(code **)(*piVar1 + 0xc))(piVar1,0);
        ImageLib_TImageNode_Delete_23EDC2EF(piVar4,param_3);
        if (piVar4[0x12] == 1) {
          uVar3 = ImageLib_TImageNode_GetImage_23EDC2EF(piVar4,0);
          *(undefined4 *)(param_1[0x12] + param_2 * 8) = uVar3;
          ImageLib_TImageNode_Delete_23EDC2EF(piVar4,0);
          System_TObject_Free_23EDC2EF(piVar4);
        }
        (**(code **)(*param_1 + 0x2c))(param_1,1);
        piVar4 = piVar1;
      }
    }
    else if (param_3 == 0) {
      (**(code **)(*piVar4 + 0xc))(piVar4,0);
      *(undefined4 *)(param_1[0x12] + param_2 * 8) = 0;
      (**(code **)(*param_1 + 0x2c))(param_1,1);
    }
    else {
      piVar4 = (int *)0x0;
    }
  }
  return piVar4;
}
