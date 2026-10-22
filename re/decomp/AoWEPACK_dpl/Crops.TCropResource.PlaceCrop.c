
int * Crops_TCropResource_PlaceCrop_23EDC2EF
                (undefined4 param_1,char param_2,char param_3,int param_4,char param_5)

{
  char cVar1;
  int *piVar2;
  
                    /* 0xa6008  6747  Crops.TCropResource.PlaceCrop@23EDC2EF */
  piVar2 = (int *)Crops_TCrop_Create_23EDC2EF(&Crops__TCrop_3E981956,1,0);
  (**(code **)(*piVar2 + 0x110))(piVar2,param_1);
  HSEngine_TMapObject_SetResourceLinked_23EDC2EF(piVar2,1);
  piVar2[7] = param_4;
  cVar1 = HSEngine_THSMap_PlaceHX_23EDC2EF
                    (*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,piVar2,(int)param_2,
                     (int)param_5,(int)param_3);
  if (cVar1 == '\0') {
    System_TObject_Free_23EDC2EF(piVar2);
    piVar2 = (int *)0x0;
  }
  return piVar2;
}

