#include "arch/riscv/insts/mem_tag.hh"

#include <sstream>

#include "arch/riscv/utility.hh"
#include "cpu/exec_context.hh"
#include "sim/byteswap.hh"

namespace gem5
{

namespace RiscvISA
{

// CheckedLoadMicro

CheckedLoadMicro::CheckedLoadMicro(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                                   RegId _dest, RegId _base, int64_t _offset, unsigned _dataSize)
    : RiscvMicroInst(mnem, _machInst, __opClass), offset(_offset), dataSize(_dataSize)
{
    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&std::remove_pointer_t<decltype(this)>::srcRegIdxArr),
        reinterpret_cast<RegIdArrayPtr>(&std::remove_pointer_t<decltype(this)>::destRegIdxArr));

    _numSrcRegs = 0;
    _numDestRegs = 0;

    setDestRegIdx(_numDestRegs++, _dest);
    setSrcRegIdx(_numSrcRegs++, _base);

    flags[IsLoad] = true;
}

Fault
CheckedLoadMicro::execute(ExecContext *xc, Trace::InstRecord *traceData) const
{
    return initiateAcc(xc, traceData);
}

Fault
CheckedLoadMicro::initiateAcc(ExecContext *xc, Trace::InstRecord *traceData) const
{
    Addr EA = xc->getRegOperand(this, 0);
    uint8_t Mem = {};
    // Addr addr = base + offset;
    // Assuming standard flags for now
    // Request::Flags memAccessFlags = Request::PHYSICAL;

    return initiateMemRead(xc, traceData, EA, Mem, memAccessFlags);
}

Fault
CheckedLoadMicro::completeAcc(PacketPtr pkt, ExecContext *xc, Trace::InstRecord *traceData) const
{
    getMemAccPredicate(xc, traceData); // Update trace data if needed

//     uint64_t val = 0;
//     switch(dataSize) {
//         case 1: val = pkt->getLE<uint8_t>(); break;
//         case 2: val = pkt->getLE<uint16_t>(); break;
//         case 4: val = pkt->getLE<uint32_t>(); break;
//         case 8: val = pkt->getLE<uint64_t>(); break;
//         default: panic("Unsupported data size in CheckedLoadMicro");
//     }

    uint64_t val = pkt->getLE<uint8_t>();
    xc->setRegOperand(this, 0, val);

    if (traceData)
        traceData->setData(val);

    return NoFault;
}

std::string
CheckedLoadMicro::generateDisassembly(Addr pc, const loader::SymbolTable *symtab) const
{
    std::stringstream ss;
    ss << mnemonic << " " << registerName(destRegIdx(0)) << ", "
       << offset << "(" << registerName(srcRegIdx(0)) << ")";
    return ss.str();
}

// CheckedStoreMicro

CheckedStoreMicro::CheckedStoreMicro(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                                     RegId _val, RegId _base, int64_t _offset, unsigned _dataSize)
    : RiscvMicroInst(mnem, _machInst, __opClass), offset(_offset), dataSize(_dataSize)
{
    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&std::remove_pointer_t<decltype(this)>::srcRegIdxArr),
        reinterpret_cast<RegIdArrayPtr>(&std::remove_pointer_t<decltype(this)>::destRegIdxArr));

    _numSrcRegs = 0;
    _numDestRegs = 0;

    setSrcRegIdx(_numSrcRegs++, _base);
    setSrcRegIdx(_numSrcRegs++, _val); // Data to store

    flags[IsStore] = true;
}

Fault
CheckedStoreMicro::execute(ExecContext *xc, Trace::InstRecord *traceData) const
{
    return initiateAcc(xc, traceData);
}

Fault
CheckedStoreMicro::initiateAcc(ExecContext *xc, Trace::InstRecord *traceData) const
{
    Addr EA = xc->getRegOperand(this, 0);
//     Addr addr = base + offset;
    uint8_t Mem = xc->getRegOperand(this, 1);
//     Request::Flags memAccessFlags = Request::PHYSICAL;

    uint64_t val_le = htole(val);
    return  writeMemTimingLE(xc, traceData, Mem, EA, memAccessFlags, nullptr);
}

Fault
CheckedStoreMicro::completeAcc(PacketPtr pkt, ExecContext *xc, Trace::InstRecord *traceData) const
{
    return NoFault;
}

std::string
CheckedStoreMicro::generateDisassembly(Addr pc, const loader::SymbolTable *symtab) const
{
    std::stringstream ss;
    ss << mnemonic << " " << registerName(srcRegIdx(1)) << ", "
       << offset << "(" << registerName(srcRegIdx(0)) << ")";
    return ss.str();
}

// CheckedLoad Macro

CheckedLoad::CheckedLoad(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                         RegId _dest, RegId _base, int64_t _offset, unsigned _dataSize)
    : RiscvMacroInst(mnem, _machInst, __opClass)
{
    StaticInstPtr microop = new CheckedLoadMicro(
        "checked_load_micro", _machInst, __opClass, _dest, _base, _offset, _dataSize);
    microop->setFlag(IsLastMicroop);
    microops.push_back(microop);
}

// CheckedStore Macro

CheckedStore::CheckedStore(const char *mnem, ExtMachInst _machInst, OpClass __opClass,
                           RegId _val, RegId _base, int64_t _offset, unsigned _dataSize)
    : RiscvMacroInst(mnem, _machInst, __opClass)
{
    StaticInstPtr microop = new CheckedStoreMicro(
        "checked_store_micro", _machInst, __opClass, _val, _base, _offset, _dataSize);
    microop->setFlag(IsLastMicroop);
    microops.push_back(microop);
}

} // namespace RiscvISA
} // namespace gem5