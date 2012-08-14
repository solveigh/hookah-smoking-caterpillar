#! /bin/sh

# K8HP75minSPmin	Scenario I
# K8HP75minSPmax	Scenario II
# K8HP75minSPvar	Scenario III
# K8HP25minSPmin	Scenario IV
# K8HP25minSPmax	Scenario V
# K8HP25minSPvar	Scenario VI

# K8HP75maxSPmin	Scenario VII
# K8HP75maxSPmax	Scenario VIII
# K8HP75maxSPvar	Scenario IX
# K8HP25maxSPmin	Scenario X
# K8HP25maxSPmax	Scenario XI
# K8HP25maxSPvar	Scenario XII

# K3HP75minSPmin	Scenario I
# K3HP75minSPmax	Scenario II
# K3HP75minSPvar	Scenario III
# K3HP25minSPmin	Scenario IV
# K3HP25minSPmax	Scenario V
# K3HP25minSPvar	Scenario VI

# K3HP75maxSPmin	Scenario VII
# K3HP75maxSPmax	Scenario VIII
# K3HP75maxSPvar	Scenario IX
# K3HP25maxSPmin	Scenario X
# K3HP25maxSPmax	Scenario XI
# K3HP25maxSPvar	Scenario XII

# als: FCFS, RR, WRR, SQF, LQF, WFQ, FQSW
#parse .tex-files dropped_perc_<alg>.csv
#parse .tex-files times_<alg>.csv


# sed -i 's/old-word/new-word/g' *.txt
#K=8
sed -i 's/^K8HP75minSPmin/Scenario \\RNum{1}/' dropped_perc_*
sed -i 's/^K8HP75minSPmax/Scenario \\RNum{2}/' dropped_perc_*
sed -i 's/^K8HP75minSPvar/Scenario \\RNum{3}/' dropped_perc_*
sed -i 's/^K8HP25minSPmin/Scenario \\RNum{4}/' dropped_perc_*
sed -i 's/^K8HP25minSPmax/Scenario \\RNum{5}/' dropped_perc_*
sed -i 's/^K8HP25minSPvar/Scenario \\RNum{6}/' dropped_perc_*

sed -i 's/^K8HP75maxSPmin/Scenario \\RNum{7}/' dropped_perc_*
sed -i 's/^K8HP75maxSPmax/Scenario \\RNum{8}/' dropped_perc_*
sed -i 's/^K8HP75maxSPvar/Scenario \\RNum{9}/' dropped_perc_*
sed -i 's/^K8HP25maxSPmin/Scenario \\RNum{10}/' dropped_perc_*
sed -i 's/^K8HP25maxSPmax/Scenario \\RNum{11}/' dropped_perc_*
sed -i 's/^K8HP25maxSPvar/Scenario \\RNum{12}/' dropped_perc_*


sed -i 's/^K3HP75minSPmin/Scenario \\RNum{1}/' dropped_perc_*
sed -i 's/^K3HP75minSPmax/Scenario \\RNum{2}/' dropped_perc_*
sed -i 's/^K3HP75minSPvar/Scenario \\RNum{3}/' dropped_perc_*
sed -i 's/^K3HP25minSPmin/Scenario \\RNum{4}/' dropped_perc_*
sed -i 's/^K3HP25minSPmax/Scenario \\RNum{5}/' dropped_perc_*
sed -i 's/^K3HP25minSPvar/Scenario \\RNum{6}/' dropped_perc_*

sed -i 's/^K3HP75maxSPmin/Scenario \\RNum{7}/' dropped_perc_*
sed -i 's/^K3HP75maxSPmax/Scenario \\RNum{8}/' dropped_perc_*
sed -i 's/^K3HP75maxSPvar/Scenario \\RNum{9}/' dropped_perc_*
sed -i 's/^K3HP25maxSPmin/Scenario \\RNum{10}/' dropped_perc_*
sed -i 's/^K3HP25maxSPmax/Scenario \\RNum{11}/' dropped_perc_*
sed -i 's/^K3HP25maxSPvar/Scenario \\RNum{12}/' dropped_perc_*

#K=3
sed -i 's/^K8HP75minSPmin/Scenario \\RNum{1}/' times_*
sed -i 's/^K8HP75minSPmax/Scenario \\RNum{2}/' times_*
sed -i 's/^K8HP75minSPvar/Scenario \\RNum{3}/' times_*
sed -i 's/^K8HP25minSPmin/Scenario \\RNum{4}/' times_*
sed -i 's/^K8HP25minSPmax/Scenario \\RNum{5}/' times_*
sed -i 's/^K8HP25minSPvar/Scenario \\RNum{6}/' times_*

sed -i 's/^K8HP75maxSPmin/Scenario \\RNum{7}/' times_*
sed -i 's/^K8HP75maxSPmax/Scenario \\RNum{8}/' times_*
sed -i 's/^K8HP75maxSPvar/Scenario \\RNum{9}/' times_*
sed -i 's/^K8HP25maxSPmin/Scenario \\RNum{10}/' times_*
sed -i 's/^K8HP25maxSPmax/Scenario \\RNum{11}/' times_*
sed -i 's/^K8HP25maxSPvar/Scenario \\RNum{12}/' times_*


sed -i 's/^K3HP75minSPmin/Scenario \\RNum{1}/' times_*
sed -i 's/^K3HP75minSPmax/Scenario \\RNum{2}/' times_*
sed -i 's/^K3HP75minSPvar/Scenario \\RNum{3}/' times_*
sed -i 's/^K3HP25minSPmin/Scenario \\RNum{4}/' times_*
sed -i 's/^K3HP25minSPmax/Scenario \\RNum{5}/' times_*
sed -i 's/^K3HP25minSPvar/Scenario \\RNum{6}/' times_*

sed -i 's/^K3HP75maxSPmin/Scenario \\RNum{7}/' times_*
sed -i 's/^K3HP75maxSPmax/Scenario \\RNum{8}/' times_*
sed -i 's/^K3HP75maxSPvar/Scenario \\RNum{9}/' times_*
sed -i 's/^K3HP25maxSPmin/Scenario \\RNum{10}/' times_*
sed -i 's/^K3HP25maxSPmax/Scenario \\RNum{11}/' times_*
sed -i 's/^K3HP25maxSPvar/Scenario \\RNum{12}/' times_*
