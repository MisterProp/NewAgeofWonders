
void FUN_004062a0(int param_1)

{
  undefined *puVar1;
  undefined1 *puVar2;
  undefined1 uVar3;
  char cVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puVar1 = PTR_DAT_0045a420;
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_004066ae;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  uVar3 = AoWE_TSelectedArmy_GetSelection_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8));
  *(undefined1 *)(param_1 + 0x178) = uVar3;
  if ((*(int *)(*(int *)puVar1 + 0x84) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x84) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xdc) + 0xac))(*(int **)(param_1 + 0xdc),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0x88) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x88) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xe0) + 0xac))(*(int **)(param_1 + 0xe0),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0x8c) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x8c) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xe4) + 0xac))(*(int **)(param_1 + 0xe4),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0x90) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x90) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xf4) + 0xac))(*(int **)(param_1 + 0xf4),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0x94) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x94) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xe8) + 0xac))(*(int **)(param_1 + 0xe8),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0x98) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x98) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xec) + 0xac))(*(int **)(param_1 + 0xec),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0x9c) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0x9c) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xf0) + 0xac))(*(int **)(param_1 + 0xf0),uVar7);
  if ((*(int *)(*(int *)puVar1 + 0xa0) == -1) ||
     ((1 << ((byte)*(int *)(*(int *)puVar1 + 0xa0) & 0x1f) & (uint)*(byte *)(param_1 + 0x178)) != 0)
     ) {
    uVar7 = 0;
  }
  else {
    uVar7 = 1;
  }
  (**(code **)(**(int **)(param_1 + 0xf8) + 0xac))(*(int **)(param_1 + 0xf8),uVar7);
  if (*(char *)(*(int *)(param_1 + 0xdc) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x50),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x50),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xe0) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x54),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x54),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xe4) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x58),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x58),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xf4) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xe8) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x60),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x60),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xec) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 100),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 100),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xf0) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x68),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x68),0x1a);
  }
  if (*(char *)(*(int *)(param_1 + 0xf8) + 0xb0) == '\0') {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x6c),0xc);
  }
  else {
    AoWFrame_TAoWFrame_SetILFIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x6c),0x1a);
  }
  uVar7 = AoWE_TSelectedArmy_GetSelection_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8));
  uVar7 = AoWE_TArmy_GetPartyMoves_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xa4),uVar7);
  uVar7 = FUN_004541b0(*(undefined4 *)puVar1,uVar7);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),uVar7);
  uVar7 = AoWE_TSelectedArmy_GetSelection_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8));
  cVar4 = AoWE_TArmy_MovePoints_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0xa4),uVar7);
  SysUtils_IntToStr_0F6FDFF6((int)cVar4,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x74),local_8);
  uVar7 = AoWE_TSelectedArmy_GetSelection_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8));
  iVar5 = AoWE_TSelectedArmy_GetArmyHS_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xd8));
  uVar6 = AoWE_TArmy_GetPartyStatus_23EDC2EF(*(undefined4 *)(iVar5 + 0x1c),uVar7);
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)(AoWE_TPartyStatusRStr_DE1251C3_exref + (uVar6 & 0x7f) * 4),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_004066b5;
  puStack_18 = (undefined1 *)0x4066ad;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

