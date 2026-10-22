
void AoWE_TStructure_Activate_23EDC2EF(int *param_1)

{
  char cVar1;
  
                    /* 0x5f6c0  2249  AoWE.TStructure.Activate@23EDC2EF */
  ILTer_TILTerrainMO_Activate_23EDC2EF(param_1);
  AoWE_TStructureControl_Register_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x100),param_1);
  if ((*(char *)(AoWE_AoWHSMap_6B957A58 + 0x249) != '\0') && (*(char *)(param_1[2] + 0x59) != '\0'))
  {
    cVar1 = (**(code **)(*param_1 + 0x84))();
    if (cVar1 == '\0') {
      (**(code **)(*param_1 + 400))();
    }
  }
  return;
}

