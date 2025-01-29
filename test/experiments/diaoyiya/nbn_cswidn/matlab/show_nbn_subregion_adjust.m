readdir= '//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data/paper_com_experiment_data/totalTsp/eax_lkh_nbnData_2/';
readdir= '//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data/paper_com_experiment_data/totalTsp/nbn_data_eax_sampling_100000_nbnNetwork3/';
readdir= '//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data/paper_com_experiment_data/totalTsp/nbn_data_eax_sampling_1000000_final_nbnNetworks/';
readdir= '//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data/paper_com_experiment_data/totalTsp/nbn_data_eax_sampling_1000000_final_nbnNetworks/';
readdir= '//172.24.207.203/share/2018/diaoyiya/ofec-data/paper_com_experiment_data/totalTsp/nbn_data_eax_sampling_1000000_final_nbnNetworks_remote/';

outputdir ='//172.24.24.151/e/DiaoYiya/paper_com_experiment_data/total_tsp/nbn_subproblem_figures_adjust/';

mkdir(outputdir);
str = [ "u574","5955" "2202"  "1281" "6702" "6717" ];
str = ["5955", "u574", "2202"];
str = ["5955"];
strSize = size(str);
strSize= strSize(1,2);

basicNodeSize = 8;
numColor = 1000;

nbn_subfixs= ["eaxRun", "randomSamples"];
nbn_subfixs= ["randomSamples"];
nbn_subfixsSize = size(nbn_subfixs);
nbn_subfixsSize= nbn_subfixsSize(1,2);
neighborK_subfixs = ["3", "6", "12", "25", "50", "100"];
neighborK_subfixs = ["12"];
neighborK_subfixsSize = size(neighborK_subfixs);
neighborK_subfixsSize= neighborK_subfixsSize(1,2);

for taskId= 1:strSize
clf;
tspname = str(1,taskId);
    for nameId= 1:nbn_subfixsSize
        filename=  tspname+"_"+ nbn_subfixs(1,nameId);
        disp(filename);
        try
       %  drawNBN(readdir, outputdir, filename);
       catch ME
        %    disp(ME.identifier);
        end
    end
    
    for nameId= 1:neighborK_subfixsSize
        filename=  tspname+"_randomSamples_neighborK_"+ neighborK_subfixs(1,nameId);
        disp(filename);
        try
          drawNBN_adjust(readdir, outputdir, filename);
        catch ME
     %       disp(ME.identifier);
        end
    end
    

end
