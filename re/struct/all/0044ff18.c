
void FUN_0044ff18(void)

{
  undefined *puVar1;
  
  puVar1 = PTR_DAT_0045a420;
  if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xbc) != 0) {
    Sign_TSign_ChangeText_23EDC2EF
              (*(int *)(*(int *)PTR_DAT_0045a420 + 0xbc),
               (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),
               *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a5c4 + 0x78) + 0x118));
  }
  *(undefined4 *)(*(int *)puVar1 + 0xa8) = 0;
  *(undefined4 *)(*(int *)puVar1 + 0xb0) = 0;
  *(undefined4 *)(*(int *)puVar1 + 0xb4) = 0;
  *(undefined4 *)(*(int *)puVar1 + 0xb8) = 0;
  *(undefined4 *)(*(int *)puVar1 + 0xbc) = 0;
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a5c4 + 100),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a5c4 + 0x60) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a5c4 + 0x60),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a5c4 + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a5c4 + 0x44),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x6c) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x6c),0);
  return;
}

