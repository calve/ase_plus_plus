#!/bin/bash
KVM_HOME=../../kvm  # Maybe your kvm_home is elsewhere
KERNEL=${1-$KVM_HOME/build/kvm32/arch/x86/boot/bzImage}
SMP=${2-2}

qemu-system-x86_64 -enable-kvm -smp $SMP -m 2048 -boot c \
  -hda $KVM_HOME/debian32.img \
  -net nic -net user,hostfwd=tcp::10022-:22 \
  -serial stdio \
  -kernel $KERNEL -append "root=/dev/sda1 console=ttyS0 rw" \
  -display none
