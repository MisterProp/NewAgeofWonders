
void FUN_0040e2f0(int param_1,int *param_2)

{
  int iVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  
  iVar1 = *(int *)(*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) + 0x20);
  if ((*(char *)(param_1 + 0x1c8) == '\x01') &&
     (param_2[3] + *(int *)(param_1 + 0x1ac) == *(int *)(param_1 + 0x1cc))) {
    uVar3 = 1;
  }
  else {
    uVar3 = 0;
  }
  (**(code **)(*param_2 + 0xac))(param_2,uVar3);
  if (((char)param_2[0x2c] == '\0') &&
     (param_2[3] + *(int *)(param_1 + 0x1ac) < *(int *)(*(int *)(iVar1 + 0x28) + 8))) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  *(undefined1 *)(param_2 + 0x30) = uVar2;
  return;
}

