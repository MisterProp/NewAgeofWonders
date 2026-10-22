
void FUN_00412c8c(int param_1)

{
  int iVar1;
  
  if (*(char *)(param_1 + 0x344) != '\0') {
    iVar1 = *(int *)(param_1 + 0x4c);
    AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF
              (iVar1,(*(int *)(iVar1 + 0x13c) + 1) - *(int *)(iVar1 + 0x158));
    *(undefined1 *)(param_1 + 0x344) = 0;
  }
  return;
}

