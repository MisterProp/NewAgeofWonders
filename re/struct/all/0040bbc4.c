
int FUN_0040bbc4(int param_1,char param_2)

{
  undefined4 uVar1;
  char extraout_DL;
  char cVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  
  cVar2 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar2 = extraout_DL;
  }
  AoWE_TAoWCampaignScenario_Create_23EDC2EF(param_1,0);
  uVar1 = Engine_TENode_Create_23EDC2EF(&PTR_LAB_0040b8c4,1,0);
  *(undefined4 *)(param_1 + 0x30) = uVar1;
  uVar1 = Engine_TENode_Create_23EDC2EF(&PTR_LAB_0040b8c4,1,0);
  *(undefined4 *)(param_1 + 0x34) = uVar1;
  iVar3 = 10;
  do {
    FUN_0040bae8(&LAB_0040b804,1,*(undefined4 *)(param_1 + 0x30));
    FUN_0040bae8(&LAB_0040b804,1,*(undefined4 *)(param_1 + 0x34));
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  iVar3 = System_TObject_Create_23EDC2EF(Engine__TIntegerList_D8218C48_exref,1);
  *(int *)(param_1 + 0x2c) = iVar3;
  *(undefined4 *)(iVar3 + 0x10) = 0xffffffff;
  if (cVar2 != '\0') {
    *in_FS_OFFSET = uStack_20;
  }
  return param_1;
}

