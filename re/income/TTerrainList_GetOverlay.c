

undefined1 HSEngine_TTerrainList_GetOverlay_23EDC2EF(int param_1,int param_2)



{

                    /* 0x50c8  542  HSEngine.TTerrainList.GetOverlay@23EDC2EF */

  if (param_2 < *(int *)(param_1 + 4)) {

    return *(undefined1 *)(*(int *)(param_1 + 8) + param_2 * 2 + 1);

  }

  return 0xfe;

}




