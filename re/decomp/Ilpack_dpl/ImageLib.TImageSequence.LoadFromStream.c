
void ImageLib_TImageSequence_LoadFromStream_23EDC2EF(int param_1,int *param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uStack_10;
  
  uStack_10 = param_3;
                    /* 0x18a60  348  ImageLib.TImageSequence.LoadFromStream@23EDC2EF */
  ImageLib_TImageSequence_ReleaseIL_23EDC2EF(param_1);
  FUN_5520d1e0(param_1 + 0x14,param_2);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 8,1);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x18,1);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x19,1);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x20,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x24,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x28,4);
  if ((byte)(*(char *)(param_1 + 0x19) - 4U) < 2) {
    if (*(int *)(param_1 + 0x1c) == 0) {
      uVar1 = System_TObject_Create_23EDC2EF(&ImageLib__TFrameTableList_39E40F71,1);
      *(undefined4 *)(param_1 + 0x1c) = uVar1;
    }
    ImageLib_TFrameTableList_LoadFromStream_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
    uVar1 = System__RandInt_51F89FF7(*(undefined4 *)(*(int *)(param_1 + 0x1c) + 8));
    *(undefined4 *)(param_1 + 0x20) = uVar1;
  }
  (**(code **)(*param_2 + 4))(param_2,&uStack_10,1);
  if ((char)uStack_10 != '\0') {
    uVar1 = ImageLib_TImageSequence_Create_23EDC2EF
                      (&ImageLib__TImageSequence_4C8C0378,1,*(undefined4 *)(param_1 + 4));
    *(undefined4 *)(param_1 + 0xc) = uVar1;
    ImageLib_TImageSequence_LoadFromStream_23EDC2EF(uVar1,param_2);
  }
  return;
}

