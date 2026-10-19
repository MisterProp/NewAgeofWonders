
undefined4 Mountain_TMountainMO_GetOverlay_23EDC2EF(int param_1)

{
  undefined4 uVar1;
  
                    /* 0xa2dc8  6622  Mountain.TMountainMO.GetOverlay@23EDC2EF */
  uVar1 = HSEngine_TTerrainList_GetOverlay_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 8) + 0x28));
  if (-1 < (char)uVar1) {
    uVar1 = 0;
  }
  return uVar1;
}

