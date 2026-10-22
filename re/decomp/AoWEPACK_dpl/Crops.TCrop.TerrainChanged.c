
void Crops_TCrop_TerrainChanged_23EDC2EF(int *param_1,char *param_2)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  int *piVar5;
  
                    /* 0xa5e74  6754  Crops.TCrop.TerrainChanged@23EDC2EF */
  if (param_2[8] != '\0') {
    return;
  }
  if ((*(byte *)(param_1 + 3) & 0x20) == 0) {
    *(char *)(param_1 + 6) = *param_2;
    return;
  }
  cVar1 = (**(code **)(*param_1 + 0x128))(param_1,(int)*param_2);
  if (cVar1 == '\0') {
    (**(code **)(*param_1 + -4))(param_1,1);
    return;
  }
  *(char *)(param_1 + 6) = *param_2;
  iVar4 = (**(code **)(*param_1 + 300))(param_1,(int)*param_2);
  param_1[5] = iVar4;
  piVar5 = (int *)HSEngine_TMapField_GetMaxLevelNoneTransparentHS_23EDC2EF
                            (*(undefined4 *)(param_2 + 4));
  if (piVar5 != (int *)0x0) {
    bVar2 = (**(code **)(*piVar5 + 100))();
    bVar3 = (**(code **)(*param_1 + 100))();
    if ((bVar3 < bVar2) &&
       (cVar1 = System__IsClass_51F89FF7(piVar5,HSEngine__TAbstractRoad_1FDAFEB5_exref),
       cVar1 == '\0')) goto LAB_557a5ef4;
  }
  if (param_2[1] < '\0') {
    return;
  }
  if (param_2[1] == '\x04') {
    return;
  }
LAB_557a5ef4:
  (**(code **)(*param_1 + -4))(param_1,1);
  return;
}

