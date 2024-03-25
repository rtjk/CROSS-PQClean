#!/usr/bin/python3
# This script computes all the derived parameters for CROSS which cannot be
# automated via C preprocessor macros
# Such parameters include the number of nodes for each level of the seed/Merkle
# trees and the amount of randomness required to be extracted from the CSPRNGs


# arbitrary precision floating point library
from mpmath import mp
from math import comb,log2,ceil
import csv
from sys import argv,exit

# number of bits to represent a given number
def bits_to_represent(p):
    return ceil(log2(p+1))

# number of bits to represent an element of Z_p
def bits_for_element_mod(p):
    # it's the number of bits to represent p-1
    return bits_to_represent(p-1)

def mpmath_log2(a):
    return mp.log(a)/mp.log(mp.mpf(2))

# find number of bits to draw from the CSPRNG to obtain at least n
# values mod p via rejection sampling, with Pr at least 1-2**-lambda
def find_num_csprng_bits(n,p,lam):
    bitlen_elem = bits_for_element_mod(p)
    pr_succ = mp.mpf(p/2**bitlen_elem)
    pr_insucc = 1-pr_succ
    # we at least need n attempts in rejection sampling
    n_attempts = n
    # iterate until the cumulative probability of getting enough successes is
    # higher than 1-2**-lambda
    cumul_pr = mp.mpf(0)
    while(cumul_pr < 1-mp.mpf(2)**-lam):
        n_attempts += 1
        cumul_pr = mp.mpf(0)
        # add the probabilities of all the cases where at least n successes
        # take place, i.e., at least n good values are sampled
        for n_ok in range(n, n_attempts+1):
            cumul_pr += mp.mpf(comb(n_attempts,n_ok))* mp.power(pr_succ,n_ok) * mp.power(pr_insucc,(n_attempts-n_ok))
    # one bit per attempt, plus the ones to represent the element
    total_bits_required = n_attempts+ n*(bitlen_elem-1)
    return total_bits_required

# find number of bits to draw from the CSPRNG to obtain at least t-w distinct
# positions in a t bits long bitstring via rejection sampling,
# with Pr at least 1-2**-lambda
def find_num_csprng_bits_cw_string(t,p,lam):
    n_iter_FY = t-1
    successes_to_achieve = n_iter_FY
    cumul_pr = mp.mpf(0)
    while(cumul_pr < 1-mp.mpf(2)**-lam):
        n_iter_FY += 1
        # discrete distribution w/range representing number of succesfully placed
        # positions
        discrete_prob = [mp.mpf(0)]*(successes_to_achieve+1)
        discrete_prob[0] = mp.mpf(1)
        for step in range(n_iter_FY):
            next_discrete_prob = [mp.mpf(0)]*(successes_to_achieve+1)
            for n_succ in range(successes_to_achieve):
                remaining = successes_to_achieve-n_succ
                bits_for_rem = bits_to_represent(remaining)
                # probability that a binary string is an acceptable index in 0...remaining-1
                pr_ok = mp.mpf(remaining)/mp.mpf(2**bits_for_rem)
                pr_ko = 1-pr_ok
                next_discrete_prob[n_succ]   += discrete_prob[n_succ] * mp.mpf(pr_ko)
                next_discrete_prob[n_succ+1] += discrete_prob[n_succ] * mp.mpf(pr_ok)
            # mass of t successes is preserved and added
            next_discrete_prob[successes_to_achieve] += discrete_prob[successes_to_achieve]
            discrete_prob = next_discrete_prob
        cumul_pr = discrete_prob[successes_to_achieve]
        # print(f"natt {n_iter_FY}, prob {mp.log(mp.mpf(1)-cumul_pr,2)}")
    remaining_required_bits = 0
    for i in range(t):
        remaining_required_bits += bits_to_represent(i)-1
    return n_iter_FY+remaining_required_bits


# /* level offsets contains the indexes of the first seed of a given level,
#  * nodes per level contains the number of nodes for each level. All these can
#  * be precomputed and declared as constants: a runtime computation is provided for
#  * reference purposes. The function returns the total number of tree nodes for
#  * VLA allocation */

def compute_nodes_per_level_and_offsets(T):
    nodes_per_level = []
    nodes_curr_level = T
    total_nodes = 0
    for level in range(ceil(log2(T)),-1,-1):
        nodes_per_level = [nodes_curr_level] + nodes_per_level
        total_nodes += nodes_curr_level
        nodes_curr_level = ceil(nodes_curr_level/2)

    cumulative_offset = 0
    total_saved_nodes = 0
    level_offsets = []
    # Level 0, the root has no previous levels, no missing nodes before it
    missing_nodes_before_level = []
    cumul_saved_nodes = []
    for level in range(ceil(log2(T))+1):
        level_offsets.append(cumulative_offset)
        cumulative_offset += nodes_per_level[level]
        if (level == 0):
            missing_nodes_before_level.append(0)
        else:
            missing_nodes_before_level.append(total_saved_nodes)
        total_saved_nodes += 2**level-nodes_per_level[level]

        cumul_saved_nodes.append(total_saved_nodes)

    return total_nodes,total_nodes-nodes_per_level[-1],nodes_per_level,level_offsets,missing_nodes_before_level

def worst_case_seed_tree_cost(t,w):
    num_not_to_reveal = t-w
    return ceil(num_not_to_reveal* log2(t/(num_not_to_reveal)))

def print_tree_defines(nodes_to_store,npl,mnbl,num_nodes):
    print(f"#define TREE_NODES_TO_STORE {nodes_to_store}")
    print(f"#define NUM_NODES_SEED_TREE {num_nodes}")
    c_array_npl = repr(npl).replace("[","{").replace("]","}")
    print(f"#define NODES_PER_LEVEL_ARRAY {c_array_npl}")
    c_array_mnbl = repr(mnbl).replace("[","{").replace("]","}")
    print(f"#define MISSING_NODES_BEFORE_LEVEL_ARRAY {c_array_mnbl}")



if __name__=="__main__":
    # arbitrary precision floats mantissa precision (in decimal digits)
    mp.dps = 200
    # check that a security level is provided
    if (len(argv)< 2):
        print("Script computing the auxiliary parameters for CROSS")
        print(f"Usage:{argv[0]} <CSV with code parameters>")
        print(f"Expected CSV header: lambda,opt,p,z,n,m,t")
        print("opt is one out of SPEED,BALANCED,SIG_SIZE")
        print(f"m=0 marks a RSDP instance")
        exit()

    try:
        parameter_file = open(argv[1],"r")
    except:
        print(f"parameter file {argv[1]} not found")

    print("\n/* Derived parameters computed via compute_derived_parameters.py */")
    csv_field_names=["lambda","opt","p","z","n","k","m","t","w"]
    csvreader = csv.DictReader(parameter_file,fieldnames=csv_field_names)
    for line in csvreader:
        if (line["lambda"]=="lambda"):
            continue
        seclevel = int(line["lambda"])
        opt = line["opt"]
        p = int(line["p"])
        z = int(line["z"])
        n = int(line["n"])
        k = int(line["k"])
        m = int(line["m"])
        t = int(line["t"])
        w = int(line["w"])

        category_strings = {128: "CATEGORY_1",192: "CATEGORY_3",256: "CATEGORY_5"}

        if(seclevel == 128) and (m == 0) and (opt == "SPEED"):
            print(f"#if ( defined({category_strings[seclevel]}) ",end='')
        else :
            print(f"\n#elif ( defined({category_strings[seclevel]}) ",end='')

        if( m == 0 ):
            print(f"&& defined(RSDP) ",end='')
        else :
            print(f"&&  defined(RSDPG) ",end='')

        print(f" && defined({opt}) )")

        num_nodes,_,npl,_,mnbl = compute_nodes_per_level_and_offsets(t)
        nodes_to_store = worst_case_seed_tree_cost(t,w)
        print_tree_defines(nodes_to_store,npl,mnbl,num_nodes)

        # u' is n elems mod p
        bits_uprime = find_num_csprng_bits(n,p,seclevel)
        print(f"#define BITS_N_ZQ_CT_RNG {bits_uprime}")
        bits_beta = find_num_csprng_bits(t,p-1,seclevel)
        print(f"#define BITS_BETA_ZQSTAR_CT_RNG {bits_beta}")

        bits_V_tr = find_num_csprng_bits((n-k)*k,p,seclevel)
        print(f"#define BITS_V_CT_RNG {bits_V_tr}")
        if (m==0):
            # eta is n elems mod z
            bits_etaprime = find_num_csprng_bits(n,z,seclevel)
            tot=bits_uprime+bits_etaprime
            print(f"#define BITS_N_ZZ_CT_RNG {bits_etaprime}")
        else:
            bits_W = find_num_csprng_bits((n-m)*m,z,seclevel)
            print(f"#define BITS_W_CT_RNG {bits_W}")
            # zeta is m elems mod z
            bits_zetaprime = find_num_csprng_bits(m,z,seclevel)
            tot=bits_uprime+bits_zetaprime
            print(f"#define BITS_M_ZZ_CT_RNG {bits_zetaprime}")
        num_bits_cwstring = find_num_csprng_bits_cw_string(t,p,seclevel)
        print(f"#define BITS_CWSTR_RNG {num_bits_cwstring}")
    print("\n#endif")

