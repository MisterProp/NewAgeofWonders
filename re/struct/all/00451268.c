
void FUN_00451268(int param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5)

{
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    HSEngine_THSMapViewer_SetSceneWidth_23EDC2EF
              (*(undefined4 *)(param_1 + 0x48),*(int *)(*(int *)(param_1 + 0x44) + 0x7c) + -0x18);
    HSEngine_THSMapViewer_SetSceneHeight_23EDC2EF
              (*(undefined4 *)(param_1 + 0x48),*(int *)(*(int *)(param_1 + 0x44) + 0x80) + -0x18);
    if (*(char *)(*(int *)(param_1 + 0x44) + 0x101) == '\0') {
      HSEngine_THSMapViewer_ShowScene_23EDC2EF
                (*(undefined4 *)(param_1 + 0x48),param_5 + 0xc,param_4 + 0xc,param_3);
    }
    else {
      HSEngine_THSMapViewer_ShowScene_23EDC2EF
                (*(undefined4 *)(param_1 + 0x48),0xc,0xc,
                 *(undefined4 *)(*(int *)(param_1 + 0x44) + 0x18c));
    }
  }
  return;
}

