
int ImageLib_TImageSequence_GetUnloopedFrameIndex_23EDC2EF(int param_1,int param_2)

{
                    /* 0x188a8  352  ImageLib.TImageSequence.GetUnloopedFrameIndex@23EDC2EF */
  if (param_2 < 0) {
    param_2 = -1;
  }
  else {
    if (*(byte *)(param_1 + 0x19) == 0) {
      return *(int *)(param_1 + 0x20);
    }
    switch(*(byte *)(param_1 + 0x19) & 0x7f) {
    default:
      param_2 = -1;
      break;
    case 1:
      param_2 = param_2 / (int)(*(byte *)(param_1 + 0x18) + 1);
      if (*(int *)(*(int *)(param_1 + 0x10) + 0x44) <= param_2) {
        param_2 = -1;
      }
      break;
    case 2:
      param_2 = -1;
      break;
    case 3:
      param_2 = -1;
      break;
    case 4:
      param_2 = param_2 / (int)(*(byte *)(param_1 + 0x18) + 1);
      if (param_2 < *(int *)(*(int *)(param_1 + 0x1c) + 8)) {
        param_2 = *(int *)(*(int *)(*(int *)(param_1 + 0x1c) + 4) + param_2 * 4);
      }
      else {
        param_2 = -1;
      }
      break;
    case 5:
      param_2 = -1;
    }
  }
  return param_2;
}

