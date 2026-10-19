
undefined4 AoWE_TArmy_GetArmyHS_23EDC2EF(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  char cStack_c;
  char cStack_b;
  char cStack_a;
  undefined4 local_8;
  
                    /* 0x8c8b8  432  AoWE.TArmy.GetArmyHS@23EDC2EF */
  iVar1 = (int)&local_8 + 2;
  local_8 = param_3;
  (**(code **)(*param_1 + 0x98))(param_1,&local_8,(int)&local_8 + 1);
  cStack_c = (char)iVar1;
  if (cStack_c != -1) {
    cStack_a = (char)((uint)iVar1 >> 0x10);
    cStack_b = (char)((uint)iVar1 >> 8);
    piVar2 = (int *)HSEngine_TMapContainer_GetField_23EDC2EF
                              (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x10),(int)cStack_c,
                               (int)cStack_b,(int)cStack_a);
    uVar3 = (**(code **)(*piVar2 + 0x80))(piVar2,0x20217);
    return uVar3;
  }
  return 0;
}

