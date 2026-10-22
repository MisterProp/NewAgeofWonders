
int Crops_TPlayerCropStructureResource_Destroy_23EDC2EF(int param_1,uint param_2)

{
                    /* 0xa6a50  6721  Crops.TPlayerCropStructureResource.Destroy@23EDC2EF */
  System_TObject_Free_23EDC2EF(*(undefined4 *)(param_1 + 0x74));
  *(undefined4 *)(param_1 + 0x74) = 0;
  AoWE_TPlayerStructureResource_Destroy_23EDC2EF(param_1,param_2 & 0xfffffffe);
  if ((char)param_2 != '\0') {
    System__ClassDestroy_51F89FF7(param_1);
  }
  return param_1;
}

