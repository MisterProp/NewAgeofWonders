
int Crops_TPlayerCropStructure_Destroy_23EDC2EF(int param_1,uint param_2)

{
                    /* 0xa60cc  6745  Crops.TPlayerCropStructure.Destroy@23EDC2EF */
  AoWE_TPlayerStructure_Destroy_23EDC2EF(param_1,param_2 & 0xfffffffe);
  System_TObject_Free_23EDC2EF(*(undefined4 *)(param_1 + 0x40));
  if ((char)param_2 != '\0') {
    System__ClassDestroy_51F89FF7(param_1);
  }
  return param_1;
}

