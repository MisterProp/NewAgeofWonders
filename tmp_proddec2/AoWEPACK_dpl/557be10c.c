// /AoWEPACK.dpl : 557be10c

int * PrdPlace_TProductionPlace_Create_23EDC2EF(int *param_1,char param_2)

{
  char cVar1;
  int iVar2;
  char extraout_DL;
  char cVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  
                    /* 0xbe10c  7480  PrdPlace.TProductionPlace.Create@23EDC2EF */
  cVar3 = '\0';
  if (param_2 != '\0') {
    param_1 = (int *)System__ClassCreate_51F89FF7();
    cVar3 = extraout_DL;
  }
  AoWE_TPlayerStructure_Create_23EDC2EF(param_1,0);
  iVar2 = (**(code **)(*param_1 + 0x224))();
  param_1[0x11] = iVar2;
  *(int **)(iVar2 + 0x28) = param_1;
  cVar1 = (**(code **)(*param_1 + 0x230))();
  if (cVar1 == '\b') {
    iVar2 = Engine_TEObject_Create_23EDC2EF(&AoWE__TPlayerStructureIncomeSource_321B7250,1,0);
    param_1[0x10] = iVar2;
    AoWE_TPlayerStructureIncomeSource_Setup_23EDC2EF(iVar2,param_1);
    *(undefined1 *)((int)param_1 + 0x49) = 2;
  }
  else {
    iVar2 = Engine_TEObject_Create_23EDC2EF(&AoWE__TPlayerStructurePowerSource_159F917B,1,0);
    param_1[0xf] = iVar2;
    AoWE_TPlayerStructurePowerSource_Setup_23EDC2EF(iVar2,param_1);
    *(undefined1 *)((int)param_1 + 0x49) = 3;
  }
  if (cVar3 != '\0') {
    *in_FS_OFFSET = uStack_20;
  }
  return param_1;
}

