
void ImageLib_TImageSequenceList_LoadFromStream_23EDC2EF(int param_1,int *param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iStack_18;
  char acStack_14 [4];
  
                    /* 0x190d8  339  ImageLib.TImageSequenceList.LoadFromStream@23EDC2EF */
  ImageLib_TImageSequenceList_DestroyItems_23EDC2EF(param_1);
  (**(code **)(*param_2 + 4))(param_2,&iStack_18,4);
  iVar2 = iStack_18;
  if (-1 < iStack_18 + -1) {
    do {
      (**(code **)(*param_2 + 4))(param_2,acStack_14,1);
      if (acStack_14[0] == '\0') {
        uVar1 = ImageLib_TImageSequence_Create_23EDC2EF
                          (&ImageLib__TImageSequence_4C8C0378,1,param_1);
        ImageLib_TImageSequence_LoadFromStream_23EDC2EF(uVar1,param_2);
        Classes_TList_Add_23EDC2EF(param_1,uVar1);
      }
      else {
        Classes_TList_Add_23EDC2EF(param_1,0);
      }
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  if (*(char *)(param_1 + 0x10) != '\0') {
    *(undefined1 *)(param_1 + 0x10) = 0;
    ImageLib_TImageSequenceList_LinkToIL_23EDC2EF(param_1);
  }
  return;
}

