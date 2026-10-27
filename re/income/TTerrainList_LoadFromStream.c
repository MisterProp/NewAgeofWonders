

void HSEngine_TTerrainList_LoadFromStream_23EDC2EF(int param_1,int *param_2)



{

                    /* 0x5354  532  HSEngine.TTerrainList.LoadFromStream@23EDC2EF */

  (**(code **)(*param_2 + 4))(param_2,param_1 + 4,4);

  HSEngine_TTerrainList_SetCount_23EDC2EF(param_1,*(undefined4 *)(param_1 + 4));

  (**(code **)(*param_2 + 4))(param_2,*(undefined4 *)(param_1 + 8),*(int *)(param_1 + 4) * 2);

  return;

}




