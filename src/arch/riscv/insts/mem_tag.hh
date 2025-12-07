#ifndef __ARCH_RISCV_INSTS_MEM_TAG_HH__
#define __ARCH_RISCV_INSTS_MEM_TAG_HH__

#include "arch/riscv/insts/static_inst.hh"
#include "cpu/static_inst.hh"

namespace gem5
{

namespace RiscvISA
{

class CheckedLoadMicro : public RiscvMicroInst
{
  protected:
    int64_t offset;
    unsigned dataSize; // in bytes
    RegId srcRegIdxArr[1];
    RegId destRegIdxArr[1];

  public:
    CheckedLoadMicro(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                     RegId _dest, RegId _base, int64_t _offset, unsigned _dataSize);

    Fault execute(ExecContext *xc, Trace::InstRecord *traceData) const override;
    Fault initiateAcc(ExecContext *xc, Trace::InstRecord *traceData) const override;
    Fault completeAcc(PacketPtr pkt, ExecContext *xc, Trace::InstRecord *traceData) const override;

    std::string generateDisassembly(Addr pc, const loader::SymbolTable *symtab) const override;
};

class CheckedStoreMicro : public RiscvMicroInst
{
  protected:
    int64_t offset;
    unsigned dataSize;
    RegId srcRegIdxArr[2];
    RegId destRegIdxArr[0];

  public:
    CheckedStoreMicro(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                      RegId _val, RegId _base, int64_t _offset, unsigned _dataSize);

    Fault execute(ExecContext *xc, Trace::InstRecord *traceData) const override;
    Fault initiateAcc(ExecContext *xc, Trace::InstRecord *traceData) const override;
    Fault completeAcc(PacketPtr pkt, ExecContext *xc, Trace::InstRecord *traceData) const override;

    std::string generateDisassembly(Addr pc, const loader::SymbolTable *symtab) const override;
};

class CheckedLoad : public RiscvMacroInst
{
  public:
    CheckedLoad(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                RegId _dest, RegId _base, int64_t _offset, unsigned _dataSize = 8);
};

class CheckedStore : public RiscvMacroInst
{
  public:
    CheckedStore(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                 RegId _val, RegId _base, int64_t _offset, unsigned _dataSize = 8);
};

} // namespace RiscvISA
} // namespace gem5