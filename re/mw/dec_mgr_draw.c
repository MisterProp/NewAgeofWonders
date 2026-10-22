void AOWWinManager_TAOWWinManager_DrawSurface_23EDC2EF(int *param_1,int *param_2)
{
  bool bVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  int iVar6;
  int iStack_88;
  int local_74;
  int iStack_70;
  undefined1 local_6c [92];
                    /* 0x4518  99  AOWWinManager.TAOWWinManager.DrawSurface@23EDC2EF */
  local_74 = *(int *)(param_1[0x4c] + 8);
  if (-1 < local_74 + -1) {
    iVar6 = 0;
    do {
      piVar2 = (int *)Classes_TList_Get_23EDC2EF(param_1[0x4c],iVar6);
      (**(code **)(*piVar2 + 0x68))();
      if (*(short *)((int)param_1 + 0xea) != 0) {
        (*(code *)param_1[0x3a])(param_1[0x3b],param_1);
      }
      iVar6 = iVar6 + 1;
      local_74 = local_74 + -1;
    } while (local_74 != 0);
  }
  cVar5 = *(char *)((int)param_1 + 0xd2);
  iVar6 = (**(code **)(*param_2 + 0x2c))(param_2,local_6c);
  if (-1 < iVar6) {
    (**(code **)(*param_1 + 100))(param_1,local_6c);
    (**(code **)(*param_2 + 0x30))();
  }
  local_74 = *(int *)(param_1[0x4c] + 8);
  if (-1 < local_74 + -1) {
    iVar6 = 0;
    do {
      iVar3 = Classes_TList_Get_23EDC2EF(param_1[0x4c],iVar6);
      if (cVar5 == '\0') {
        if (*(char *)(iVar3 + 0x18d) == '\0') {
          *(undefined1 *)(iVar3 + 0xd2) = 0;
        }
        else {
          *(undefined1 *)(iVar3 + 0x18c) = 0;
        }
      }
      if (*(char *)(iVar3 + 0x178) == '\x01') {
        *(undefined1 *)(iVar3 + 0x178) = 0;
        if (cVar5 == '\x01') {
          iVar4 = (**(code **)(*param_2 + 0x2c))(param_2,local_6c);
          if (-1 < iVar4) {
            AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                      (param_1,*(int *)(iVar3 + 0x80),*(undefined4 *)(iVar3 + 0x84),local_6c,
                       *(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x7c),
                       *(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x78));
            (**(code **)(*param_2 + 0x30))();
          }
          param_1[0x4a] = -1;
          param_1[0x4b] = *(int *)(param_1[0x4c] + 8) + -1;
          AOWWinManager_TAOWWinManager_RedrawRect_23EDC2EF
                    (param_1,*(int *)(iVar3 + 0x80),*(int *)(iVar3 + 0x84),param_2,
                     *(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x7c),
                     *(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x78));
        }
      }
      else {
        if (((cVar5 == '\x01') && (0 < *(int *)(iVar3 + 0x184))) && (0 < *(int *)(iVar3 + 0x188))) {
          bVar1 = false;
          iVar4 = (**(code **)(*param_2 + 0x2c))(param_2,local_6c);
          if (-1 < iVar4) {
            iVar4 = *(int *)(iVar3 + 0x17c);
            if (iVar4 < *(int *)(iVar3 + 0x80)) {
              if (*(char *)(iVar3 + 0x155) == '\0') {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,iVar4,*(undefined4 *)(iVar3 + 0x180),local_6c,
                           *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188),
                           *(undefined4 *)(iVar3 + 0x80));
              }
              else {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,iVar4,*(undefined4 *)(iVar3 + 0x180),local_6c,
                           *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188),
                           *(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x158));
              }
              bVar1 = true;
            }
            iVar4 = *(int *)(iVar3 + 0x180);
            if (iVar4 < *(int *)(iVar3 + 0x84)) {
              if (*(char *)(iVar3 + 0x155) == '\0') {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,*(undefined4 *)(iVar3 + 0x17c),iVar4,local_6c,
                           *(undefined4 *)(iVar3 + 0x84),
                           *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184));
              }
              else {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,*(undefined4 *)(iVar3 + 0x17c),iVar4,local_6c,
                           *(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x15c),
                           *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184));
              }
              bVar1 = true;
            }
            iVar4 = *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184);
            if (*(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x78) < iVar4) {
              if (*(char *)(iVar3 + 0x155) == '\0') {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,*(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x78),
                           *(undefined4 *)(iVar3 + 0x180),local_6c,
                           *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188),iVar4);
              }
              else {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,*(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x160),
                           *(undefined4 *)(iVar3 + 0x180),local_6c,
                           *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188),iVar4);
              }
              bVar1 = true;
            }
            iVar4 = *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188);
            if (*(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x7c) < iVar4) {
              if (*(char *)(iVar3 + 0x155) == '\0') {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,*(undefined4 *)(iVar3 + 0x17c),
                           *(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x7c),local_6c,iVar4,
                           *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184));
              }
              else {
                AOWWinManager_TAOWWinManager_RedrawBack_23EDC2EF
                          (param_1,*(undefined4 *)(iVar3 + 0x17c),
                           *(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x164),local_6c,iVar4,
                           *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184));
              }
              bVar1 = true;
            }
            (**(code **)(*param_2 + 0x30))();
          }
          if (bVar1) {
            param_1[0x4a] = -1;
            param_1[0x4b] = *(int *)(param_1[0x4c] + 8) + -1;
            if (*(char *)(iVar3 + 0x155) == '\0') {
              AOWWinManager_TAOWWinManager_RedrawRect_23EDC2EF
                        (param_1,*(int *)(iVar3 + 0x17c),*(int *)(iVar3 + 0x180),param_2,
                         *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188),
                         *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184));
            }
            else {
              AOWWinManager_TAOWWinManager_RedrawRect_23EDC2EF
                        (param_1,*(int *)(iVar3 + 0x17c),*(int *)(iVar3 + 0x180),param_2,
                         *(int *)(iVar3 + 0x180) + *(int *)(iVar3 + 0x188),
                         *(int *)(iVar3 + 0x17c) + *(int *)(iVar3 + 0x184));
            }
          }
        }
        *(undefined4 *)(iVar3 + 0x180) = *(undefined4 *)(iVar3 + 0x84);
        *(undefined4 *)(iVar3 + 0x17c) = *(undefined4 *)(iVar3 + 0x80);
        *(undefined4 *)(iVar3 + 0x184) = *(undefined4 *)(iVar3 + 0x78);
        *(undefined4 *)(iVar3 + 0x188) = *(undefined4 *)(iVar3 + 0x7c);
      }
      iVar6 = iVar6 + 1;
      local_74 = local_74 + -1;
    } while (local_74 != 0);
  }
  local_74 = *(int *)(param_1[0x4c] + 8);
  if (-1 < local_74 + -1) {
    iVar6 = 0;
    do {
      piVar2 = (int *)Classes_TList_Get_23EDC2EF(param_1[0x4c],iVar6);
      if ((*(char *)((int)piVar2 + 0x69) == '\x01') && (param_1[0x41] < piVar2[0x2a] + 1)) {
        cVar5 = (char)piVar2[99];
        if (*(char *)((int)piVar2 + 0xd2) == '\0') {
          cVar5 = '\0';
        }
        param_1[0x4a] = iVar6;
        param_1[0x4b] = *(int *)(param_1[0x4c] + 8) + -1;
        iStack_88 = piVar2[100];
        if (iStack_88 < iVar6) {
          if (iStack_88 <= iVar6 + -1) {
            iStack_70 = ((iVar6 + -1) - iStack_88) + 1;
            do {
              iVar3 = Classes_TList_Get_23EDC2EF(param_1[0x4c],iStack_88);
              param_1[0x4a] = iVar6;
              AOWWinManager_TAOWWinManager_RedrawRectS_23EDC2EF
                        (param_1,*(undefined4 *)(iVar3 + 0x80),*(undefined4 *)(iVar3 + 0x84),param_2
                         ,*(int *)(iVar3 + 0x84) + *(int *)(iVar3 + 0x7c),
                         *(int *)(iVar3 + 0x80) + *(int *)(iVar3 + 0x78));
              iStack_88 = iStack_88 + 1;
              iStack_70 = iStack_70 + -1;
            } while (iStack_70 != 0);
          }
          piVar2[100] = iVar6;
        }
        (**(code **)(*piVar2 + 0x5c))(piVar2,param_2);
        *(undefined1 *)((int)piVar2 + 0xd2) = 1;
        *(undefined1 *)(piVar2 + 99) = 1;
        if (cVar5 == '\0') {
          param_1[0x4a] = iVar6;
          param_1[0x4b] = *(int *)(param_1[0x4c] + 8) + -1;
          AOWWinManager_TAOWWinManager_RedrawRect_23EDC2EF
                    (param_1,piVar2[0x20],piVar2[0x21],param_2,piVar2[0x21] + piVar2[0x1f],
                     piVar2[0x20] + piVar2[0x1e]);
        }
      }
      iVar6 = iVar6 + 1;
      local_74 = local_74 + -1;
    } while (local_74 != 0);
  }
  return;
}
