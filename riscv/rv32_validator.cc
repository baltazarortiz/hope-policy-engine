#include "rv32_validator.h"

//extern "C" int32_t  decode(uint32_t ibits, uint32_t *rs1, uint32_t *rs2, uint32_t *rs3, uint32_t *rd, int32_t *imm, const char** name);

rv32_validator_t::rv32_validator_t(RegisterReader_t rr, MemoryReader_t mr) :
  abstract_renode_validator_t(rr, mr), ms_factory(&ms_cache) {
  ctx = (context_t *)malloc(sizeof(context_t));
  ops = (operands_t *)malloc(sizeof(operands_t));
  res = (results_t *)malloc(sizeof(results_t));

  meta_set_t *ms;
  ms = ms_factory.get_meta_set("requires.dover.Kernel.Code.ElfSection.SHF_EXECINSTR");
  tag_bus.add_provider(0x80000000, 0x100000 * 4,
		       new platform_ram_tag_provider_t(0x1000000 * 4, 4, m_to_t(ms)));
  ms = ms_factory.get_meta_set("requires.dover.riscv.Mach.Reg");
  ireg_tags.reset(m_to_t(ms));
  ms = ms_factory.get_meta_set("requires.dover.riscv.Mach.RegZero");
  ireg_tags[0] = m_to_t(ms);
  ms = ms_factory.get_meta_set("requires.dover.SOC.CSR.Default");
  csr_tags.reset(m_to_t(ms));
  ms = ms_factory.get_meta_set("requires.dover.riscv.Mach.PC");
  pc_tag = m_to_t(ms);
}

bool rv32_validator_t::validate(address_t pc, insn_bits_t insn) {
  int policy_result = POLICY_EXP_FAILURE;
  
  prepare_eval(pc, insn);
  
  policy_result = eval_policy(ctx, ops, res);
//  policy_result = POLICY_SUCCESS;

  if (policy_result == POLICY_SUCCESS) {
    complete_eval();
  }

//  if (policy_result != POLICY_SUCCESS)
//    handle_violation(ctx, ops, res);

  return policy_result == POLICY_SUCCESS;
//  return true;
}

void rv32_validator_t::commit() {
}

void rv32_validator_t::prepare_eval(address_t pc, insn_bits_t insn) {
  uint32_t rs1, rs2, rs3;
  int32_t imm;
  const char *name;
  address_t offset;
  tag_t ci_tag;

  int32_t flags;

  if (insn == 0x30200073) {
    flags = 0;
    name = "mret";
  }
  else
    flags = decode(insn, &rs1, &rs2, &rs3, &pending_RD, &imm, &name);
  printf("0x%x: 0x%08x   %s\n", pc, insn, name);

  if (flags & HAS_RS1) ops->op1 = t_to_m(ireg_tags[rs1]);
  if (flags & HAS_RS2) ops->op2 = t_to_m(ireg_tags[rs2]);
  if (flags & HAS_RS3) ops->op3 = t_to_m(ireg_tags[rs3]);
  has_pending_RD = (flags & HAS_RD) != 0;
  if (flags & (HAS_LOAD | HAS_STORE)) {
    address_t maddr = reg_reader(rs1);
    if (flags & HAS_IMM)
      maddr += imm;
    tag_t mtag;
    if (!tag_bus.load_tag(maddr, mtag)) {
      printf("failed to load MR tag\n");
    } else {
      ops->mem = t_to_m(mtag);
      printf("mr tag = 0x%p\n", ops->mem);
    }
  }

  if (tag_bus.load_tag(pc, ci_tag))
    printf("ci_tag: 0x%lx\n", ci_tag);
  else
    printf("panic\n");
  ctx->epc = pc;
  ctx->bad_addr = 0;
  ctx->cached = false;
  ops->ci = t_to_m(ci_tag);

  // hacking in the opgroup part of the metadata dynamically.
  meta_set_t *group_set = ms_factory.get_group_meta_set(name);
  ms_union(ops->ci, group_set);

  ops->pc = 0;
  ops->op1 = 0;
  ops->op2 = 0;
  ops->op3 = 0;
  ops->mem = 0;
}

void rv32_validator_t::complete_eval() {
}
