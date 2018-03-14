/*
 * Copyright © 2017-2018 Dover Microsystems, Inc.
 * All rights reserved. 
 *
 * Use and disclosure subject to the following license. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef RV32_VALIDATOR_H
#define RV32_VALIDATOR_H

#include <stdio.h>
#include <string>

#include "soc_tag_configuration.h"
#include "tag_based_validator.h"
#include "tag_converter.h"
#include "policy_eval.h"
#include "metadata_memory_map.h"

namespace policy_engine {

#define REG_SP 2
class rv32_validator_t : public tag_based_validator_t {
  context_t *ctx;
  operands_t *ops;
  results_t *res;
  tag_bus_t tag_bus;
  tag_file_t<32> ireg_tags;
  tag_file_t<0x1000> csr_tags;
  tag_t pc_tag;
  uint32_t pending_RD;
  address_t mem_addr;
  uint32_t pending_CSR;
  bool has_pending_RD;
  bool has_pending_mem;
  bool has_pending_CSR;
//  meta_set_t temp_ci_tag;

  void handle_violation(context_t *ctx, operands_t *ops);
  
  public:
  rv32_validator_t(meta_set_cache_t *ms_cache,
		   meta_set_factory_t *ms_factory,
		   soc_tag_configuration_t *tag_config,
		   RegisterReader_t rr);

  void apply_metadata(metadata_memory_map_t *md_map);
  virtual ~rv32_validator_t() {
    free(ctx);
    free(ops);
    free(res);
  }
  bool validate(address_t pc, insn_bits_t insn);
  void commit();
  
  // Provides the tag for a given address.  Used for debugging.
  virtual bool get_tag(address_t addr, tag_t &tag) {
    return tag_bus.load_tag(addr, tag);
  }

  void prepare_eval(address_t pc, insn_bits_t insn);
  void complete_eval();

  // fields used by main.cc
  bool failed;
  context_t failed_ctx;
  operands_t failed_ops;

};

} // namespace policy_engine

#endif
