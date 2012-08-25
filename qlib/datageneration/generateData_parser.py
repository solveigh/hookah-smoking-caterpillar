#!/usr/bin/python2


import random

from optparse import OptionParser 
 
parser = OptionParser() 

parser.add_option("--num_p", dest="num_p",
    help="Number of packets",
    default="100000", type="int")
parser.add_option("--hp_p_size_min", dest="hp_p_size_min",
    help="High priority minimal packet size",
    default=64, type="int")
parser.add_option("--hp_p_size_max", dest="hp_p_size_max",
    help="High priority maximal packet size",
    default=1400, type="int")
parser.add_option("--sp_p_size_min", dest="sp_p_size_min",
    help="Minimal packet size",
    default=64, type="int")
parser.add_option("--sp_p_size_max", dest="sp_p_size_max",
    help="Maximal packet size",
    default=1400, type="int")
parser.add_option("--out_file", dest="out_file",
    help="Name of output file",
    default="data.txt", type="string")
parser.add_option("--num_prios", dest="num_prios",
    help="Number of priority classes",
    default="8", type="int")
parser.add_option("--perc_high", dest="perc_high",
    help="Percentage high",
    default="0.75", type="float")

(options, args) = parser.parse_args() 

print 'Options chosen: '
print 'Number of packets: ', options.num_p
print 'Min. high priority packet size: ', options.hp_p_size_min
print 'Max. high priority packet size: ', options.hp_p_size_max
print 'Min. standard priority packet size: ', options.sp_p_size_min 
print 'Max. standard priority packet size: ', options.sp_p_size_max
print 'Output file name: ', options.out_file
print 'Number of priority classes: ', options.num_prios
print 'Percentage of high priority packets: ', options.perc_high


###
### CONFIG
###
total_packages = options.num_p
percentage_high = options.perc_high
if options.num_prios==3:
    prios_high = [2, ] # an array of all high prios 
    prios_other = [0, 1] # = range(2)

if options.num_prios==8:
    prios_high = [7, ] # an array of all high prios 
    prios_other = [0, 1, 2, 3, 4, 5, 6] # = range(7)


sizes_high = [options.hp_p_size_min, options.hp_p_size_max] # min, max
sizes_other = [options.sp_p_size_min,options.sp_p_size_max] # min, max

# nothing to configure below
f = open(options.out_file, 'a')

random.seed(1)

def select(prios, sizes):
    """generate tuple prio,size"""
    prio = random.choice(prios)   # returns one of the available priorities
    size = random.randint(*sizes) # random integer min <= result <= max
    # the star is a shortcut to random.randint(sizes[0], sizes[1])
    return (prio,size)

for i in range(total_packages):
    high_low = random.random()
    # high_low is [0,1.0)
    if high_low < percentage_high:
        prio, size = select(prios_high, sizes_high)
    else:
        prio, size = select(prios_other, sizes_other)
    #print "%d %4d" % (prio,size)
    f.write("%d %4d\n" % (prio,size))
