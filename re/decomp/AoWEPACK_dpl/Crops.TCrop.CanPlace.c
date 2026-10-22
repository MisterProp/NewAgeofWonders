
undefined4
Crops_TCrop_CanPlace_23EDC2EF
          (undefined4 param_1,undefined4 param_2,char param_3,char param_4,char param_5)

{
  byte bVar1;
  char cVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  
                    /* 0xa5f40  6752  Crops.TCrop.CanPlace@23EDC2EF */
  uVar3 = HSEngine_TMapContainer_GetField_23EDC2EF(param_2,(int)param_3,(int)param_5,(int)param_4);
  piVar4 = (int *)HSEngine_TMapField_GetMaxLevelNoneTransparentHS_23EDC2EF(uVar3);
  iVar5 = HSEngine_TMapField_FindOwnedHS_23EDC2EF(uVar3,&AoWE__TFire_9543D422);
  if (iVar5 == 0) {
    if (piVar4 != (int *)0x0) {
      bVar1 = (**(code **)(*piVar4 + 100))();
      if (9 < bVar1) {
        cVar2 = System__IsClass_51F89FF7(piVar4,HSEngine__TAbstractRoad_1FDAFEB5_exref);
        if (cVar2 == '\0') goto LAB_557a5f93;
      }
    }
    uVar3 = 1;
  }
  else {
LAB_557a5f93:
    uVar3 = 0;
  }
  return uVar3;
}

