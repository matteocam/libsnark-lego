/** @file
 *****************************************************************************

 Implementation of interfaces for a compliance predicate for R1CS PCD.

 See compliance_predicate.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef COMPLIANCE_PREDICATE_TCC_
#define COMPLIANCE_PREDICATE_TCC_

#include "common/utils.hpp"
#include "zk_proof_systems/pcd/r1cs_pcd/r1cs_sp_ppzkpcd/r1cs_sp_ppzkpcd_params.hpp"

namespace libsnark {

template<typename FieldT>
class r1cs_pcd_compliance_predicate_primary_input;

template<typename FieldT>
class r1cs_pcd_compliance_predicate_auxiliary_input;

template<typename FieldT>
void r1cs_pcd_message<FieldT>::print() const
{
    printf("PCD message:\n");
    printf("  Type: %zu\n", type);

    printf("  Payload\n");
    for (auto &elt: payload)
    {
        elt.print();
    }
}

template<typename FieldT>
r1cs_pcd_compliance_predicate<FieldT>::r1cs_pcd_compliance_predicate(const size_t name,
                                                                     const size_t type,
                                                                     const r1cs_constraint_system<FieldT> &constraint_system,
                                                                     const size_t outgoing_message_payload_length,
                                                                     const size_t max_arity,
                                                                     const std::vector<size_t> &incoming_message_payload_lengths,
                                                                     const size_t local_data_length,
                                                                     const size_t witness_length,
                                                                     const bool relies_on_same_type_inputs) :
    name(name),
    type(type),
    constraint_system(constraint_system),
    outgoing_message_payload_length(outgoing_message_payload_length),
    max_arity(max_arity),
    incoming_message_payload_lengths(incoming_message_payload_lengths),
    local_data_length(local_data_length),
    witness_length(witness_length),
    relies_on_same_type_inputs(relies_on_same_type_inputs)
{
    assert(max_arity == incoming_message_payload_lengths.size());
}

template<typename FieldT>
bool r1cs_pcd_compliance_predicate<FieldT>::is_well_formed() const
{
    const bool type_not_zero = (type != 0);
    const bool incoming_message_payload_lengths_well_specified = (incoming_message_payload_lengths.size() == max_arity);

    size_t all_messages_length = outgoing_message_payload_length;
    for (size_t i = 0; i < incoming_message_payload_lengths.size(); ++i)
    {
        all_messages_length += incoming_message_payload_lengths[i];
    }
    const size_t type_vec_length = max_arity+1;
    const size_t arity_length = 1;

    const bool correct_num_inputs = ((outgoing_message_payload_length + 1) == constraint_system.num_inputs());
    const bool correct_num_variables = ((all_messages_length + local_data_length + type_vec_length + arity_length + witness_length) == constraint_system.num_variables());

#ifdef DEBUG
    printf("type_not_zero: %d\n", type_not_zero);
    printf("incoming_message_payload_lengths_well_specified: %d\n", incoming_message_payload_lengths_well_specified);
    printf("correct_num_inputs: %d (outgoing_message_payload_length = %zu, constraint_system.num_inputs() = %zu)\n",
           correct_num_inputs, outgoing_message_payload_length, constraint_system.num_inputs());
    printf("correct_num_variables: %d (all_messages_length = %zu, local_data_length = %zu, type_vec_length = %zu, arity_length = %zu, witness_length = %zu, constraint_system.num_variables() = %zu)\n",
           correct_num_variables,
           all_messages_length, local_data_length, type_vec_length, arity_length, witness_length,
           constraint_system.num_variables());
#endif

    return (type_not_zero && incoming_message_payload_lengths_well_specified && correct_num_inputs && correct_num_variables);
}

template<typename FieldT>
bool r1cs_pcd_compliance_predicate<FieldT>::has_equal_input_and_output_lengths() const
{
    for (size_t i = 0; i < incoming_message_payload_lengths.size(); ++i)
    {
        if (incoming_message_payload_lengths[i] != outgoing_message_payload_length)
        {
            return false;
        }
    }

    return true;
}

template<typename FieldT>
bool r1cs_pcd_compliance_predicate<FieldT>::has_equal_input_lengths() const
{
    for (size_t i = 1; i < incoming_message_payload_lengths.size(); ++i)
    {
        if (incoming_message_payload_lengths[i] != incoming_message_payload_lengths[0])
        {
            return false;
        }
    }

    return true;
}

template<typename FieldT>
bool r1cs_pcd_compliance_predicate<FieldT>::operator==(const r1cs_pcd_compliance_predicate<FieldT> &other) const
{
    return (this->name == other.name &&
            this->type == other.type &&
            this->constraint_system == other.constraint_system &&
            this->max_arity == other.max_arity &&
            vectors_equal<size_t>(this->incoming_message_payload_lengths, other.incoming_message_payload_lengths) &&
            this->outgoing_message_payload_length == other.outgoing_message_payload_length &&
            this->local_data_length == other.local_data_length &&
            this->witness_length == other.witness_length);
}

template<typename FieldT>
std::ostream& operator<<(std::ostream &out, const r1cs_pcd_compliance_predicate<FieldT> &cp)
{
    out << cp.name << "\n";
    out << cp.type << "\n";
    out << cp.max_arity << "\n";
    assert(cp.max_arity == cp.incoming_message_payload_lengths.size());
    for (size_t i = 0; i < cp.max_arity; ++i)
    {
        out << cp.incoming_message_payload_lengths[i] << "\n";
    }
    out << cp.outgoing_message_payload_length << "\n";
    out << cp.local_data_length << "\n";
    out << cp.witness_length << "\n";
    out << cp.constraint_system << "\n";

    return out;
}

template<typename FieldT>
std::istream& operator>>(std::istream &in, r1cs_pcd_compliance_predicate<FieldT> &cp)
{
    in >> cp.name;
    consume_newline(in);
    in >> cp.type;
    consume_newline(in);
    in >> cp.max_arity;
    consume_newline(in);
    cp.incoming_message_payload_lengths.resize(cp.max_arity);
    for (size_t i = 0; i < cp.max_arity; ++i)
    {
        in >> cp.incoming_message_payload_lengths[i];
        consume_newline(in);
    }
    in >> cp.outgoing_message_payload_length;
    consume_newline(in);
    in >> cp.local_data_length;
    consume_newline(in);
    in >> cp.witness_length;
    consume_newline(in);
    in >> cp.constraint_system;
    consume_newline(in);

    return in;
}

template<typename FieldT>
bool r1cs_pcd_compliance_predicate<FieldT>::is_satisfied(const r1cs_pcd_message<FieldT> &outgoing_message,
                                                         const std::vector<r1cs_pcd_message<FieldT> > &incoming_messages,
                                                         const r1cs_pcd_local_data<FieldT> &local_data,
                                                         const r1cs_pcd_witness<FieldT> &witness) const
{
    assert(outgoing_message.payload.size() == outgoing_message_payload_length);
    assert(incoming_messages.size() <= max_arity);
    for (size_t i = 0; i < incoming_messages.size(); ++i)
    {
        assert(incoming_messages[i].payload.size() == incoming_message_payload_lengths[i]);
    }
    assert(local_data.payload.size() == local_data_length);

    r1cs_pcd_compliance_predicate_primary_input<FieldT> cp_primary_input;
    cp_primary_input.outgoing_message = outgoing_message;

    r1cs_pcd_compliance_predicate_auxiliary_input<FieldT> cp_auxiliary_input;
    cp_auxiliary_input.incoming_messages = incoming_messages;
    cp_auxiliary_input.local_data = local_data;
    cp_auxiliary_input.witness = witness;

    return constraint_system.is_satisfied(cp_primary_input.as_r1cs_primary_input(),
                                          cp_auxiliary_input.as_r1cs_auxiliary_input(incoming_message_payload_lengths));
}

} // libsnark

#endif //  COMPLIANCE_PREDICATE_TCC_
