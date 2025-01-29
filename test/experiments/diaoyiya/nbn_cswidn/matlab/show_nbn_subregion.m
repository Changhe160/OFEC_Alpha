readdir ="\\172.24.24.151\e\DiaoYiya\code\data\ofec-data\paper_com_experiment_data\totalTsp3\nbn_subregion_centers2\optima_network\";
readdir= "\\172.24.24.151\e\DiaoYiya\code\data\ofec-data\paper_com_experiment_data\totalTsp3\nbn_subregion_aournd_center_hnswRnd_improveAccur_network\";
readdir= "\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_com_experiment_data\totalTsp3\nbn_subregion_4optima_network\";
readdir ="\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_com_experiment_data\totalTsp3\nbn_subregion_6\optima_network\";
readdir ="\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_com_experiment_data\totalTsp3\nbn_subregion_lkh_fail\merge_network\";
readdir = "\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_com_experiment_data\totalTsp\nbn_subregion_two_funnel\optima_network\";
readdir = "\\172.24.24.151\e\DiaoYiya\code\data\ofec-data\paper_com_experiment_data\totalTsp\nbn_subregion_two_funnel2_test\optima_network\";
readdir ="\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_com_experiment_data\totalTsp\nbn_subregion_two_funnel2_linux\optima_network\";
readdir = "\\172.24.24.151\e\DiaoYiya\code\data\ofec-data\paper_com_experiment_data\totalTsp\nbn_subregion_two_funnel2_winServer\optima_network\";
readdir = "\\172.24.34.11\share\2018\diaoyiya\ofec-data\paper_com_experiment_data\totalTsp\nbn_sub_region_seqRand_network\";
readdir = "\\172.24.24.151\e\DiaoYiya\code\data\ofec-data\paper_com_experiment_data\totalTsp3\nbn_subregion_centers\merge_network\";

outputdir ='//172.24.24.151/e/DiaoYiya/paper_com_experiment_data/total_tsp/nbn_sub_region_solVec_hnswModelequalBetter_2_networkFigure2/';
outputdir ='//172.24.24.151/e/DiaoYiya/paper_com_experiment_data/total_tsp/nbn_sub_region_lkh_fail2/';
outputdir ='//172.24.24.151/e/DiaoYiya/paper_com_experiment_data/total_tsp/nbn_center_two_funnel_nobridge/';


mkdir(outputdir);
str = [ "u574","5955" "2202"  "1281" "6702" "6717" ];
str = ["5955", "u574", "2202"];
%str = [ "5955"];
%str = ["5955", "2202"];
strSize = size(str);
strSize= strSize(1,2);

basicNodeSize = 8;
numColor = 1000;

nbn_subfixs= ["eaxRun", "randomSamples"];
nbn_subfixs= ["randomSamples"];
nbn_subfixsSize = size(nbn_subfixs);
nbn_subfixsSize= nbn_subfixsSize(1,2);
neighborK_subfixs = ["3", "6", "12", "25", "50", "100"];
neighborK_subfixs = [ "10", "30","50"];
neighborK_subfixs = [ "35"];
neighborK_subfixsSize = size(neighborK_subfixs);
neighborK_subfixsSize= neighborK_subfixsSize(1,2);

for taskId= 1:strSize
clf;
tspname = str(1,taskId);
    % for nameId= 1:nbn_subfixsSize
    %     filename=  tspname+"_"+ nbn_subfixs(1,nameId);
    %     disp(filename);
    %     try
    %      drawNBN(readdir, outputdir, filename);
    %    catch ME
    %         disp(ME.identifier);
    %     end
    % end
    
    for nameId= 1:neighborK_subfixsSize
        filename=  tspname+"_randomSamples_neighborK_"+ neighborK_subfixs(1,nameId);
    %    filename=  tspname+"_randomSamples_solId_2_neighborK_"+ neighborK_subfixs(1,nameId)+"_numSamples_1000000";
        disp(filename);
      %drawNBN(readdir, outputdir, filename);
        try
            drawNBN(readdir, outputdir, filename);
        catch ME
            disp(ME.identifier);
        end
    end
    

end
