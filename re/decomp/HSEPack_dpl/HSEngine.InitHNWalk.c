
void HSEngine_InitHNWalk_3DC7E9B4(undefined4 *param_1,undefined4 param_2,int param_3,int param_4)

{
                    /* 0xe5c8  55  HSEngine.InitHNWalk@3DC7E9B4 */
  if (param_4 == 0) {
    param_1[2] = 0;
    *param_1 = 5;
  }
  else {
    param_1[2] = (param_4 + -1) * param_4 * 3 + 1;
    *param_1 = 0;
  }
  param_1[6] = param_2;
  param_1[7] = param_3;
  param_1[3] = param_4;
  param_1[4] = param_2;
  param_1[5] = param_3 - param_4;
  param_1[1] = 0;
  return;
}

