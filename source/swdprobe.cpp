/*
 * Copyright (c) 2012, Anton Staaf, Cliff L. Biffle.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the project nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "target.h"
#include "swd_dp.h"
#include "swd_mpsse.h"
#include "swd.h"

#include "rptr.h"

#include "armv6m_v7m.h"
#include "arm.h"

#include "libs/error/error_stack.h"
#include "libs/log/log_default.h"
#include "libs/command_line/command_line.h"

#include <vector>

#include <unistd.h>
#include <stdio.h>
#include <ftdi.h>

using namespace Log;
using Err::Error;

using namespace ARM;
using namespace ARMv6M_v7M;

using std::vector;


/*******************************************************************************
 * Command line flags
 */
namespace CommandLine
{
    static Scalar<int>
    debug("debug", true, 0, "What level of debug logging to use.");

    static Scalar<String>
    programmer("programmer", true, "um232h", "FTDI-based programmer to use");

    static Scalar<int> vid("vid", true, 0, "FTDI VID");
    static Scalar<int> pid("pid", true, 0, "FTDI PID");

    static Scalar<int>
    interface("interface", true, 0, "Interface on FTDI chip");

    static Argument * arguments[] =
    {
        &debug,
        &programmer,
        &vid,
        &pid,
        &interface,
        NULL
    };
}

struct TargetInfo
{
    bool mem_ap_found;
    uint8_t mem_ap_index;

    enum arch_t
    {
        arch_undetermined = 0,
        arch_v6m = 1,
        arch_v7m = 2,
    };
    arch_t arch;

    enum cpu_t
    {
        cpu_undetermined = 0,
        cpu_cortex_m0 = 1,
        cpu_cortex_m3 = 2,
    };
    cpu_t cpu;

    unsigned major_revision;
    unsigned minor_revision;

    TargetInfo() :
        mem_ap_found(false),
        mem_ap_index(0) {}
};

Error probe_unknown_device(Target &           target,
                           rptr_const<word_t> regfile,
                           TargetInfo *       info);

/*******************************************************************************
 * Heuristically identifies the CPU and architecture revision.
 */
Error identify_cpu(Target & target, TargetInfo *info)
{
    if (info->arch != TargetInfo::arch_undetermined
        && info->cpu != TargetInfo::cpu_undetermined)
    {
        return Err::success;
    }

    rptr_const<word_t> cpuid_addr(SCB::CPUID);

    word_t cpuid;
    CheckRetry(target.read_word(cpuid_addr, &cpuid), 100);

    uint8_t  implementer = (cpuid >> 24) & 0x0FF;
    uint8_t  variant     = (cpuid >> 20) & 0x00F;
    uint8_t  arch_rev    = (cpuid >> 16) & 0x00F;
    uint16_t partno      = (cpuid >>  4) & 0xFFF;
    uint8_t  rev         = (cpuid >>  0) & 0x00F;

    if (implementer == 0x41
            && arch_rev == 0xC
            && partno == 0xC20)	// LPC1114
    {
        info->arch = TargetInfo::arch_v6m;
        info->cpu = TargetInfo::cpu_cortex_m0;
        info->major_revision = variant;
        info->minor_revision = rev;

        notice("CPU is Cortex-M0 r%up%u", variant, rev);
    }
    else if (implementer == 0x41
		&& arch_rev == 0xC
		&& partno == 0xC60)	// LPC810
    {
        info->arch = TargetInfo::arch_v6m;
        info->cpu = TargetInfo::cpu_cortex_m0;
        info->major_revision = variant;
        info->minor_revision = rev;
		
        notice("CPU is Cortex-M0 r%up%u", variant, rev);
    }
    else if (implementer == 0x41
            && arch_rev == 0xF
            && partno == 0xC23)
    {
        info->arch = TargetInfo::arch_v7m;
        info->cpu = TargetInfo::cpu_cortex_m3;
        info->major_revision = variant;
        info->minor_revision = rev;

        notice("CPU is Cortex-M3 r%up%u", variant, rev);
    }
    else
    {
        notice("Unrecognized CPUID info; heuristic detection failed.");
    }
    
    return Err::success;
}

/*******************************************************************************
 * Explores the ARMv7M Instrumentation Trace Macrocell.
 */
Error probe_v7m_itm(Target &           target,
                    rptr_const<word_t> base,
                    rptr_const<word_t> regfile,
                    size_t             size_in_bytes,
                    TargetInfo *       info)
{
    notice("  Component is ARMv7M ITM");

    return Err::success;
}

/*******************************************************************************
 * Explores the ARMv7M Embedded Trace Macrocell.
 */
Error probe_v7m_etm(Target &           target,
                    rptr_const<word_t> base,
                    rptr_const<word_t> regfile,
                    size_t             size_in_bytes,
                    TargetInfo *       info)
{
    notice("  Component is ARMv7M ETM");

    return Err::success;
}

/*******************************************************************************
 * Explores the ARMv7M Trace Port Interface Unit.
 */
Error probe_v7m_tpiu(Target &           target,
                     rptr_const<word_t> base,
                     rptr_const<word_t> regfile,
                     size_t             size_in_bytes,
                     TargetInfo *       info)
{
    notice("  Component is ARMv7M TPIU");

    return Err::success;
}

/*******************************************************************************
 * Explores the ARMv6M BreakPoint Unit and the ARMv7M Flash Patch and
 * Breakpoint unit.
 */
Error probe_bpu_fpb(Target &           target,
                    rptr_const<word_t> base,
                    rptr_const<word_t> regfile,
                    size_t             size_in_bytes,
                    TargetInfo *       info)
{
    notice("  Component is %s",
           info->arch == TargetInfo::arch_v7m ? "ARMv7M FPB"
                                                 : "ARMv6M BPU");

    word_t ctrl;
    Check(target.read_word(BPU::BP_CTRL, &ctrl));

    /*
     * Reassemble the number of code comparators (i.e. breakpoints).
     */
    unsigned code_count = BPU::BP_CTRL_NUM_CODE.extract(ctrl);
    if (info->arch == TargetInfo::arch_v7m)
    {
        unsigned high_bits = BPU::BP_CTRL_NUM_CODE2.extract(ctrl);
        code_count |= high_bits << 4;
    }

    notice("  Breakpoint count: %u", code_count);

    /*
     * ARMv7M uses a Harvard architecture internally, so it can distinguish
     * Flash fetches on the "I-Code" bus (breakpoints) from Flash fetches on the
     * "D-Code" bus (literals stored in the text).
     */
    unsigned literal_count = BPU::BP_CTRL_NUM_LIT.extract(ctrl);
    notice("  Literal watchpoint count: %u", literal_count);

    return Err::success;
}

/*******************************************************************************
 * Explores the ARMv6M/v7M Data Watchpoint and Trace unit.
 */
Error probe_v6m_v7m_dwt(Target &           target,
                        rptr_const<word_t> base,
                        rptr_const<word_t> regfile,
                        size_t             size_in_bytes,
                        TargetInfo *       info)
{
    notice("  Component is ARMv%dM DWT",
           info->arch == TargetInfo::arch_v7m ? 7 : 6);

    word_t ctrl;
    Check(target.read_word(DWT::DWT_CTRL, &ctrl));

    notice("  DWT comparator count   : %u",
           DWT::DWT_CTRL_NUMCOMP.extract(ctrl));

    if (info->arch == TargetInfo::arch_v7m)
    {
        notice("  Tracing/sampling       : %s",
               (ctrl & DWT::DWT_CTRL_NOTRCPKT)  ? "Unsupported" : "Supported");
        notice("  External match signals : %s",
               (ctrl & DWT::DWT_CTRL_NOEXTTRIG) ? "Unsupported" : "Supported");
        notice("  Cycle counter          : %s",
               (ctrl & DWT::DWT_CTRL_NOCYCCNT)  ? "Unsupported" : "Supported");
        notice("  Profiling counters     : %s",
               (ctrl & DWT::DWT_CTRL_NOPRFCNT)  ? "Unsupported" : "Supported");
    }

    return Err::success;
}

/*******************************************************************************
 * Explores the ARMv6M/v7M System Control Space, turning on power to other
 * devices so that enumeration can proceed.
 */
Error probe_v6m_v7m_scs(Target &           target,
                        rptr_const<word_t> base,
                        rptr_const<word_t> regfile,
                        size_t             size_in_bytes,
                        TargetInfo *       info)
{
    notice("  Component is ARMv%dM SCS",
           info->arch == TargetInfo::arch_v7m ? 7 : 6);

    // Enable DWT/ITM.
    word_t demcr;
    Check(target.read_word(DCB::DEMCR, &demcr));
    Check(target.write_word(DCB::DEMCR, demcr | DCB::DEMCR_DWTENA));

    return Err::success;
}

/*******************************************************************************
 * Explores a "Debug Component" heuristically.
 */
Error probe_debug_component(Target &           target,
                            rptr_const<word_t> base,
                            rptr_const<word_t> regfile,
                            size_t             size_in_bytes,
                            TargetInfo *       info)
{
    debug(1, "Applying heuristics to debug component at %08X", regfile.bits());

    Check(identify_cpu(target, info));

    if (info->arch == TargetInfo::arch_v7m)
    {
        switch (regfile.bits())
        {
            case 0xE0040000:
                return probe_v7m_tpiu(target,
                                      base,
                                      regfile,
                                      size_in_bytes,
                                      info);

            case 0xE0041000:
                return probe_v7m_etm(target,
                                     base,
                                     regfile,
                                     size_in_bytes,
                                     info);
        }
    }

    notice("Heuristic detection failed, component unknown.");

    return Err::success;
}
/*******************************************************************************
 * Explores a "Generic IP Component" heuristically.
 */
Error probe_generic_component(Target &           target,
                              rptr_const<word_t> base,
                              rptr_const<word_t> regfile,
                              size_t             size_in_bytes,
                              TargetInfo *       info)
{
    debug(1, "Applying heuristics to component at %08X", regfile.bits());

    Check(identify_cpu(target, info));

    if (info->arch == TargetInfo::arch_v7m
        || info->arch == TargetInfo::arch_v6m)
    {
        switch (regfile.bits())
        {
            case 0xE000E000:
                return probe_v6m_v7m_scs(target,
                                         base,
                                         regfile,
                                         size_in_bytes,
                                         info);

            case 0xE0001000:
                return probe_v6m_v7m_dwt(target,
                                         base,
                                         regfile,
                                         size_in_bytes,
                                         info);

            case 0xE0002000:
                return probe_bpu_fpb(target,
                                     base,
                                     regfile,
                                     size_in_bytes,
                                     info);
        }

    }

    if (info->arch == TargetInfo::arch_v7m)
    {
        switch (regfile.bits())
        {
            case 0xE0000000:
                return probe_v7m_itm(target,
                                     base,
                                     regfile,
                                     size_in_bytes,
                                     info);
        }
    }

    notice("Heuristic detection failed, component unknown.");

    return Err::success;
}

/*******************************************************************************
 * Explores a ROM table, descending to its devices.
 */
Error probe_rom_table(Target &           target,
                      rptr_const<word_t> base,
                      rptr_const<word_t> regfile,
                      size_t             size_in_bytes,
                      TargetInfo *       info)
{
    notice("  Device is ROM table: %zu bytes", size_in_bytes);

    if (size_in_bytes != 4096)
    {
        warning("ROM Tables in ADIv5 are always 4096 bytes!  What is this?");
        return Err::success;
    }

    unsigned const memtype_index = 0xFCC / sizeof(word_t);
    word_t         memtype;
    CheckRetry(target.read_word(base + memtype_index, &memtype), 100);

    if ((memtype & 1) != 1)
    {
        warning("MEM-AP contains only debug support, no memory!  "
                "(swdprobe does not understand this.)");
        return Err::success;
    }


    // ADIv5 says offsets starting at 0xFCB are reserved.
    unsigned const max_rom_table_entries = 0xFCB / sizeof(word_t);
    vector<int32_t> entry_offsets;

    for (unsigned i = 0; i < max_rom_table_entries; ++i)
    {
        word_t entry;
        CheckRetry(target.read_word(base + i, &entry), 100);

        if (entry == 0) break;

        if ((entry & (1 << 1)) == 0)
        {
            warning("Found 8-bit ROM table: not supported by swdprobe!");
            return Err::success;
        }

        if (entry & 1)  // Present?
        {
            debug(2, "Table entry %u = %08X", i, entry);
            entry_offsets.push_back(entry & ~0xFFF);
        }
    }

    for (vector<int32_t>::iterator it = entry_offsets.begin();
         it != entry_offsets.end();
         ++it)
    {
        rptr_const<word_t> child_regfile(base + (*it / sizeof(word_t)));
        Check(probe_unknown_device(target, child_regfile, info));
    }

    return Err::success;
}

/*******************************************************************************
 * Explores a peripheral or ROM table through a MEM-AP.
 */

Error probe_unknown_device(Target &           target,
                           rptr_const<word_t> regfile,
                           TargetInfo *       info)
{
    notice("Device @%08X", regfile.bits());

    word_t component_id[4];

    unsigned const component_id_index = 0xFF0 / sizeof(word_t);

    CheckRetry(target.read_words(regfile + component_id_index,
                                 component_id,
                                 4),
               100);

    if (component_id[0] != 0x0D
        || component_id[2] != 0x05
        || component_id[3] != 0xB1)
    {
        warning("Unexpected component ID preamble; legacy peripheral at %08X?",
                regfile.bits());
        return Err::success;
    }

    unsigned const peripheral_id4_index = 0xFD0 / sizeof(word_t);

    word_t peripheral_id4;
    CheckRetry(target.read_word(regfile + peripheral_id4_index,
                                &peripheral_id4),
               100);

    unsigned log2_size_in_blocks = (peripheral_id4 >> 4) & 0xF;
    unsigned size_in_blocks = 1 << log2_size_in_blocks;
    unsigned size_in_bytes = 4096 * size_in_blocks;

    rptr_const<word_t> base(regfile - (size_in_bytes / sizeof(word_t))
                                    + (4096 / sizeof(word_t)));

    uint8_t component_class = (component_id[1] >> 4) & 0xF;
    switch (component_class)
    {
        case 0x1:
            Check(probe_rom_table(target, base, regfile, size_in_bytes, info));
            break;

        case 0xE:
            Check(probe_generic_component(target,
                                          base,
                                          regfile,
                                          size_in_bytes,
                                          info));
            break;

        case 0x9:
            Check(probe_debug_component(target,
                                        base,
                                        regfile,
                                        size_in_bytes,
                                        info));
            break;

        default:
            notice("  Unknown component class %X", component_class);
            break;
    }
    
    return Err::success;
}

/*******************************************************************************
 * Explores a MEM-AP.
 */

Error probe_mem_ap(SWDDriver &       swd,
                   DebugAccessPort & dap,
                   uint8_t           ap_index,
                   TargetInfo *      info)
{
    word_t csw;
    Check(dap.start_read_ap(ap_index, 0x00));
    Check(dap.read_rdbuff(&csw));
    debug(1, "CSW = %08X", csw);

    word_t cfg;
    Check(dap.start_read_ap(ap_index, 0xF4));
    Check(dap.read_rdbuff(&cfg));
    debug(1, "CFG = %08X", cfg);

    word_t base;
    Check(dap.start_read_ap(ap_index, 0xF8));
    Check(dap.read_rdbuff(&base));
    debug(1, "BASE = %08X", base);

    if ((base & 0x3) != 0x3)
    {
        warning("MEM-AP #%u uses pre-ADIv5 legacy interface; skipping!",
                ap_index);
    }
    else
    {
        // Invasively reconfigure this MEM-AP.
        Target target(swd, dap, ap_index);
        rptr_const<word_t> regfile(base & ~0xFFF);

        Check(target.initialize(false));

        Check(identify_cpu(target, info));

        // Treat this peripheral as "unknown" to use type dispatching.
        Check(probe_unknown_device(target, regfile, info));
    }

    return Err::success;
}

/*******************************************************************************
 * Performs the initial probe and sanity checks, while target reset is asserted.
 */

Error early_probe_dap(SWDDriver & swd, DebugAccessPort & dap, TargetInfo * info)
{
    notice("Scanning for connected Access Ports...");

    for (unsigned i = 0; i < 1/*256*/; ++i)
    {
        debug(2, "Trying Access Port #%u", i);

        uint32_t ap_idr;
        CheckRetry(dap.start_read_ap(i, 0xFC), 100);
        Check(dap.read_rdbuff(&ap_idr));

        if (ap_idr)
        {
            notice("Access Port #%u: IDR = %08X", i, ap_idr);
            if (ap_idr & (1 << 16))  // Describes itself as a MEM-AP
            {
                if (info->mem_ap_found)
                {
                    warning("This system has two MEM-APs.  swdprobe doesn't "
                            "handle this well.  Ignoring it!");
                }
                else
                {
                    notice("  Found MEM-AP.");
                    info->mem_ap_found = true;
                    info->mem_ap_index = i;
                }

                Check(probe_mem_ap(swd, dap, i, info));
            }
        }
        else
        {
            debug(2, "Access Port #%u not implemented (IDR=0)", i);
        }
    }

    return Err::success;
}

/*******************************************************************************
 * Outermost probe logic -- factored out of error_main to simplify its control
 * flow between labels.
 */
Error probe_main(SWDDriver & swd)
{
    DebugAccessPort dap(swd);
    TargetInfo      info;

    uint32_t idcode;
    Check(swd.initialize(&idcode));

    notice("SWD communications initialized successfully.");
    notice("SWD-DP IDCODE = %08X", idcode);
    notice("  Version:   %X", idcode >> 28);
    notice("  Part:      %X", (idcode >> 12) & 0xFFFF);
    notice("  Designer:  %X", (idcode >> 1) & 0x7FF);

    Check(swd.enter_reset());
    usleep(10000);
    Check(dap.reset_state());

    Check(early_probe_dap(swd, dap, &info));

    Check(swd.leave_reset());

    return Err::success;
}


/*******************************************************************************
 * Entry point (sort of -- see main below)
 */

static Error error_main(int argc, char const * * argv)
{
    MPSSEConfig config;
    MPSSE       mpsse;

    Check(lookup_programmer(CommandLine::programmer.get(), &config));

    if (CommandLine::interface.set())
        config.interface = CommandLine::interface.get();

    if (CommandLine::vid.set())
        config.vid = CommandLine::vid.get();

    if (CommandLine::pid.set())
        config.pid = CommandLine::pid.get();

    Check(mpsse.open(config));

    MPSSESWDDriver swd(config, &mpsse);

    Check(probe_main(swd));

    return Err::success;
}

/******************************************************************************/
int main(int argc, char const * * argv)
{
    Error check_error = Err::success;

    CheckCleanup(CommandLine::parse(argc, argv, CommandLine::arguments),
                 failure);

    log().set_level(CommandLine::debug.get());

    CheckCleanup(error_main(argc, argv), failure);
    return 0;

failure:
    Err::stack()->print();
    return 1;
}
/******************************************************************************/
