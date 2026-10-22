
void Crops_TPlayerCropStructure_NeighbourTerrainChanged_23EDC2EF(int *param_1)

{
                    /* 0xa6818  6726  Crops.TPlayerCropStructure.NeighbourTerrainChanged@23EDC2EF */
  if (((~*(byte *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x3c) & 6) == 0) &&
     (*(char *)((int)param_1 + 0x3d) == '\0')) {
    (**(code **)(*param_1 + 0x234))();
  }
  return;
}

