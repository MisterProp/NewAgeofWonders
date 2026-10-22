
void Farm_TFarm_TerrainChanged_23EDC2EF(int *param_1,int param_2)

{
                    /* 0xb2df0  7103  Farm.TFarm.TerrainChanged@23EDC2EF */
  AoWE_TStructure_TerrainChanged_23EDC2EF(param_1,param_2);
  if (*(char *)(param_2 + 8) == '\0') {
    (**(code **)(*param_1 + 0x234))();
  }
  return;
}

