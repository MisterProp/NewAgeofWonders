
void FUN_00406130(int param_1)

{
  undefined *puVar1;
  
  puVar1 = PTR_DAT_0045a420;
  *(undefined1 *)(param_1 + 0x178) = 0;
  if ((*(char *)(*(int *)(param_1 + 0xdc) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x84) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x84) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xe0) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x88) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x88) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xe4) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x8c) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x8c) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xf4) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x90) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x90) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xe8) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x94) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x94) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xec) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x98) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x98) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xf0) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0x9c) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0x9c) & 0x1f);
  }
  if ((*(char *)(*(int *)(param_1 + 0xf8) + 0xb0) == '\0') &&
     (*(int *)(*(int *)puVar1 + 0xa0) != -1)) {
    *(byte *)(param_1 + 0x178) =
         *(byte *)(param_1 + 0x178) |
         '\x01' << ((byte)*(undefined4 *)(*(int *)puVar1 + 0xa0) & 0x1f);
  }
  return;
}

