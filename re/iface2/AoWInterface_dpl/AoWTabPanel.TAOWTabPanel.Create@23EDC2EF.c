// /AoWInterface.dpl : AoWTabPanel.TAOWTabPanel.Create@23EDC2EF

int AoWTabPanel_TAOWTabPanel_Create_23EDC2EF(int param_1,char param_2)

{
  undefined4 uVar1;
  char extraout_DL;
  char cVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  
                    /* 0x13ea0  320  AoWTabPanel.TAOWTabPanel.Create@23EDC2EF */
  cVar2 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar2 = extraout_DL;
  }
  AoWControl_TAOWControl_Create_23EDC2EF(param_1,0);
  uVar1 = System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  *(undefined4 *)(param_1 + 0x110) = uVar1;
  uVar1 = System_TObject_Create_23EDC2EF(Classes__TList_9F2060BF_exref,1);
  *(undefined4 *)(param_1 + 0x120) = uVar1;
  *(undefined4 *)(param_1 + 0x74) = 0xb;
  if (cVar2 != '\0') {
    *in_FS_OFFSET = uStack_18;
  }
  return param_1;
}

