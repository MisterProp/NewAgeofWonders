
void HSEngine_TMapObjectResource_ShowComplexImage_23EDC2EF
               (int param_1,int param_2,int param_3,undefined4 param_4,int param_5,
               undefined4 param_6)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  
                    /* 0xa134  253  HSEngine.TMapObjectResource.ShowComplexImage@23EDC2EF */
  if (param_5 < *(int *)(*(int *)(param_1 + 0x24) + 8)) {
    iVar3 = *(int *)(*(int *)(*(int *)(param_1 + 0x24) + 4) + param_5 * 4);
  }
  else {
    iVar3 = 0;
  }
  for (; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0xc)) {
    iVar1 = ImageLib_TImageSequence_GetFrameIndex_23EDC2EF
                      (iVar3,*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x18) + 0x3c) + 0x40),
                       param_4);
    if (iVar1 != -1) {
      uVar4 = param_6;
      piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x10),iVar1)
      ;
      (**(code **)(*piVar2 + 0x84))
                (piVar2,param_2 + *(int *)(iVar3 + 0x24),param_3 + *(int *)(iVar3 + 0x28),uVar4);
    }
  }
  return;
}

