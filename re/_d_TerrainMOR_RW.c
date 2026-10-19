
void Terrain_TTerrainMOR_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0x16b78  859  Terrain.TTerrainMOR.ReadWrite@23EDC2EF */
  HSEngine_TMultiHexMOR_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x45,0x11);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x44,0x12);
  return;
}

