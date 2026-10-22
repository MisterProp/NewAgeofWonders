
undefined4 FUN_0043588c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  byte bVar4;
  undefined4 extraout_EDX;
  undefined3 uVar5;
  
  iVar2 = AoWE_TCombatPartyList_GetParty_23EDC2EF
                    (*(undefined4 *)
                      (*(int *)(*(int *)(*(int *)(*(int *)(param_4 + -4) + 0x144) + 8) + 0xc) + 0x3c
                      ),param_1);
  iVar3 = AoWE_TCombatPartyList_GetParty_23EDC2EF
                    (*(undefined4 *)
                      (*(int *)(*(int *)(*(int *)(*(int *)(param_4 + -4) + 0x144) + 8) + 0xc) + 0x3c
                      ),param_2);
  if ((iVar2 == 0) || (*(char *)(iVar2 + 0x18) == '\0')) {
    bVar4 = 0;
    uVar5 = 0;
  }
  else {
    uVar5 = (undefined3)((uint)extraout_EDX >> 8);
    bVar4 = 1;
  }
  if ((iVar3 == 0) || (*(char *)(iVar3 + 0x18) == '\0')) {
    bVar1 = 0;
  }
  else {
    bVar1 = 1;
  }
  return CONCAT31(uVar5,bVar4 ^ bVar1);
}

