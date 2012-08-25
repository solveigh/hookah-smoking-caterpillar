#!/bin/bash
set -e
set -u

#remove old files first
#rm -f *

#run generation script with appropriate parameters
#N=8, HP min
python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K8HP75minSPmin --num_prios 8 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K8HP75minSPmax --num_prios 8 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K8HP75minSPvar --num_prios 8 --perc_high 0.75


python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K8HP25minSPmin --num_prios 8 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K8HP25minSPmax --num_prios 8 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K8HP25minSPvar --num_prios 8 --perc_high 0.25


#N=3, HP min
python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K3HP75minSPmin --num_prios 3 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K3HP75minSPmax --num_prios 3 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K3HP75minSPvar --num_prios 3 --perc_high 0.75


python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K3HP25minSPmin --num_prios 3 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K3HP25minSPmax --num_prios 3 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 64 --hp_p_size_max 64 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K3HP25minSPvar --num_prios 3 --perc_high 0.25

mkdir HP64
mv K8HP* HP64
mv K3HP* HP64



#N=8, HP max
python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K8HP75maxSPmin --num_prios 8 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K8HP75maxSPmax --num_prios 8 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K8HP75maxSPvar --num_prios 8 --perc_high 0.75


python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K8HP25maxSPmin --num_prios 8 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K8HP25maxSPmax --num_prios 8 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K8HP25maxSPvar --num_prios 8 --perc_high 0.25


#N=3, HP max
python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K3HP75maxSPmin --num_prios 3 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K3HP75maxSPmax --num_prios 3 --perc_high 0.75

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K3HP75maxSPvar --num_prios 3 --perc_high 0.75


python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 64 --out_file K3HP25maxSPmin --num_prios 3 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 1500 --sp_p_size_max 1500 --out_file K3HP25maxSPmax --num_prios 3 --perc_high 0.25

python generateData_parser.py --num_p 10000 --hp_p_size_min 1200 --hp_p_size_max 1200 --sp_p_size_min 64 --sp_p_size_max 1500 --out_file K3HP25maxSPvar --num_prios 3 --perc_high 0.25

mkdir HP1200
mv K8HP* HP1200
mv K3HP* HP1200
