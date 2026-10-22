
int Crops_TCrop_GetTerrainTypeImage_23EDC2EF(int param_1,int param_2)

{
  char cVar1;
  
                    /* 0xa5f14  6753  Crops.TCrop.GetTerrainTypeImage@23EDC2EF */
  if (*(int **)(param_1 + 0x1c) != (int *)0x0) {
    cVar1 = (**(code **)(**(int **)(param_1 + 0x1c) + 0x238))();
    if (cVar1 == '\v') {
      param_2 = param_2 + 0x1e;
    }
  }
  return param_2;
}

