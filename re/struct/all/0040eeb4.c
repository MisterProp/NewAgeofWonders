
void FUN_0040eeb4(int param_1)

{
  AoWReg_TAoWRegistry_SetLocalEmailAddress_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),
             *(undefined4 *)(*(int *)(param_1 + 0x70) + 0x90));
  AoWReg_TAoWRegistry_SetSMTPServer_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),
             *(undefined4 *)(*(int *)(param_1 + 0x78) + 0x90));
  AoWReg_TAoWRegistry_SetAttachmentDirectory_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),
             *(undefined4 *)(*(int *)(param_1 + 0x80) + 0x90));
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
  return;
}

