
int * AoWE_TProductionControl_Create_23EDC2EF(int *param_1,char param_2)

{
  int iVar1;
  char extraout_DL;
  char cVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  
                    /* 0x5cab4  2377  AoWE.TProductionControl.Create@23EDC2EF */
  cVar2 = '\0';
  if (param_2 != '\0') {
    param_1 = (int *)System__ClassCreate_51F89FF7();
    cVar2 = extraout_DL;
  }
  Engine_TEObject_Create_23EDC2EF(param_1,0);
  iVar1 = Engine_TEObject_Create_23EDC2EF(&AoWE__TProductionSettings_CDBB22DF,1,0);
  param_1[9] = iVar1;
  iVar1 = Engine_TENode_Create_23EDC2EF(&AoWE__TUnitProductionList_A836C0FC,1,0);
  param_1[7] = iVar1;
  iVar1 = Engine_TENode_Create_23EDC2EF(&AoWE__TProductionQueue_EEEC3553,1,0);
  param_1[5] = iVar1;
  *(int **)(iVar1 + 0xc) = param_1;
  *(undefined1 *)(param_1 + 8) = 0;
  (**(code **)(*param_1 + 0x30))();
  if (cVar2 != '\0') {
    *in_FS_OFFSET = uStack_20;
  }
  return param_1;
}

