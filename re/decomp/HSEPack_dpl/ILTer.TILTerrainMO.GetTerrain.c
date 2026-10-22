
char ILTer_TILTerrainMO_GetTerrain_23EDC2EF(int param_1)

{
  char cVar1;
  
                    /* 0x18928  952  ILTer.TILTerrainMO.GetTerrain@23EDC2EF */
  cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 8) + 0x28));
  if (-1 < cVar1) {
    cVar1 = *(char *)(param_1 + 0x18);
  }
  return cVar1;
}

