

undefined1 HSEngine_TTerrainList_GetTerrain_23EDC2EF(int param_1,int param_2)



{

                    /* 0x5054  545  HSEngine.TTerrainList.GetTerrain@23EDC2EF */

  if (param_2 < *(int *)(param_1 + 4)) {

    return *(undefined1 *)(*(int *)(param_1 + 8) + param_2 * 2);

  }

  return 0xfe;

}




