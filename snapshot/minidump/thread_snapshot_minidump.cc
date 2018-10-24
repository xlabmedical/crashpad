// Copyright 2018 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "snapshot/minidump/thread_snapshot_minidump.h"

#include <stddef.h>
#include <string.h>

#include "minidump/minidump_context.h"

namespace crashpad {
namespace internal {

ThreadSnapshotMinidump::ThreadSnapshotMinidump()
    : ThreadSnapshot(),
      minidump_thread_(),
      context_(),
      context_memory_(),
      stack_(),
      initialized_() {
}

ThreadSnapshotMinidump::~ThreadSnapshotMinidump() {
}

bool ThreadSnapshotMinidump::Initialize(FileReaderInterface* file_reader,
                                        RVA minidump_thread_rva,
                                        CPUArchitecture arch) {
  INITIALIZATION_STATE_SET_INITIALIZING(initialized_);
  std::vector<unsigned char> minidump_context;

  context_.architecture = arch;

  if (!file_reader->SeekSet(minidump_thread_rva)) {
    return false;
  }

  if (!file_reader->ReadExactly(&minidump_thread_, sizeof(minidump_thread_))) {
    return false;
  }

  if (!file_reader->SeekSet(minidump_thread_.ThreadContext.Rva)) {
    return false;
  }

  minidump_context.resize(minidump_thread_.ThreadContext.DataSize);

  if (!file_reader->ReadExactly(minidump_context.data(),
                                minidump_context.size())) {
    return false;
  }

  if (!InitializeContext(minidump_context)) {
    return false;
  }

  RVA stack_info_location = minidump_thread_rva +
    offsetof(MINIDUMP_THREAD, Stack);

  if (!stack_.Initialize(file_reader, stack_info_location)) {
    return false;
  }

  INITIALIZATION_STATE_SET_VALID(initialized_);
  return true;
}

bool ThreadSnapshotMinidump::InitializeContext(
    const std::vector<unsigned char>& minidump_context) {
  if (minidump_context.size() == 0) {
    // Thread has no context.
    context_.architecture = CPUArchitecture::kCPUArchitectureUnknown;
    return true;
  }

  if (context_.architecture == CPUArchitecture::kCPUArchitectureX86) {
    LOG(WARNING) << "Snapshot X86 context support has no unit tests.";
    context_memory_.resize(sizeof(CPUContextX86));
    context_.x86 = reinterpret_cast<CPUContextX86*>(context_memory_.data());
    const MinidumpContextX86* src =
      reinterpret_cast<const MinidumpContextX86*>(minidump_context.data());
    if (minidump_context.size() < sizeof(MinidumpContextX86)) {
      return false;
    }

    if (!(src->context_flags & kMinidumpContextX86)) {
      return false;
    }

    if (src->context_flags & kMinidumpContextX86Extended) {
      context_.x86->fxsave = src->fxsave;
    } else if (src->context_flags & kMinidumpContextX86FloatingPoint) {
      CPUContextX86::FsaveToFxsave(src->fsave, &context_.x86->fxsave);
    }

    context_.x86->eax = src->eax;
    context_.x86->ebx = src->ebx;
    context_.x86->ecx = src->ecx;
    context_.x86->edx = src->edx;
    context_.x86->edi = src->edi;
    context_.x86->esi = src->esi;
    context_.x86->ebp = src->ebp;
    context_.x86->esp = src->esp;
    context_.x86->eip = src->eip;
    context_.x86->eflags = src->eflags;
    context_.x86->cs = static_cast<uint16_t>(src->cs);
    context_.x86->ds = static_cast<uint16_t>(src->ds);
    context_.x86->es = static_cast<uint16_t>(src->es);
    context_.x86->fs = static_cast<uint16_t>(src->fs);
    context_.x86->gs = static_cast<uint16_t>(src->gs);
    context_.x86->ss = static_cast<uint16_t>(src->ss);
    context_.x86->dr0 = src->dr0;
    context_.x86->dr1 = src->dr1;
    context_.x86->dr2 = src->dr2;
    context_.x86->dr3 = src->dr3;
    context_.x86->dr6 = src->dr6;
    context_.x86->dr7 = src->dr7;

    // Minidump passes no value for dr4/5. Our output context has space for
    // them. According to spec they're obsolete, but when present read as
    // aliases for dr6/7, so we'll do this.
    context_.x86->dr4 = src->dr6;
    context_.x86->dr5 = src->dr7;
  } else if (context_.architecture == CPUArchitecture::kCPUArchitectureX86_64) {
    context_memory_.resize(sizeof(CPUContextX86_64));
    context_.x86_64 =
      reinterpret_cast<CPUContextX86_64*>(context_memory_.data());
    const MinidumpContextAMD64* src =
      reinterpret_cast<const MinidumpContextAMD64*>(minidump_context.data());
    if (minidump_context.size() < sizeof(MinidumpContextAMD64)) {
      return false;
    }

    if (!(src->context_flags & kMinidumpContextAMD64)) {
      return false;
    }

    context_.x86_64->fxsave = src->fxsave;
    context_.x86_64->cs = src->cs;
    context_.x86_64->fs = src->fs;
    context_.x86_64->gs = src->gs;
    context_.x86_64->rflags = src->eflags;
    context_.x86_64->dr0 = src->dr0;
    context_.x86_64->dr1 = src->dr1;
    context_.x86_64->dr2 = src->dr2;
    context_.x86_64->dr3 = src->dr3;
    context_.x86_64->dr6 = src->dr6;
    context_.x86_64->dr7 = src->dr7;
    context_.x86_64->rax = src->rax;
    context_.x86_64->rcx = src->rcx;
    context_.x86_64->rdx = src->rdx;
    context_.x86_64->rbx = src->rbx;
    context_.x86_64->rsp = src->rsp;
    context_.x86_64->rbp = src->rbp;
    context_.x86_64->rsi = src->rsi;
    context_.x86_64->rdi = src->rdi;
    context_.x86_64->r8 = src->r8;
    context_.x86_64->r9 = src->r9;
    context_.x86_64->r10 = src->r10;
    context_.x86_64->r11 = src->r11;
    context_.x86_64->r12 = src->r12;
    context_.x86_64->r13 = src->r13;
    context_.x86_64->r14 = src->r14;
    context_.x86_64->r15 = src->r15;
    context_.x86_64->rip = src->rip;

    // See comments on x86 above.
    context_.x86_64->dr4 = src->dr6;
    context_.x86_64->dr5 = src->dr7;
  } else if (context_.architecture == CPUArchitecture::kCPUArchitectureARM) {
    LOG(WARNING) << "Snapshot ARM32 context support has no unit tests.";
    context_memory_.resize(sizeof(CPUContextARM));
    context_.arm = reinterpret_cast<CPUContextARM*>(context_memory_.data());
    const MinidumpContextARM* src =
      reinterpret_cast<const MinidumpContextARM*>(minidump_context.data());
    if (minidump_context.size() < sizeof(MinidumpContextARM)) {
      return false;
    }

    if (!(src->context_flags & kMinidumpContextARM)) {
      return false;
    }

    for (size_t i = 0; i < arraysize(src->regs); i++) {
      context_.arm->regs[i] = src->regs[i];
    }

    context_.arm->fp = src->fp;
    context_.arm->ip = src->ip;
    context_.arm->sp = src->sp;
    context_.arm->lr = src->lr;
    context_.arm->pc = src->pc;
    context_.arm->cpsr = src->cpsr;
    context_.arm->vfp_regs.fpscr = src->fpscr;

    for (size_t i = 0; i < arraysize(src->vfp); i++) {
      context_.arm->vfp_regs.vfp[i] = src->vfp[i];
    }

    context_.arm->have_fpa_regs = false;
    context_.arm->have_vfp_regs =
      !!(src->context_flags & kMinidumpContextARMVFP);
  } else if (context_.architecture == CPUArchitecture::kCPUArchitectureARM64) {
    context_memory_.resize(sizeof(CPUContextARM64));
    context_.arm64= reinterpret_cast<CPUContextARM64*>(context_memory_.data());
    const MinidumpContextARM64* src =
      reinterpret_cast<const MinidumpContextARM64*>(minidump_context.data());
    if (minidump_context.size() < sizeof(MinidumpContextARM64)) {
      return false;
    }

    if (!(src->context_flags & kMinidumpContextARM64)) {
      return false;
    }

    for (size_t i = 0; i < arraysize(src->regs); i++) {
      context_.arm64->regs[i] = src->regs[i];
    }

    context_.arm64->regs[29] = src->fp;
    context_.arm64->regs[30] = src->lr;

    for (size_t i = 0; i < arraysize(src->fpsimd); i++) {
      context_.arm64->fpsimd[i] = src->fpsimd[i];
    }

    context_.arm64->sp = src->sp;
    context_.arm64->pc = src->pc;
    context_.arm64->fpcr = src->fpcr;
    context_.arm64->fpsr = src->fpsr;

    // Seems we don't get a full PSTATE but it looks like this assignment
    // should give something useful at least.
    context_.arm64->pstate = src->cpsr;
  } else if (context_.architecture == CPUArchitecture::kCPUArchitectureMIPSEL) {
    LOG(WARNING) << "Snapshot MIPS context support has no unit tests.";
    context_memory_.resize(sizeof(CPUContextMIPS));
    context_.mipsel = reinterpret_cast<CPUContextMIPS*>(context_memory_.data());
    const MinidumpContextMIPS* src =
      reinterpret_cast<const MinidumpContextMIPS*>(minidump_context.data());
    if (minidump_context.size() < sizeof(MinidumpContextMIPS)) {
      return false;
    }

    if (!(src->context_flags & kMinidumpContextMIPS)) {
      return false;
    }

    for (size_t i = 0; i < arraysize(src->regs); i++) {
      context_.mipsel->regs[i] = src->regs[i];
    }

    context_.mipsel->mdhi = static_cast<uint32_t>(src->mdhi);
    context_.mipsel->mdlo = static_cast<uint32_t>(src->mdlo);
    context_.mipsel->dsp_control = src->dsp_control;

    for (size_t i = 0; i < arraysize(src->hi); i++) {
      context_.mipsel->hi[i] = src->hi[i];
      context_.mipsel->lo[i] = src->lo[i];
    }

    context_.mipsel->cp0_epc = static_cast<uint32_t>(src->epc);
    context_.mipsel->cp0_badvaddr = static_cast<uint32_t>(src->badvaddr);
    context_.mipsel->cp0_status = src->status;
    context_.mipsel->cp0_cause = src->cause;
    context_.mipsel->fpcsr = src->fpcsr;
    context_.mipsel->fir = src->fir;

    memcpy(&context_.mipsel->fpregs, &src->fpregs, sizeof(src->fpregs));
  } else if (context_.architecture ==
             CPUArchitecture::kCPUArchitectureMIPS64EL) {
    LOG(WARNING) << "Snapshot MIPS64 context support has no unit tests.";
    context_memory_.resize(sizeof(CPUContextMIPS64));
    context_.mips64 =
      reinterpret_cast<CPUContextMIPS64*>(context_memory_.data());
    const MinidumpContextMIPS64* src =
      reinterpret_cast<const MinidumpContextMIPS64*>(minidump_context.data());
    if (minidump_context.size() < sizeof(MinidumpContextMIPS64)) {
      return false;
    }

    if (!(src->context_flags & kMinidumpContextMIPS64)) {
      return false;
    }

    for (size_t i = 0; i < arraysize(src->regs); i++) {
      context_.mips64->regs[i] = src->regs[i];
    }

    context_.mips64->mdhi = src->mdhi;
    context_.mips64->mdlo = src->mdlo;
    context_.mips64->dsp_control = src->dsp_control;

    for (size_t i = 0; i < arraysize(src->hi); i++) {
      context_.mips64->hi[i] = src->hi[i];
      context_.mips64->lo[i] = src->lo[i];
    }

    context_.mips64->cp0_epc = src->epc;
    context_.mips64->cp0_badvaddr = src->badvaddr;
    context_.mips64->cp0_status = src->status;
    context_.mips64->cp0_cause = src->cause;
    context_.mips64->fpcsr = src->fpcsr;
    context_.mips64->fir = src->fir;

    memcpy(&context_.mips64->fpregs, &src->fpregs, sizeof(src->fpregs));
  }

  // If we fell through, Architecture is listed as "unknown".
  return true;
}

uint64_t ThreadSnapshotMinidump::ThreadID() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return minidump_thread_.ThreadId;
}

int ThreadSnapshotMinidump::SuspendCount() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return minidump_thread_.SuspendCount;
}

uint64_t ThreadSnapshotMinidump::ThreadSpecificDataAddress() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return minidump_thread_.Teb;
}

int ThreadSnapshotMinidump::Priority() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return minidump_thread_.Priority;
}

const CPUContext* ThreadSnapshotMinidump::Context() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return &context_;
}

const MemorySnapshot* ThreadSnapshotMinidump::Stack() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return &stack_;
}

std::vector<const MemorySnapshot*> ThreadSnapshotMinidump::ExtraMemory() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  // This doesn't correspond to anything minidump can give us, with the
  // exception of the BackingStore field in the MINIDUMP_THREAD_EX structure,
  // which is only valid for IA-64.
  return std::vector<const MemorySnapshot*>();
}

}  // namespace internal
}  // namespace crashpad
