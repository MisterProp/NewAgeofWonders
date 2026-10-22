
void AoWE_TStructure_TerrainChanged_23EDC2EF(int *param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  
                    /* 0x5ec80  2276  AoWE.TStructure.TerrainChanged@23EDC2EF */
  cVar1 = HSEngine_TTerrainList_GetTerrain_23EDC2EF
                    (*(undefined4 *)(param_1[2] + 0x28),(int)param_2[8]);
  if (-1 < cVar1) {
    if (*(char *)(param_1[2] + 0x44) == '\0') {
      ILTer_TFixedILTerrainMO_TerrainChanged_23EDC2EF(param_1,param_2);
    }
    else {
      cVar1 = *param_2;
      *(char *)(param_1 + 6) = cVar1;
      iVar2 = (**(code **)(*param_1 + 300))(param_1,(int)cVar1);
      param_1[5] = iVar2;
    }
  }
  return;
}

