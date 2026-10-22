
undefined4 ImageLib_TImageSequenceList_GetImageSequence_23EDC2EF(int param_1,int param_2)

{
                    /* 0x1942c  335  ImageLib.TImageSequenceList.GetImageSequence@23EDC2EF */
  if ((-1 < param_2) && (param_2 < *(int *)(param_1 + 8))) {
    return *(undefined4 *)(*(int *)(param_1 + 4) + param_2 * 4);
  }
  return 0;
}

