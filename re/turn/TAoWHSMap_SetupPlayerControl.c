
void AoWE_TAoWHSMap_SetupPlayerControl_23EDC2EF(int param_1,char param_2,undefined1 param_3)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0x77f1c  1401  AoWE.TAoWHSMap.SetupPlayerControl@23EDC2EF */
  if (param_2 == '\x01') {
    if (*(char *)(param_1 + 0x13a) == '\x02') {
      uVar2 = *(undefined4 *)(param_1 + 0x244);
      cVar1 = System__IsClass_51F89FF7(uVar2,&AoWE__TPBEMPlayerControl_E74B9DF9);
      if (cVar1 == '\0') {
        System_TObject_Free_23EDC2EF(uVar2);
        uVar2 = AoWE_TPlayerControl_Create_23EDC2EF(&AoWE__TPBEMPlayerControl_E74B9DF9,1,0);
        *(undefined4 *)(param_1 + 0x244) = uVar2;
      }
    }
    else {
      uVar2 = *(undefined4 *)(param_1 + 0x244);
      cVar1 = System__IsClass_51F89FF7(uVar2,&AoWE__TPBEMPlayerControl_E74B9DF9);
      if (cVar1 == '\0') {
        System_TObject_Free_23EDC2EF(uVar2);
        uVar2 = AoWE_TPlayerControl_Create_23EDC2EF(&AoWE__TTurnPlayerControl_EB235E99,1,0);
        *(undefined4 *)(param_1 + 0x244) = uVar2;
      }
    }
  }
  else if (param_2 == '\x02') {
    uVar2 = *(undefined4 *)(param_1 + 0x244);
    cVar1 = System__IsClass_51F89FF7(uVar2,&AoWE__TSimultaniousPlayerControl_06A16996);
    if (cVar1 == '\0') {
      System_TObject_Free_23EDC2EF(uVar2);
    }
    uVar2 = AoWE_TPlayerControl_Create_23EDC2EF(&AoWE__TSimultaniousPlayerControl_06A16996,1,0);
    *(undefined4 *)(param_1 + 0x244) = uVar2;
  }
  *(undefined1 *)(param_1 + 0x13a) = param_3;
  *(char *)(param_1 + 0x239) = param_2;
  if (*(int **)(param_1 + 0x244) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x244) + 0x60))();
  }
  return;
}

