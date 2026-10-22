
void Crops_TPlayerCropStructure_Remove_23EDC2EF(int *param_1)

{
  char cVar1;
  
                    /* 0xa6848  6725  Crops.TPlayerCropStructure.Remove@23EDC2EF */
  cVar1 = (**(code **)(*param_1 + 0x84))();
  if (cVar1 == '\0') {
    Crops_TPlayerCropStructure_SetCropCount_23EDC2EF(param_1,0);
  }
  HSEngine_TMapObject_Remove_23EDC2EF(param_1);
  return;
}

