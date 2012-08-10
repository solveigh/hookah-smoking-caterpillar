#! /bin/sh

# N8HP75minSPmin	Scenario I
# N8HP75minSPmax	Scenario II
# N8HP75minSPvar	Scenario III
# N8HP25minSPmin	Scenario IV
# N8HP25minSPmax	Scenario V
# N8HP25minSPvar	Scenario VI

# N8HP75maxSPmin	Scenario VII
# N8HP75maxSPmax	Scenario VIII
# N8HP75maxSPvar	Scenario IX
# N8HP25maxSPmin	Scenario X
# N8HP25maxSPmax	Scenario XI
# N8HP25maxSPvar	Scenario XII

# N3HP75minSPmin	Scenario I
# N3HP75minSPmax	Scenario II
# N3HP75minSPvar	Scenario III
# N3HP25minSPmin	Scenario IV
# N3HP25minSPmax	Scenario V
# N3HP25minSPvar	Scenario VI

# N3HP75maxSPmin	Scenario VII
# N3HP75maxSPmax	Scenario VIII
# N3HP75maxSPvar	Scenario IX
# N3HP25maxSPmin	Scenario X
# N3HP25maxSPmax	Scenario XI
# N3HP25maxSPvar	Scenario XII

# als: FCFS, RR, WRR, SQF, LQF, WFQ, FQSW
#parse .tex-files dropped_perc_<alg>.csv
#parse .tex-files times_<alg>.csv


# sed -i 's/old-word/new-word/g' *.txt
#K=8
sed -i 's/^N8HP75minSPmin/Scenario \\RNum{1}/' dropped_perc_*
sed -i 's/^N8HP75minSPmax/Scenario \\RNum{2}/' dropped_perc_*
sed -i 's/^N8HP75minSPvar/Scenario \\RNum{3}/' dropped_perc_*
sed -i 's/^N8HP25minSPmin/Scenario \\RNum{4}/' dropped_perc_*
sed -i 's/^N8HP25minSPmax/Scenario \\RNum{5}/' dropped_perc_*
sed -i 's/^N8HP25minSPvar/Scenario \\RNum{6}/' dropped_perc_*

sed -i 's/^N8HP75maxSPmin/Scenario \\RNum{7}/' dropped_perc_*
sed -i 's/^N8HP75maxSPmax/Scenario \\RNum{8}/' dropped_perc_*
sed -i 's/^N8HP75maxSPvar/Scenario \\RNum{9}/' dropped_perc_*
sed -i 's/^N8HP25maxSPmin/Scenario \\RNum{10}/' dropped_perc_*
sed -i 's/^N8HP25maxSPmax/Scenario \\RNum{11}/' dropped_perc_*
sed -i 's/^N8HP25maxSPvar/Scenario \\RNum{12}/' dropped_perc_*


sed -i 's/^N3HP75minSPmin/Scenario \\RNum{1}/' dropped_perc_*
sed -i 's/^N3HP75minSPmax/Scenario \\RNum{2}/' dropped_perc_*
sed -i 's/^N3HP75minSPvar/Scenario \\RNum{3}/' dropped_perc_*
sed -i 's/^N3HP25minSPmin/Scenario \\RNum{4}/' dropped_perc_*
sed -i 's/^N3HP25minSPmax/Scenario \\RNum{5}/' dropped_perc_*
sed -i 's/^N3HP25minSPvar/Scenario \\RNum{6}/' dropped_perc_*

sed -i 's/^N3HP75maxSPmin/Scenario \\RNum{7}/' dropped_perc_*
sed -i 's/^N3HP75maxSPmax/Scenario \\RNum{8}/' dropped_perc_*
sed -i 's/^N3HP75maxSPvar/Scenario \\RNum{9}/' dropped_perc_*
sed -i 's/^N3HP25maxSPmin/Scenario \\RNum{10}/' dropped_perc_*
sed -i 's/^N3HP25maxSPmax/Scenario \\RNum{11}/' dropped_perc_*
sed -i 's/^N3HP25maxSPvar/Scenario \\RNum{12}/' dropped_perc_*

#K=3
sed -i 's/^N8HP75minSPmin/Scenario \\RNum{1}/' times_*
sed -i 's/^N8HP75minSPmax/Scenario \\RNum{2}/' times_*
sed -i 's/^N8HP75minSPvar/Scenario \\RNum{3}/' times_*
sed -i 's/^N8HP25minSPmin/Scenario \\RNum{4}/' times_*
sed -i 's/^N8HP25minSPmax/Scenario \\RNum{5}/' times_*
sed -i 's/^N8HP25minSPvar/Scenario \\RNum{6}/' times_*

sed -i 's/^N8HP75maxSPmin/Scenario \\RNum{7}/' times_*
sed -i 's/^N8HP75maxSPmax/Scenario \\RNum{8}/' times_*
sed -i 's/^N8HP75maxSPvar/Scenario \\RNum{9}/' times_*
sed -i 's/^N8HP25maxSPmin/Scenario \\RNum{10}/' times_*
sed -i 's/^N8HP25maxSPmax/Scenario \\RNum{11}/' times_*
sed -i 's/^N8HP25maxSPvar/Scenario \\RNum{12}/' times_*


sed -i 's/^N3HP75minSPmin/Scenario \RNum{1}/' times_*
sed -i 's/^N3HP75minSPmax/Scenario \RNum{2}/' times_*
sed -i 's/^N3HP75minSPvar/Scenario \RNum{3}/' times_*
sed -i 's/^N3HP25minSPmin/Scenario \RNum{4}/' times_*
sed -i 's/^N3HP25minSPmax/Scenario \RNum{5}/' times_*
sed -i 's/^N3HP25minSPvar/Scenario \RNum{6}/' times_*

sed -i 's/^N3HP75maxSPmin/Scenario \RNum{7}/' times_*
sed -i 's/^N3HP75maxSPmax/Scenario \RNum{8}/' times_*
sed -i 's/^N3HP75maxSPvar/Scenario \RNum{9}/' times_*
sed -i 's/^N3HP25maxSPmin/Scenario \RNum{10}/' times_*
sed -i 's/^N3HP25maxSPmax/Scenario \RNum{11}/' times_*
sed -i 's/^N3HP25maxSPvar/Scenario \RNum{12}/' times_*
