
int FUN_0040bae8(int param_1,char param_2)

{
  undefined4 uVar1;
  char extraout_DL;
  char cVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  
  cVar2 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar2 = extraout_DL;
  }
  Engine_TEObject_Create_23EDC2EF(param_1,0);
  uVar1 = System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);
  *(undefined4 *)(param_1 + 0x14) = uVar1;
  if (cVar2 != '\0') {
    *in_FS_OFFSET = uStack_18;
  }
  return param_1;
}

