
void FUN_00415d08(int param_1)

{
  int iVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00415f85;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  iVar1 = *(int *)(*(int *)(param_1 + 0x2e4) + 0x40);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(iVar1 + 0x24),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xf8),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(iVar1 + 0x25),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x100),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(iVar1 + 0x26),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(iVar1 + 0x29),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x104),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(iVar1 + 0x27),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x108),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(iVar1 + 0x28),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)(param_1 + 0x2e4) + 0x6a),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 200),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)(param_1 + 0x2e4) + 0x6b),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xd0),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)(param_1 + 0x2e4) + 0x6c),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xcc),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)(param_1 + 0x2e4) + 0x6f),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xd4),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)(param_1 + 0x2e4) + 0x6d),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xd8),local_8);
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)(param_1 + 0x2e4) + 0x6e),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xdc),local_8);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0xa8))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x68),local_8);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0xac))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x80),local_8);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0xb0))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x74),local_8);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0xb4))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x8c),local_8);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0xd0))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),local_8);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0xd4))();
  SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
  FUN_00415c40(param_1);
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00415f8c;
  puStack_18 = (undefined1 *)0x415f84;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

