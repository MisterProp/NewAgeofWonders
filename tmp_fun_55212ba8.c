
/* WARNING: Unable to track spacebase fully for stack */

void FUN_55212ba8(void)

{
  ushort *puVar1;
  uint uVar2;
  undefined1 *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined1 *puVar8;
  undefined1 *puVar9;
  uint *puVar10;
  uint *puVar11;
  bool bVar12;
  int in_stack_00000004;
  int in_stack_00000008;
  uint *in_stack_0000000c;
  int in_stack_00000010;
  int in_stack_00000014;
  int in_stack_00000018;
  uint local_28;
  int local_24;
  int local_1c;
  undefined1 *local_8;
  
  puVar3 = (undefined1 *)(in_stack_00000008 + 0x1fU >> 5);
  iVar6 = in_stack_00000008 + in_stack_00000018;
  if (((-1 < iVar6 - *(int *)(in_stack_00000010 + 0x10)) &&
      (in_stack_00000018 < *(int *)(in_stack_00000010 + 0x18))) &&
     (*(int *)(in_stack_00000010 + 0x10) < *(int *)(in_stack_00000010 + 0x18))) {
    iVar7 = in_stack_00000004 + in_stack_00000014;
    if (((-1 < iVar7 - *(int *)(in_stack_00000010 + 0x14)) &&
        (in_stack_00000014 < *(int *)(in_stack_00000010 + 0x1c))) &&
       (*(int *)(in_stack_00000010 + 0x14) < *(int *)(in_stack_00000010 + 0x1c))) {
      iVar4 = iVar7 - *(int *)(in_stack_00000010 + 0x1c);
      if (iVar4 != 0 && *(int *)(in_stack_00000010 + 0x1c) <= iVar7) {
        in_stack_00000004 = in_stack_00000004 - iVar4;
      }
      iVar7 = *(int *)(in_stack_00000010 + 0x14) - in_stack_00000014;
      if (iVar7 != 0 && in_stack_00000014 <= *(int *)(in_stack_00000010 + 0x14)) {
        in_stack_00000014 = in_stack_00000014 + iVar7;
        in_stack_00000004 = in_stack_00000004 - iVar7;
        in_stack_0000000c = (uint *)((int)in_stack_0000000c + iVar7 * (int)puVar3 * 4);
      }
      if ((in_stack_00000018 - *(int *)(in_stack_00000010 + 0x10) < 0) ||
         (*(int *)(in_stack_00000010 + 0x18) < iVar6)) {
        local_24 = in_stack_00000008 * 2;
        uVar5 = *(int *)(in_stack_00000010 + 0x10) - in_stack_00000018;
        local_8 = puVar3;
        if ((int)uVar5 < 0) {
          local_28 = 0;
        }
        else {
          local_28 = uVar5 & 0x1f;
          uVar5 = uVar5 >> 5;
          if (uVar5 != 0) {
            local_8 = puVar3 + -uVar5;
            local_24 = local_24 + uVar5 * -0x40;
            in_stack_0000000c = in_stack_0000000c + uVar5;
            in_stack_00000018 = in_stack_00000018 + uVar5 * 0x20;
          }
        }
        uVar5 = iVar6 - *(int *)(in_stack_00000010 + 0x18);
        if (-1 < (int)uVar5) {
          local_24 = local_24 + uVar5 * -2;
          local_8 = local_8 + (-1 - (uVar5 >> 5));
        }
        iVar6 = *(int *)(in_stack_00000010 + 0xc);
        iVar7 = *(int *)(in_stack_00000010 + 0x20) + in_stack_00000018 * 2 +
                in_stack_00000014 * iVar6;
        do {
          local_1c = 0x40;
          iVar4 = 0;
          puVar8 = local_8;
          puVar10 = in_stack_0000000c;
          if (local_28 == 0) goto LAB_55212df0;
          uVar5 = *in_stack_0000000c << (sbyte)local_28;
          iVar4 = local_28 * 2;
          puVar9 = local_8;
          puVar11 = in_stack_0000000c + 1;
          if (local_8 == (undefined1 *)0x0) {
LAB_55212e94:
            do {
              while( true ) {
                bVar12 = CARRY4(uVar5,uVar5);
                uVar5 = uVar5 * 2;
                if (bVar12) break;
                iVar4 = iVar4 + 2;
                if ((uVar5 == 0) || (local_24 <= iVar4)) goto LAB_55212ecd;
              }
              puVar1 = (ushort *)(iVar7 + iVar4);
              iVar4 = iVar4 + 2;
              *(ushort *)(iVar7 + iVar4) =
                   (*puVar1 >> 1 & 0x7bef) + (short)((*puVar1 & 0xe79c) >> 2);
            } while ((uVar5 != 0) && (iVar4 < local_24));
          }
          else {
LAB_55212df2:
            while( true ) {
              uVar2 = uVar5 * 2;
              if (CARRY4(uVar5,uVar5)) break;
              if (uVar2 == 0) goto LAB_55212e4c;
              iVar4 = iVar4 + 2;
              uVar5 = uVar2;
            }
            if (uVar2 != 0) goto code_r0x55212df8;
            puVar10 = puVar11 + 1;
            puVar8 = puVar9 + -1;
            *(ushort *)(iVar7 + iVar4) =
                 (*(ushort *)(iVar7 + iVar4) >> 1 & 0x7bef) +
                 (short)((*(ushort *)(iVar7 + iVar4) & 0xe79c) >> 2);
            if (puVar9 != (undefined1 *)0x1) {
              iVar4 = local_1c;
              local_1c = local_1c + 0x40;
              goto LAB_55212df0;
            }
LAB_55212e8a:
            if (local_1c < local_24) {
              uVar5 = puVar11[1];
              iVar4 = local_1c;
              goto LAB_55212e94;
            }
          }
LAB_55212ecd:
          in_stack_00000004 = in_stack_00000004 + -1;
          if (in_stack_00000004 == 0) {
            return;
          }
          iVar7 = iVar7 + iVar6;
          in_stack_0000000c = in_stack_0000000c + (int)puVar3;
        } while( true );
      }
      iVar6 = *(int *)(in_stack_00000010 + 0xc);
      iVar7 = *(int *)(in_stack_00000010 + 0x20) + in_stack_00000018 * 2 + in_stack_00000014 * iVar6
      ;
LAB_55212c72:
      local_1c = 0x40;
      iVar4 = 0;
      puVar8 = puVar3;
LAB_55212c7e:
      do {
        uVar5 = *in_stack_0000000c;
        do {
          while( true ) {
            while( true ) {
              uVar2 = uVar5 * 2;
              if (CARRY4(uVar5,uVar5)) break;
              if (uVar2 == 0) goto LAB_55212cda;
              iVar4 = iVar4 + 2;
              uVar5 = uVar2;
            }
            if (uVar2 == 0) {
              *(ushort *)(iVar7 + iVar4) =
                   (*(ushort *)(iVar7 + iVar4) >> 1 & 0x7bef) +
                   (short)((*(ushort *)(iVar7 + iVar4) & 0xe79c) >> 2);
              if (puVar8 == (undefined1 *)0x1) goto LAB_55212d18;
              iVar4 = local_1c;
              puVar8 = puVar8 + -1;
              in_stack_0000000c = in_stack_0000000c + 1;
              local_1c = local_1c + 0x40;
              goto LAB_55212c7e;
            }
            uVar5 = uVar5 * 4;
            if (!CARRY4(uVar2,uVar2)) break;
            puVar10 = (uint *)(iVar7 + iVar4);
            iVar4 = iVar4 + 4;
            *(uint *)(iVar7 + -4 + iVar4) =
                 (*puVar10 >> 1 & 0x7bef7bef) + ((*puVar10 & 0xe79ce79c) >> 2);
            if (uVar5 == 0) goto LAB_55212cda;
          }
          puVar1 = (ushort *)(iVar7 + iVar4);
          iVar4 = iVar4 + 4;
          *(ushort *)(iVar7 + iVar4) = (*puVar1 >> 1 & 0x7bef) + (short)((*puVar1 & 0xe79c) >> 2);
        } while (uVar5 != 0);
LAB_55212cda:
        if (puVar8 == (undefined1 *)0x1) goto LAB_55212d18;
        iVar4 = local_1c;
        puVar8 = puVar8 + -1;
        in_stack_0000000c = in_stack_0000000c + 1;
        local_1c = local_1c + 0x40;
      } while( true );
    }
  }
  return;
code_r0x55212df8:
  uVar5 = uVar5 * 4;
  if (CARRY4(uVar2,uVar2)) {
    *(uint *)(iVar7 + iVar4) =
         (*(uint *)(iVar7 + iVar4) >> 1 & 0x7bef7bef) +
         ((*(uint *)(iVar7 + iVar4) & 0xe79ce79c) >> 2);
  }
  else {
    *(ushort *)(iVar7 + iVar4 + 4) =
         (*(ushort *)(iVar7 + iVar4) >> 1 & 0x7bef) +
         (short)((*(ushort *)(iVar7 + iVar4) & 0xe79c) >> 2);
  }
  iVar4 = iVar4 + 4;
  if (uVar5 == 0) {
LAB_55212e4c:
    puVar10 = puVar11 + 1;
    puVar8 = puVar9 + -1;
    if (puVar9 == (undefined1 *)0x1) goto LAB_55212e8a;
    iVar4 = local_1c;
    local_1c = local_1c + 0x40;
LAB_55212df0:
    uVar5 = *puVar10;
    puVar9 = puVar8;
    puVar11 = puVar10;
  }
  goto LAB_55212df2;
LAB_55212d18:
  in_stack_0000000c = in_stack_0000000c + 1;
  in_stack_00000004 = in_stack_00000004 + -1;
  if (in_stack_00000004 == 0) {
    return;
  }
  iVar7 = iVar7 + iVar6;
  goto LAB_55212c72;
}

