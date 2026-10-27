// /AoW.exe : FUN_0040a2a0

void FUN_0040a2a0(int param_1,int param_2,undefined4 *param_3)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_48 [15];
  
  puVar3 = local_48;
  for (iVar2 = 0xf; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_3;
    param_3 = param_3 + 1;
    puVar3 = puVar3 + 1;
  }
  if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc0) != 0) {
    *(undefined1 *)(param_1 + 0x23c) = 3;
    cVar1 = System__IsClass_51F89FF7
                      (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0),AoWE__THero_228E24B5_exref);
    if (cVar1 == '\0') {
      *(undefined1 *)(param_1 + 0x23d) = 2;
    }
    else {
      *(undefined1 *)(param_1 + 0x23d) = 0;
    }
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a420 + 0xc0) + 0x1a4))
              (*(int **)(*(int *)PTR_DAT_0045a420 + 0xc0),*(int *)(param_2 + 0x84) + 0x31,
               *(int *)(param_2 + 0x88) + 0x4e,param_1 + 0x23c,local_48);
  }
  return;
}

