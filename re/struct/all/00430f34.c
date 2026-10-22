
void FUN_00430f34(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00430fec;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  cVar2 = FUN_0042eecc(param_1);
  if (cVar2 != '\0') {
    if (*(char *)(param_1 + 0x220) == '\0') {
      cVar2 = AoWE_THero_CastSpell_23EDC2EF
                        (*(int *)(param_1 + 0x22c),*(undefined4 *)(*(int *)(param_1 + 0x22c) + 0x84)
                         ,&local_8);
      if (cVar2 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x1c4) + 0x6c))(*(int **)(param_1 + 0x1c4),0);
      }
    }
    else if (*(char *)(param_1 + 0x220) == '\x01') {
      uVar3 = AoWE_TSpellControl_GetSpell_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),
                         *(undefined4 *)(*(int *)(param_1 + 0x22c) + 0x84));
      FUN_00430788(param_1,uVar3);
    }
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00430ff3;
  puStack_18 = (undefined1 *)0x430feb;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

