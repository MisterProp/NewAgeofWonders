
int * Engine_TEObject_Create_23EDC2EF(int *param_1,char param_2,undefined4 param_3)

{
  undefined4 extraout_ECX;
  char extraout_DL;
  char cVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  
                    /* 0x18edc  264  Engine.TEObject.Create@23EDC2EF */
  cVar1 = '\0';
  if (param_2 != '\0') {
    param_1 = (int *)System__ClassCreate_51F89FF7();
    param_3 = extraout_ECX;
    cVar1 = extraout_DL;
  }
  System_TObject_Create_23EDC2EF(param_1,0);
  (**(code **)(*param_1 + 8))(param_1,param_3);
  if (cVar1 != '\0') {
    *in_FS_OFFSET = uStack_1c;
  }
  return param_1;
}

