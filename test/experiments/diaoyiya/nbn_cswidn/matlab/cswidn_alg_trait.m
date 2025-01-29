% 获取指定文件夹路径
folderPath = '\\172.24.242.8\share\Student\2018\YiyaDiao\code_total\data\paper_cswidn\cswidn_algorithm_runProcess_network4\'; % 将这里替换为实际的文件夹路径
outputdir ='\\172.24.24.151\f\DiaoYiya\cswidn_exp\cswidn_figures_alg_trait4/';
mkdir(outputdir);

% 获取文件夹下所有文件名
fileNames = dir(fullfile(folderPath, '*_network.txt'));

% 遍历文件名数组
for i = 1:length(fileNames)
    % 获取完整文件路径
    filePath = fullfile(folderPath, fileNames(i).name);
    display(filePath);
    display(fileNames(i).name);
    curfilename = fileNames(i).name(1:end-length('_network.txt'));
    display(curfilename);
    
    drawNBN(folderPath, outputdir, curfilename);

    % 读取文件内容，可以根据文件的具体格式选择合适的读取函数
%    data = importdata(filePath);
    % 在这里可以对读取的数据进行处理
end